#!/usr/bin/env ruby

require 'RMagick'

ARGV.each do |file|
    extname = File.extname(file)
    basename = File.basename(file, extname)
    basename.gsub!(/[^_a-z0-9]/, '_')

    img = Magick::ImageList.new(file)

    puts "#define IMG_#{basename.upcase}_WIDTH #{img.columns}"
    puts "#define IMG_#{basename.upcase}_HEIGHT #{img.rows}"
    puts "const unsigned char img_#{basename}[] PROGMEM = {"

    data = nil
    (0 ... img.rows).each do |y|
	if y % 8 == 0
	    data = Array.new(img.columns, 0)
	end

	(0 ... img.columns).each do |x|
	    if img.pixel_color(x, y).intensity < Magick::QuantumRange / 2
		data[x] = (data[x] >> 1) | 128 # black
	    else
		data[x] = (data[x] >> 1) # white
	    end
	end

	if y % 8 == 7
	    puts data.join(', ') + ","
	end
    end

    puts "};\n"
end
