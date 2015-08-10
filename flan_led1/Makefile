include ../Template.mk

CPU_TYPE= attiny2313
CPU_FREQ= 8000000
AVRDUDE_PART = t2313

TARGET	= led_test
SOURCES = led_test.c flan_led_intr.c
OBJECTS = $(SOURCES:%.c=%.o)

all:: $(TARGET).hex

$(TARGET).hex: $(TARGET).elf

$(TARGET).dump: $(TARGET).elf

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET).elf $(OBJECTS)
