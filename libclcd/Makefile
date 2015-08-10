include ../Template.mk

CPU_TYPE= atmega168p
CPU_FREQ= 20000000
AVRDUDE_PART = m168p

TARGET	= clcd_test
SOURCES = clcd_test.c libclcd_sample.c
OBJECTS = $(SOURCES:%.c=%.o)

all:: $(TARGET).hex

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET).elf $(OBJECTS)

libclcd_sample.o: libclcd.c libclcd.h
