################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include EEPROM_Atmega2560/subdir.mk
-include Atmega2560/subdir.mk
-include AES1xxLibrary/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
FLASH_IMAGE += \
AES132_AT90USB1287_TWI.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: AES132_AT90USB1287_TWI.elf secondary-outputs

# Tool invocations
AES132_AT90USB1287_TWI.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -mmcu=at90usb1287 -o"AES132_AT90USB1287_TWI.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

AES132_AT90USB1287_TWI.hex: AES132_AT90USB1287_TWI.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex AES132_AT90USB1287_TWI.elf  "AES132_AT90USB1287_TWI.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: AES132_AT90USB1287_TWI.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=at90usb1287 AES132_AT90USB1287_TWI.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(FLASH_IMAGE)$(ELFS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) AES132_AT90USB1287_TWI.elf
	-@echo ' '

secondary-outputs: $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
