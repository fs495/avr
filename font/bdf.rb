#!/usr/bin/env ruby
# -*- coding: utf-8 -*-

require './charcode.rb'

#----------------------------------------------------------------------

# BDFファイルをロードし、以下のメンバを持つグリフ情報HashのArrayを返す
# :code, :w, :h, :ptn
#
# BDFファイル中のコードポイントはJISコードとする。
# unicodeがtrueの場合は、Array格納時にUnicodeコードポイントに変換する
def load_bdffile(bdffile, unicode = false)
    gbbxw, gbbxh = nil, nil
    bbxw, bbxh, bytes_per_row = nil, nil, nil
    in_bitmap = false
    ptn, code = nil, nil
    unicode_tab = load_sjis_to_unicode_table() if unicode
    ret = []

    open(bdffile) do |fd|
	while buf = fd.gets
	    buf.chomp!

	    if in_bitmap && buf == 'ENDCHAR'
		raise "縦サイズがBBX文と異なる" if ptn.length != gbbxh
		in_bitmap = false
		tmp = {:code => code, :w => bbxw, :h => bbxh, :ptn => ptn}
		if unicode && code >= 0x100
		    sjis = jis_to_sjis(tmp[:code])
		    tmp[:orig_code] = tmp[:code]
		    tmp[:code] = unicode_tab[sjis]
		end
		ret << tmp

	    elsif in_bitmap
		raise "横サイズがBBX文と異なる" if buf.length != bytes_per_row * 2
		ptn << buf.hex

	    elsif buf =~ /^FONTBOUNDINGBOX (\d+) (\d+) (\d+) (-?\d+)/
		gbbxw, gbbxh = $1.to_i, $2.to_i

	    elsif buf =~ /^STARTCHAR/
		code = nil
		ptn = []

	    elsif buf =~ /^ENCODING (\d+)/
		code = $1.to_i # BDFファイル中なのでJIS区点コード

	    elsif buf =~ /^BBX (\d+) (\d+) (\d+) (-?\d+)/
		bbxw, bbxh = $1.to_i, $2.to_i
		bytes_per_row = (bbxw + 7) / 8
		raise "縦サイズが固定値ではない" if bbxh != gbbxh

	    elsif buf =~ /^BITMAP/
		in_bitmap = true
	    end
	end
    end
    return ret
end

TESTFONT = 'intlfonts-1.2.1/Japanese.X/8x16rk.bdf'
def test_load_bdf()
    # 8x16rkは190文字のデータがある
    bdf = load_bdffile(TESTFONT)
    raise if bdf.size != 190
    puts "test_load_bdf: OK"
end

#----------------------------------------------------------------------

# ロードしたBDFファイル情報に対し、グリフ横幅が同じでかつコードポイントが連続する
# もののブロックを見つけ、その範囲情報を返す。
# 範囲情報はArrayで、その要素は[開始コード, 終了コード, 横幅]というArray。
#
def find_continuous_blocks(bdf)
    state = 0
    blocks = []
    start_code, last_code, last_width = nil, nil, nil

    bdf.sort{|x,y| x[:code] <=> y[:code]}.each do |x|
	code, bbxw, bbxh, ptn = x[:code], x[:w], x[:h], x[:ptn]
	#pp [code, start_code, last_code, last_width]

	case state
	when 0
	    state = 1
	    start_code, last_code, last_width = code, code, bbxw

	when 1
	    if code == last_code + 1 && bbxw == last_width
		last_code = code
	    else
		blocks << [start_code, last_code, last_width]
		start_code, last_code, last_width = code, code, bbxw
	    end
	end
    end
    blocks << [start_code, last_code, last_width]
    return blocks
end

def test_find_continuous_blocks()
    bdf = load_bdffile(TESTFONT)
    res = find_continuous_blocks(bdf)
    raise if res.size != 2
    raise if res[0] != [0x01, 0x7e, 8]
    raise if res[1] != [0xa0, 0xdf, 8]
    puts "test_find_continuous_blocks: OK"
end

#======================================================================

# グリフを半時計回りに90度回転する
def rotate_bitmap(ptn, width)
    height = ptn.length
    bpr = (width + 7) / 8
    work = nil
    out = []

    # 横1ラインずつスキャン
    (0 ... height).each do |y|
	#puts "in: " + ('0' * width + ptn[y].to_s(2))[-width, width]

	# 回転しつつ出力データにビットをコピーしていく
	if y % 8 == 0
	    work = [0] * width
	end
	(0 ... width).each do |x|
	    if (ptn[y] & (1 << (width - 1 - x))) == 0
		work[x] = (work[x] >> 1)
	    else
		work[x] = (work[x] >> 1) | 128
	    end
	end

	# 縦8ドットごとに出力する
	if (y + 1) % 8 == 0
	    (0 ... width).each do |x|
		#puts ('00000000' + work[x].to_s(2))[-8, 8]
	    end
	    out << work
	end
    end
    return out.flatten
end

def test_rotate_bitmap()
    ptn = [0b11110000,
	   0b11001100,
	   0b10101010,
	   0b10001000,
	   0b10000000,
	   0b11000000,
	   0b11100000,
	   0b11111111]
    expected = [0b11111111,
		0b11100011,
		0b11000101,
		0b10000001,
		0b10001110,
		0b10000010,
		0b10000100,
		0b10000000]

    # 横8 x 縦8
    res = rotate_bitmap(ptn, 8)
    raise if res != expected

    # 横8 x 縦16
    res = rotate_bitmap(ptn + [0] * 8, 8)
    raise if res != (expected + [0] * 8)

    # 横16 x 縦8
    res = rotate_bitmap(ptn, 16)
    raise if res != [0] * 8 + expected

    # 横16 x 縦8
    res = rotate_bitmap(ptn + [0] * 8, 16)
    raise if res != [0] * 8 + expected + [0] * 16

    puts "test_rotate_bitmap: OK"
end
