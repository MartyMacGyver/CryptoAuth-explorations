################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Projects/CryptoToolset/LibraryExamples/Hardware/AVR_AT/i2c_phys.c \
C:/Projects/CryptoToolset/Libraries/utilities/timer_utilities.c 

OBJS += \
./i2c_phys.o \
./timer_utilities.o 

C_DEPS += \
./i2c_phys.d \
./timer_utilities.d 


# Each subdirectory must supply rules for building sources it contributes
i2c_phys.o: C:/Projects/CryptoToolset/LibraryExamples/Hardware/AVR_AT/i2c_phys.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"..\..\..\..\Libraries\AES1xxLibrary" -I"..\..\..\..\Libraries\utilities" -I"..\..\..\Hardware\Utilities" -I"..\..\..\Hardware\AVR_AT" -I"\WinAVR-20100110\avr\include" -Wall -g2 -gdwarf-2 -O1 -fpack-struct -fshort-enums -std=c99 -funsigned-char -funsigned-bitfields -mmcu=at90usb1287 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

timer_utilities.o: C:/Projects/CryptoToolset/Libraries/utilities/timer_utilities.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"..\..\..\..\Libraries\AES1xxLibrary" -I"..\..\..\..\Libraries\utilities" -I"..\..\..\Hardware\Utilities" -I"..\..\..\Hardware\AVR_AT" -I"\WinAVR-20100110\avr\include" -Wall -g2 -gdwarf-2 -O1 -fpack-struct -fshort-enums -std=c99 -funsigned-char -funsigned-bitfields -mmcu=at90usb1287 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


