#!/usr/bin/env ruby
# https://twitter.com/sygetch/status/2063662912

require 'rubygems'
require 'thread'
require 'strscan'
require 'narray'

# ====================================================
# Mixer
# ====================================================
=begin
Class Mixer
 Manages mixing buffer and send it to the DSP or other devices.
 You can write your own wave sequencer to save sound to the file
 by overwriting output method.
=end
class Mixer
  attr_accessor :sampling_rate
  attr_accessor :default_length

  def initialize(rate, length)
    @sampling_rate = rate
    @sampling_buffer = nil
    @default_length = length
  end

  def start
    @queue ||= SizedQueue.new(2)
    @pipe = IO.popen("aplay -f dat","w")

    @th_output = Thread.start {
      looping = true
      while looping do
        buffer = @queue.shift
        if !buffer || buffer.empty? then
          looping = false
        else
          output(buffer)
        end
      end
    }
  end
 
  def join
    @th_output.join if @th_output
  end

  def mix
    @sampling_buffer ||= NArray.sfloat(@default_length * 2) # Assign double buffer size (for stereo stream)
    @written_length = yield @sampling_buffer, @default_length
  end

  def emit
    return unless @queue
    buffer = @sampling_buffer.to_i.to_a
    @queue << buffer
    @sampling_buffer = nil
    self
  end

  def output(buffer)
    print "output-#{buffer.size} bytes\n"
    @pipe.write buffer.pack("v*") if @pipe
    @pipe.flush if @pipe
  end

  def stop
    @queue << []
    join
  end
end


# ====================================================
# Track Sound Scheduler
# ====================================================
=begin
Class TrackManager
 Hold one or more TrackScheduler objects.
 This object manages the whole track schedule.
=end
class TrackManager
  attr_reader :tracks

  OSC_ID = 0
  SCHED_ID = 1

  def initialize
    @tracks = []
    @now = 0
  end

  def end_time
    @tracks.map {|track| track[SCHED_ID].end_time }.max
  end

  def reset
    @tracks.each {|track| track[SCHED_ID].reset }
    @now = 0
  end

  def play(mixer, length = (self.end_time - @now))
    print "playback #{@now} - #{@now + length}\n"
    @tracks.each {|track|
      sched = track[SCHED_ID]
      sched.seek(@now)
      sched.play(mixer, track[OSC_ID], length)
    }
    @now += length
    mixer.emit
    self
  end
end

=begin
Class TrackScheduler
 Hold set of scheduling data. Scheduling data consists of 
 [start-end time, freq, amp] data.
 This module generate sound wave acording to the above data set
 and send it to the mixer.
=end
class TrackScheduler
  START_TIME=0
  END_TIME=1
  FREQ=2
  AMP=3
  LEFT=4
  RIGHT=5
  DATA_RANGE=FREQ..RIGHT

  def initialize
    @schedule = []
    reset
  end

  def reset
    @now = 0
    @current_index = 0
    @schedule.clear
  end


  def end_time
    return 0 if @schedule.empty?
    return @schedule[-1][END_TIME]
  end


  def seek(time)
    t = @now
    if t < time then
      while @current_index < @schedule.size - 1 do
        s = @schedule[@current_index]
	if time < s[END_TIME] then
	  break
	end
	@current_index += 1
      end
    elsif t > time then
      while @current_index > 0 do
        s = @schedule[@current_index]
	if time > s[START_TIME] then
	  break
	end
	@current_index -= 1
      end
    end
    @now = time
  end

  def forward(time)
    seek(@now + time)
  end

  def current
    s = @schedule[@current_index]
    if s[START_TIME] <= @now && @now < s[END_TIME] then
      return s[DATA_RANGE]
    end
    return [0, 0]
  end

  def []=(range, values)
    schedule = [range.first, range.last]
    schedule.concat(values)
    i = 0
    if @schedule.empty? then
      @schedule << schedule
      return self
    end

    while i < @schedule.size do
      s = @schedule[i]

      if s[START_TIME] > schedule[START_TIME] then

	while i < @schedule.size do
	  s = @schedule[i]
          if s[START_TIME] < schedule[ENT_TIME] then
	    s[START_TIME] = schedule[END_TIME]
	    @schedule.delete(i) if s[START_TIME] >= s[END_TIME]
	  else
	    break
	  end
	end

        @schedule.insert(i, schedule)
	p @schedule
	return self
      end

      i += 1
    end
    @schedule << schedule
    return self
  end


  def append(len, values)
    self[end_time...(end_time+len)] = values
  end


  def play(mixer, generator, length = (self.end_time - @now))
    stop_time = @now + length

    mixer.mix {|buffer, default_length|
      buffer_idx = 0
      while @current_index < @schedule.size do
        s = @schedule[@current_index]

	if @now < s[START_TIME] then
	  @now = [s[START_TIME], stop_time].min
	end

	break if stop_time < s[START_TIME]

	if s[END_TIME] > stop_time then
	  do_mix(generator, buffer, s[FREQ], s[AMP], buffer_idx, stop_time - @now, s[LEFT], s[RIGHT])
	  buffer_idx += stop_time - @now
	  @now = stop_time
	  break
	else
	  do_mix(generator, buffer, s[FREQ], s[AMP], buffer_idx, s[END_TIME] - @now, s[LEFT], s[RIGHT])
	  buffer_idx += s[END_TIME] - @now
	  @now = s[END_TIME]
	end

	@current_index += 1
      end
    }
  end


  def do_mix(generator, buffer, freq, amp, ofs, len, left, right)
    return ofs if len < 1
    begin
      output = generator.call(freq, @now, len, nil) * amp
      tmp = NArray.sfloat(2, len)
      tmp[0, true] = output
      tmp[1, true] = output
      buffer[(2*ofs)...(2*(ofs+len))] += tmp[0...(2*len)]
    rescue IndexError => e
    end
    ofs + len
  end
  private :do_mix

