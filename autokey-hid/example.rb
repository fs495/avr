#!/usr/bin/ruby

require './scancode'
require './setkeyseq'
include ScanCode

Vendor = 0x16c0
Product = 0x05dc

Key_Codes = [
	     M_LShift, K_F,
	     0, K_R,
	     0, K_A,
	     0, K_N,
	     0, K_D,
	     0, K_L,
	     0, K_E,
	     0, K_Space,
	     M_LShift, K_S,
	     0, K_C,
	     0, K_A,
	     0, K_R,
	     0, K_L,
	     0, K_E,
	     0, K_T,
	     0, K_Space,
]

register_keyseq(Vendor, Product, Key_Codes)
