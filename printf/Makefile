include ../Template.mk

CPU_TYPE= attiny2313
CPU_FREQ= 8000000
AVRDUDE_PART = t2313

TARGET	= zprintf_test
SOURCES = zprintf_test.c libclcd_impl.c zprintf_sample.c
OBJECTS = $(SOURCES:%.c=%.o)

all:: $(TARGET).hex

$(TARGET).hex: $(TARGET).elf

$(TARGET).dump: $(TARGET).elf

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET).elf $(OBJECTS)

zprintf_sample.o: zprintf_sample.c zprintf.c zprintf.h
libclcd_impl.o: libclcd_impl.c ../libclcd/libclcd.c ../libclcd/libclcd.h

# ホスト環境下での単体テスト
test:
	gcc -o a.exe -funsigned-char zprintf_ut.c
	./a.exe

clean::
	rm -f a.exe