end


# ====================================================
# Oscillator & Synthesizer
# ====================================================

# Basic oscillator definitions.
# returns oscillator generation procedure object.
# you can get oscillated wave by calling procedure object.
def sine_wave()
  return proc {|freq, offset, length, mod|
    input = NArray.sfloat(length).indgen!(offset) * (freq * 2 * Math::PI)
    input += mod if mod
    NMath.sin(input)
  }
end

def rectangle_wave()
  return proc {|freq, offset, length, mod|
    input = NArray.sfloat(length).indgen!(offset) * (freq * 2 * Math::PI)
    input += mod if mod
    ((NMath.sin(input) > 0).to_f - 0.5) * 2
  }
end

def triangle_wave()
  return proc {|freq, offset, length, mod|
    input = NArray.sfloat(length).indgen!(offset) * (freq * 4)
    input += mod if mod
    ((input % 4) - 2).abs - 1
  }
end

def sawtooth_wave()
  return proc {|freq, offset, length, mod|
    input = NArray.sfloat(length).indgen!(offset) * (freq * 2)
    input += mod if mod
    ((input % 2) - 1)
  }
end

# FM synthesizer definitions.
# returns synthesizer procedure object.
# you can get synthesized wave by calling procedure object.
def fm_synth_generator(func, mod_func, mod_freq_rate, mod_amp)
  return proc { |freq, offset, length, prev_mod|
    mod_func_freq = freq * mod_freq_rate
    mod = mod_func.call(mod_func_freq, offset, length, prev_mod) * mod_amp
    output = func.call(freq, offset, length, mod) 
  }
end

def sin_fm_synth(mod_freq, mod_amp)
  return fm_synth_generator(
    sine_wave,
    sine_wave,
    mod_freq, mod_amp
  )
end

def rect_fm_synth(mod_freq, mod_amp)
  return fm_synth_generator(
    rectangle_wave,
    rectangle_wave,
    mod_freq, mod_amp
  )
end

def tri_fm_synth(mod_freq, mod_amp)
  return fm_synth_generator(
    triangle_wave,
    triangle_wave,
    mod_freq, mod_amp
  )
end

def saw_fm_synth(mod_freq, mod_amp)
  return fm_synth_generator(
    sawtooth_wave,
    sawtooth_wave,
    mod_freq, mod_amp
  )
end


# ====================================================
# MML Generator
# ====================================================

=begin
Class MMLGenerator
 Convert MML into [start-end, freq, amp] data sets.
