#!/usr/bin/env ruby
# -*- coding: utf-8 -*-

require './bdf.rb'
require './charcode.rb'

Object.private_methods.each do |sym|
    next if sym.to_s !~ /test_/
    send(sym)
end
