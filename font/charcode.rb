#!/usr/bin/env ruby
# -*- coding: utf-8 -*-

SJIS_TO_UNICODE_TAB = 'CP932.TXT'

#======================================================================
# CP932(MS Shift_JIS)からUnicodeコードポイントへの変換テーブルを得る

def load_sjis_to_unicode_table()
    tab = {}
    open(SJIS_TO_UNICODE_TAB) do |fd|
	while buf = fd.gets
	    buf.chomp!
	    if buf =~ /0x([0-9a-fA-F]+)	0x([0-9a-fA-F]+)/
		sjis, uni = $1.hex, $2.hex
		#puts "sjis:#{sjis}(0x#{sjis.to_s(16)}) -> unicode:U+#{uni.to_s(16)}"
		tab[sjis] = uni
	    end
	end
    end
    return tab
end

def test_load_sjis_to_unicode_table()
    tab = load_sjis_to_unicode_table()
    raise if tab[0x00] != 0x0000
    raise if tab[0x20] != 0x0020
    raise if tab[0x7e] != 0x007e
    raise if tab.include?(0x80)
    raise if tab[0xa1] != 0xff61
    raise if tab[0xa2] != 0xff62
    raise if tab[0x8140] != 0x3000
    raise if tab[0x8143] != 0xff0c
    raise if tab[0xfc4b] != 0x9ed1
    puts "test_load_sjis_to_unicode_table: OK"
end

#======================================================================
# JIS区点コードをShift_JISコードに変換する

def jis_to_sjis(jis)
    c1, c2 = jis >> 8, jis & 0xff
    if c1 % 2 == 0
	c1 = (c1 / 2) + 0x70;
	c2 = c2 + 0x7d;
    else
	c1 = ((c1 + 1) / 2) + 0x70;
	c2 = c2 + 0x1f;
    end
    c1 = c1 + 0x40 if c1 >= 0xa0
    c2 = c2 + 1 if c2 >= 0x7f
    return c1 << 8 | c2
end

def test_jis_to_sjis()
    raise if jis_to_sjis(0x2120) != 0x813f
    raise if jis_to_sjis(0x2121) != 0x8140
    raise if jis_to_sjis(0x2122) != 0x8141
    raise if jis_to_sjis(0x2123) != 0x8142

    raise if jis_to_sjis(0x2160) != 0x8180
    raise if jis_to_sjis(0x2220) != 0x819e
    raise if jis_to_sjis(0x2320) != 0x823f
    raise if jis_to_sjis(0x2350) != 0x826f

    raise if jis_to_sjis(0x7421) != 0xea9f
    puts "test_jis_to_sjis: OK"
end

#======================================================================

if __FILE__ == $0
    # Shift_JIS->Unicodeのコード変換テーブルを出力
    tab = load_sjis_to_unicode_table()
    ary = Array.new(65536, 65535)
    tab.each do |sjis, unicode|
	ary[sjis] = unicode
    end

    open('sjis2uni.dat', "wb") do |fd|
	# リトルエンディアン16ビット値に詰めてファイルに出力
	fd.write(ary.pack("v*"))
    end
end
