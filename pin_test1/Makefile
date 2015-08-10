include ../Template.mk

#CPU_TYPE= attiny2313
CPU_TYPE= atmega168p
CPU_FREQ= 8000000
#AVRDUDE_PART = t2313
AVRDUDE_PART = m168p
#AVRDUDE_FLAGS = -v -B 100
AVRDUDE_FLAGS =

TARGET	= pin_test1
SOURCES = pin_test1.c
OBJECTS = $(SOURCES:%.c=%.o)

all:: $(TARGET).hex

$(TARGET).hex: $(TARGET).elf

$(TARGET).dump: $(TARGET).elf

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET).elf $(OBJECTS)

reset_ckdiv8:
	$(MAKE) LFUSE=0xef write_lfuse
