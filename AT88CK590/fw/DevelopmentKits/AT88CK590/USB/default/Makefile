###############################################################################
# Makefile for the project AVR_Javan_UART
###############################################################################

## General Flags
PROJECT = AVR_Javan_UART
MCU = at90usb1287
TARGET = AT88ubase_Javan_USB.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99                                                        -D_DEBUG    -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=AT88ubase_Javan_USB.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = scheduler.o wdt_drv.o start_boot.o usb_task.o usb_device_task.o usb_standard_request.o usb_drv.o usb_descriptors.o usb_specific_request.o utility.o parser.o Auth_Comm.o CA_API.o CA_Func.o CRCFunction.o HA_API.o HA_Func.o sha256.o AVR_Javan_USB.o Auth_Phys_AVR_BitBang.o log.o usb.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) AT88ubase_Javan_USB.hex AT88ubase_Javan_USB.eep AT88ubase_Javan_USB.lss size

## Compile
scheduler.o: ../common/modules/scheduler/scheduler.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

wdt_drv.o: ../common/lib_mcu/wdt/wdt_drv.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

start_boot.o: ../lib_mcu/util/start_boot.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb_task.o: ../modules/usb/usb_task.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb_device_task.o: ../modules/usb/device_chap9/usb_device_task.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb_standard_request.o: ../modules/usb/device_chap9/usb_standard_request.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb_drv.o: ../lib_mcu/usb/usb_drv.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb_descriptors.o: ../usb/usb_descriptors.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb_specific_request.o: ../usb/usb_specific_request.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

utility.o: ../usb/utility.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

parser.o: ../usb/parser.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Auth_Comm.o: ../SA1XXX_LIB/Auth_Comm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

CA_API.o: ../SA1XXX_LIB/CA_API.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

CA_Func.o: ../SA1XXX_LIB/CA_Func.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

CRCFunction.o: ../SA1XXX_LIB/CRCFunction.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

HA_API.o: ../SA1XXX_LIB/HA_API.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

HA_Func.o: ../SA1XXX_LIB/HA_Func.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sha256.o: ../SA1XXX_LIB/sha256.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

AVR_Javan_USB.o: ../AVR_Javan_USB.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Auth_Phys_AVR_BitBang.o: ../SA1XXX_LIB/Auth_Phys_AVR_BitBang.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

log.o: ../log.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb.o: ../usb/usb.c
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
	-rm -rf $(OBJECTS) AT88ubase_Javan_USB.elf dep/* AT88ubase_Javan_USB.hex AT88ubase_Javan_USB.eep AT88ubase_Javan_USB.lss AT88ubase_Javan_USB.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

