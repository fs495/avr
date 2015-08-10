include ../Template.mk

CPU_TYPE= atmega168p
CPU_FREQ= 8000000
AVRDUDE_PART = m168p

TARGET	= glcd_test
SOURCES = glcd_test.c libglcd_sample.c
OBJECTS = $(SOURCES:%.c=%.o)

LFUSE = 0xf7
AVRDUDE_FLAGS = -B 1

all:: $(TARGET).hex

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET).elf $(OBJECTS)

glcd_test.o: toho-komakyo.c

toho-komakyo.c: toho-komakyo.png
	ruby img2c.rb toho-komakyo.png > toho-komakyo.c

clean::
	rm -f toho-komakyo.c

libglcd_sample.o: libglcd.c libglcd.h
