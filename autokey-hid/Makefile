include ../Template.mk

CPU_FREQ = 20000000
CPU_TYPE = attiny2313
AVRDUDE_PART = t2313
AVRDUDE_FLAGS =

TARGET	= firmware
C_SOURCES = firmware.c usbdrv.c
ASM_SOURCES = usbdrvasm.S
OBJECTS = $(C_SOURCES:%.c=%.o) $(ASM_SOURCES:%.S=%.o)

VUSB_DIR = ../vusb/usbdrv
VUSB_FILES = usbdrv.c usbdrvasm.S
VUSB_SOURCES = $(VUSB_FILES:%=$(VUSB_DIR)/%)

CPPFLAGS = -DF_CPU=$(CPU_FREQ) -I. -I$(VUSB_DIR)

#----------------------------------------------------------------------

all:: $(TARGET).hex

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET).elf $(OBJECTS)

$(OBJECTS): usbconfig.h

$(VUSB_FILES): $(VUSB_SOURCES)
	cp -p $(VUSB_SOURCES) .

diffconfig:
	diff -u $(VUSB_DIR)/usbconfig-prototype.h usbconfig.h

clean::
	-rm -f $(VUSB_FILES)

#----------------------------------------------------------------------

xtal:
	$(AVRDUDE_CMD) -U lfuse:w:0xef:m

rc:
	$(AVRDUDE_CMD) -U lfuse:w:0xe4:m
