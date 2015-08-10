#!/usr/bin/env ruby
# -*- coding: utf-8 -*-

require './bdf'
require 'pp'

BDFFONT = 'intlfonts-1.2.1/Japanese.X/8x16rk.bdf'

bdf = load_bdffile(BDFFONT)
bdf = bdf.select{|g| g[:code] >= 0x20 && g[:code] <= 0x7e}
blks = find_continuous_blocks(bdf)

raise "must be continuous" if blks.size != 1
raise "must be start == 32" if blks[0][0] != 32
raise "must be end == 126" if blks[0][1] != 126

ptns = {}
bdf.each do |g|
    raise if g[:w] != 8
    raise if g[:h] != 16

    ptns[g[:code]] = rotate_bitmap(g[:ptn], 8)
end

puts "const unsigned char font8x16[] PROGMEM = {"
(0x20 .. 0x7e).each do |code|
    puts "\t/* character '#{code.chr}', 0x#{code.to_s(16)} */"
    puts "\t" + ptns[code].join(',') + ','
end
puts "};"
