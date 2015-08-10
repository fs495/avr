# -*- coding: utf-8 -*-
# -*- ruby -*-
# -*- coding: utf-8 -*-

class MmlParser
rule

mml_file
    : mml_file mml_file_elem
    | mml_file_elem

mml_file_elem
    : '@' TRACK '[' NAME ']' '=' seq_list ';' {
	@tracks[val[3]] = val[6]
    }
    | NAME '=' {
	@mode = :literal
    } mml_elem_list ';' {
	@seqs[val[0]] = val[3]
	@mode = :global
    }

seq_list
    : seq_list NAME		{ result = val[0] << Seq.new(val[1], 1) }
    | seq_list NAME '/' NUMBER	{ result = val[0] << Seq.new(val[1], val[3]) }
    | NAME			{ result = [Seq.new(val[0], 1)] }
    | NAME '/' NUMBER		{ result = [Seq.new(val[0], val[2])] }

mml_elem_list
    : mml_elem_list music_elem	{ result = val[0] + [val[1]] }
    | mml_elem_list led_elem	{ result = val[0] << val[1] }
    | music_elem		{ result = [val[0]] }
    | led_elem			{ result = [val[0]] }

music_elem
    : tone_spec               { result = Tone.new(val[0], @length, 1, false) }
    | tone_spec           '&' { result = Tone.new(val[0], @length, 1, true) }
    | tone_spec       '.'     { result = Tone.new(val[0], @length, 1.5, false) }
    | tone_spec       '.' '&' { result = Tone.new(val[0], @length, 1.5, true) }
    | tone_spec NUMBER         { result = Tone.new(val[0], val[1], 1, false) }
    | tone_spec NUMBER     '&' { result = Tone.new(val[0], val[1], 1, true) }
    | tone_spec NUMBER '.'     { result = Tone.new(val[0], val[1], 1.5, false) }
    | tone_spec NUMBER '.' '&' { result = Tone.new(val[0], val[1], 1.5, true) }
    | 't' NUMBER	{ @tempo = val[1]; Tone::set_tempo(@tempo) }
    | 'l' NUMBER	{ @length = val[1]; result = nil }
    | 'o' NUMBER	{ @octave = val[1]; result = nil }
    | '<'		{ @octave += 1; result = nil }
    | '>'		{ @octave -= 1; result = nil }
    | 'q' NUMBER	{ result = Quote.new(val[1]) }
    | 'v' NUMBER	{ result = Volume.new(val[1]) }

tone_spec
    : note 		{ result = [@octave * 12 + val[0]] }
    # TODO: chord

note
    : base_note     { result = val[0] }
    | base_note '+' { result = val[0] + 1 }
    | base_note '-' { result = val[0] - 1 }

base_note
    : 'c' { result = 0 }
    | 'd' { result = 2 }
    | 'e' { result = 4 }
    | 'f' { result = 5 }
    | 'g' { result = 7 }
    | 'a' { result = 9 }
    | 'b' { result = 11 }

---- header
require 'strscan'

---- inner
    class Seq
	def initialize(seq, repeat)
	    @seq, @repeat = seq, repeat
	end

	def inspect
	    return "Seq(#{@seq},#{@repeat})"
	end
    end

    class Tone
	def initialize(nr, len, len_mag, tie)
	    @nr = nr
	    @steptime = 60.0 / @@tempo * 4 / len * len_mag
	    @tie = tie
	end

	def self.set_tempo(tempo)
	    @@tempo = tempo
	end

	def inspect
	    return "Tone(#{@nr},#{@steptime})"
	end
    end

    class Quote
	def initialize(q)
	    @q = q
	end

	def inspect
	    return "Quote(#{@w})"
	end
    end

    class Volume
	def initialize(v)
	    @v = v
	end

	def inspect
	    return "Volume(#{@v})"
	end
    end

    def initialize()
	@tempo, @octave, @length = 120, 4, 4
	Tone::set_tempo(@tempo)

	@tracks, @seqs = {}, {}

	@mode = :global
	@lines = []
	@scanner = nil
	@yydebug = true
    end

    def parse(lines)
	@lines = lines
	do_parse
    end

    def next_token
	while true
	    if @scanner == nil || @scanner.eos?
		line = @lines.shift

		# 取り出す行がなくなればトークンの終わりを通知
		return [false, false] if line == nil

		# コメント行なら読み直し
		next if line =~ %r"^\s*#"

		@scanner = StringScanner.new(line)
	    end

	    # ポインタ先頭の空白を読み捨てる
	    @scanner.scan(/\s*/)
	    next if @scanner.eos?

	    case @mode
	    when :global
		if @scanner.scan(/track/)
		    return [:TRACK, nil]
		elsif @scanner.scan(/([_a-zA-Z]\w*)/)
		    return [:NAME, @scanner[1]]
		elsif @scanner.scan(/(\d+)/)
		    return [:NUMBER, @scanner[1].to_i]
		else
		    @scanner.scan(/(\S)/)
		    return [@scanner[1], nil]
		end
	    when :literal
		if @scanner.scan(/(\d+)/)
		    return [:NUMBER, @scanner[1].to_i]
		else
		    @scanner.scan(/(\S)/)
		    return [@scanner[1], nil]
		end
	    end
	end
    end

    def self.normalize_seq(seq)
	res = []
	prev_tone = nil

	seq.each do |elem|
	    next if elem == nil
	    if elem.is_a?(Tone)
		if elem.tie
		    if prev_tone == nil
			prev_tone = elem
		    else
			prev_tone.steptime += elem.steptime
		    end
		else
		    res << prev_tone if prev_tone != nil
		    res << elem
		    prev_tone = nil
		end
	    else
		res << prev_tone if prev_tone != nil
		res << elem
	    end
	end
	return res
    end

---- footer

parser = MmlParser.new()
parser.parse(readlines())
