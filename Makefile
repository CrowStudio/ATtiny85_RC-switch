PRG            = RC-switch
OBJ            = $(PRG).o 
MCU_TARGET     = attiny85
OPTIMIZE       = -O2

DEFS           =
LIBS           = 
CC             = avr-gcc

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PRG).elf lst hex

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	avr-size -C --mcu=$(MCU_TARGET) $(PRG).elf
main.o: main.c 

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

text: hex bin srec

hex:  $(PRG).hex

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@


program:
	avrdude -p $(MCU_TARGET) -c stk500v1 -b 19200 -P /dev/ttyACM0 -U flash:w:$(PRG).hex
