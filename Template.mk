.SUFFIXES: .S .c .o .elf .hex .dump

#----------------------------------------------------------------------

all::

clean::
	-rm -f *.o *.elf *.hex *.dump

$(TARGET).hex: $(TARGET).elf

$(TARGET).dump: $(TARGET).elf

.c.o: # for C source
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

.c.S: # for debug purpose
	$(CC) $(CFLAGS) $(CPPFLAGS) -S -o $@ $<

.S.o: # for assembly source
	$(CC) $(ASFLAGS) $(CPPFLAGS) -c -o $@ $<

.elf.hex:
	$(OBJCOPY) -O ihex $< $@

.elf.dump:
	$(OBJDUMP) -D $< > $@

size:
	$(OBJDUMP) -h *.elf | grep text

#----------------------------------------------------------------------

CPPFLAGS = -DF_CPU=$(CPU_FREQ)

CC = avr-gcc
CFLAGS_ARCH = -mmcu=$(CPU_TYPE)
CFLAGS_WARN = -std=gnu99 -W -Wall -Wstrict-prototypes
CFLAGS_CODEGEN = -g -gdwarf-2 -Os \
	-funsigned-char \
	-funsigned-bitfields \
	-fpack-struct \
	-fshort-enums \
	-fno-split-wide-types \
	-finline-limit=3 \
	-fno-inline-small-functions \
	-ffunction-sections \
	-fdata-sections \
	-fno-tree-scev-cprop \
	-fno-schedule-insns2 \
	-ffreestanding

CFLAGS = $(CFLAGS_ARCH) $(CFLAGS_WARN) $(CFLAGS_CODEGEN) $(CFLAGS_EXTRA)

AS = avr-as
ASFLAGS = -mmcu=$(CPU_TYPE) -g

LD	= avr-ld
LDFLAGS = -mmcu=$(CPU_TYPE) -Wl,--gc-sections -Wl,-relax -g

OBJDUMP	= avr-objdump
OBJCOPY	= avr-objcopy

# usbdrv/Readme.txt
#  -fno-move-loop-invariants, -fno-tree-scev-cprop and
#  -fno-inline-small-functions -Os
#
# https://gcc.gnu.org/onlinedocs/gcc-4.8.2/gcc/Optimize-Options.html
#
# http://www.tty1.net/blog/2008/avr-gcc-optimisations_en.html
# Short types
#	-funsigned-char
#	-funsigned-bitfields
#	-fpack-struct
#	-fshort-enums
# Wide types
#	-fno-split-wide-types
# Conservative inlining
#	--param inline-call-cost=2 (unavail)
#	-finline-limit=3
#	-fno-inline-small-functions
# Don't include unused function and data
#	--gc-sections (at linking)
#	-ffunction-sections
#	-fdata-sections
# Global optimization
#	-combine -fwhole-program (unavail)
# Loop optimization
#	-fno-tree-scev-cprop
#	-fno-schedule-insns2
# Other codes and data size
#	-ffreestanding


#----------------------------------------------------------------------

AVRDUDE_PROG = avrispmkII
AVRDUDE_CMD = sudo avrdude -p $(AVRDUDE_PART) -c $(AVRDUDE_PROG) -P usb $(AVRDUDE_FLAGS)

flash:
	$(AVRDUDE_CMD) -U 'flash:w:$(TARGET).hex:i'

reset:
	$(AVRDUDE_CMD)

write_lfuse:
	$(AVRDUDE_CMD) -U 'lfuse:w:$(LFUSE):m'
