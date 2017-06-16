###############################################################################
# Makefile for the project SPI
###############################################################################

## General Flags
PROJECT = SPI
MCU = at90usb1287
TARGET = AES1xxSpi.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99             -DF_CPU=16000000UL -O1 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += 


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"C:\Projects\CryptoToolset\LibraryExamples\AES1xx\AVRStudioSpi\..\..\Hardware\AVR_AT" -I"C:\Projects\CryptoToolset\LibraryExamples\AES1xx\AVRStudioSpi\..\..\..\Libraries\AES1xxLibrary" -I"C:\Projects\CryptoToolset\LibraryExamples\AES1xx\AVRStudioSpi\..\..\..\Libraries\includes" -I"C:\Projects\CryptoToolset\LibraryExamples\AES1xx\AVRStudioSpi\..\..\..\..\..\WinAVR-20100110\avr\include" 

## Objects that must be built in order to link
OBJECTS = aes132_comm.o aes132_commands.o aes132_spi.o aes132_utilities.o aes132_test_vectors.o aes132_test_main.o spi_phys.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) AES1xxSpi.hex AES1xxSpi.eep size

## Compile
aes132_comm.o: ../../../../Libraries/AES1xxLibrary/aes132_comm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

aes132_commands.o: ../../../../Libraries/AES1xxLibrary/aes132_commands.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

aes132_spi.o: ../../../../Libraries/AES1xxLibrary/aes132_spi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

aes132_utilities.o: ../../../../Libraries/AES1xxLibrary/aes132_utilities.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

aes132_test_vectors.o: ../../MainModules/aes132_test_vectors.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

aes132_test_main.o: ../../MainModules/aes132_test_main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

spi_phys.o: ../../../Hardware/AVR_AT/spi_phys.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) AES1xxSpi.elf dep/* AES1xxSpi.hex AES1xxSpi.eep

## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

