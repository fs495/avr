
all:: sjis2uni.dat font8x16.c

sjis2uni.dat:
	ruby charcode.rb

font8x16.c:
	ruby gen8x16.rb > font8x16.c

test:
	ruby unittest.rb

clean::
	rm -f sjis2uni.dat font8x16.c