=end
class MMLGenerator
  DEFAULT_TEMPO  = 60
  DEFAULT_LENGTH = 4

  DEFAULT_OCTAVE = 4
  MIN_OCTAVE     = 0
  MAX_OCTAVE     = 8
  BASE_FREQ      = 440 #O3 A
  DEFAULT_VOLUME = 10
  MAX_VOLUME     = 15
  AMP            = 5000

  KEYS           = { "A" => 9, "B" => 11, "C" => 0, "D" => 2, "E" => 4, "F" => 5, "G" => 7 }
  KEYS_IN_OCTAVE = 12.0

  attr_accessor :tempo
  attr_accessor :default_length
  attr_accessor :octave

  def initialize(sampling_rate)
    @tempo = DEFAULT_TEMPO
    @default_length = DEFAULT_LENGTH
    @octave = DEFAULT_OCTAVE
    @volume = DEFAULT_VOLUME
    @sampling_rate = sampling_rate
    @base_C = BASE_FREQ * 2**(-9.0/12)
    @gate_length = 14
    print "BASE C=#{@base_C}\n\n"
  end

  def parse(tracks, lines)
    track_id = 0

    lines.each {|line|
      scanner = StringScanner.new(line.upcase)
      until scanner.eos? do
#        print "Scan:#{scanner.rest}\n"
        if scanner.scan(/;/) then
	  track_id += 1
	elsif scanner.scan(/\s*([A-GR])([\+\-]?)(\d+)?(\.?)\s*/) then
	  command = scanner[1]
	  mod     = scanner[2]
	  length  = scanner[3]
	  dot     = scanner[4]

          length ||= @default_length
	  length = 64 / length.to_i
	  if dot == "." then
	    length *= 1.5
	  end
	  len = @sampling_rate.to_f * length  / 16.0 * 60  / @tempo.to_f
          rest_length = len * (16.0 - @gate_length) / 16.0
	  track = tracks[track_id][TrackManager::SCHED_ID]

	  if command == "R" then
	    print "R#{64.0/length} "
	    $stdout.flush
	    key = 0
	    track.append(len.to_i, [0, 0, true, true])
	  else
	    print "#{command}"
	    key = KEYS[command]
	    case mod
	    when "+"
	      print "#"
	      key += 1
	    when "-"
	      print "b"
	      key -= 1
	    end
	    printf "%1.2g ", 64.0/length
	    $stdout.flush
	    key = 2 ** ((@octave - DEFAULT_OCTAVE) + key / KEYS_IN_OCTAVE) * @base_C / @sampling_rate
	    $stdout.flush
	    track.append((len - rest_length).to_i, [key, @volume * AMP.to_f / MAX_VOLUME, true, true])
	    track.append(rest_length.to_i, [0, 0, true, true])
	  end

	elsif scanner.scan(/T(\d+)/) then
          @tempo = scanner[1].to_i

	elsif scanner.scan(/O([0-8])/) then
	  @octave = scanner[1].to_i

	elsif scanner.scan(/\>/) then
	  @octave -= 1 if @octave > MIN_OCTAVE

	elsif scanner.scan(/\</) then
	  @octave += 1 if @octave < MAX_OCTAVE

	elsif scanner.scan(/Q(\d+)/) then

	elsif scanner.scan(/L(\d+)/) then
          length = scanner[1].to_i
          if [1, 2, 4, 8, 16, 32, 64].include?(length) then
	    @default_length = scanner[1].to_i
	  else
	    print "Invalid L options #{length}\n"
	  end
	elsif scanner.scan(/V(\d+)/) then
	  volume = scanner[1]
	  @volume = volume if volume >= 0 && volume < 15
	else
          if scanner.rest !~ /\s*\n?$/ then
            print "Parse stopped at #{scanner.rest}\n"
          end
          break
	end
      end
      print "\n"
    }
  end


end



# ====================================================
# Main routine
# ====================================================

def nested_fm_synth
  mod_func = fm_synth_generator(rectangle_wave, sine_wave, 10, 1)
  func = fm_synth_generator(triangle_wave, sawtooth_wave, 2.5, 1)
  fm_synth_generator(func, mod_func, 1, 10)
end

th_mixing = Thread.start {
  # Create track sound schedulers.
  track_manager = TrackManager.new
  generator = MMLGenerator.new(48000)

  # Nested FM Test: Create scheduler and Assign nested FM Synthesizer
  (0...3).each {|i| track_manager.tracks << [nested_fm_synth, TrackScheduler.new] }

  # Parse MML into [start-end, freq, amp] parameters.
  generator.parse(track_manager.tracks, readlines())

  # Start Mixer processor
  mixer = Mixer.new(48000, 48000)
  mixer.start

  # Generate sound wave.
  t = 0
  while t < track_manager.end_time do
    track_manager.play(mixer, mixer.default_length)
    t += mixer.default_length
  end
  mixer.stop
}

[th_mixing].each {|th| th.join }
