################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Projects/CryptoToolset/Libraries/AES1xxLibrary/AES1_Comm.c \
C:/Projects/CryptoToolset/Libraries/AES1xxLibrary/AES1_Spi.c \
C:/Projects/CryptoToolset/LibraryExamples/AES1xx/MainModules/AES1_TestMain.c \
C:/Projects/CryptoToolset/LibraryExamples/Hardware/AVR_AT/SpiPhys.c 

OBJS += \
./AES1_Comm.o \
./AES1_Spi.o \
./AES1_TestMain.o \
./SpiPhys.o 

C_DEPS += \
./AES1_Comm.d \
./AES1_Spi.d \
./AES1_TestMain.d \
./SpiPhys.d 


# Each subdirectory must supply rules for building sources it contributes
AES1_Comm.o: C:/Projects/CryptoToolset/Libraries/AES1xxLibrary/AES1_Comm.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"..\..\..\..\Libraries\AES1xxLibrary" -I"..\..\..\Hardware\Utilities" -I"..\..\..\..\Libraries\includes" -I"..\..\..\Hardware\AVR_AT" -I"\WinAVR-20100110\avr\include" -Wall -g2 -gdwarf-2 -O1 -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega2560 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

AES1_Spi.o: C:/Projects/CryptoToolset/Libraries/AES1xxLibrary/AES1_Spi.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"..\..\..\..\Libraries\AES1xxLibrary" -I"..\..\..\Hardware\Utilities" -I"..\..\..\..\Libraries\includes" -I"..\..\..\Hardware\AVR_AT" -I"\WinAVR-20100110\avr\include" -Wall -g2 -gdwarf-2 -O1 -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega2560 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

AES1_TestMain.o: C:/Projects/CryptoToolset/LibraryExamples/AES1xx/MainModules/AES1_TestMain.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"..\..\..\..\Libraries\AES1xxLibrary" -I"..\..\..\Hardware\Utilities" -I"..\..\..\..\Libraries\includes" -I"..\..\..\Hardware\AVR_AT" -I"\WinAVR-20100110\avr\include" -Wall -g2 -gdwarf-2 -O1 -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega2560 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

SpiPhys.o: C:/Projects/CryptoToolset/LibraryExamples/Hardware/AVR_AT/SpiPhys.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"..\..\..\..\Libraries\AES1xxLibrary" -I"..\..\..\Hardware\Utilities" -I"..\..\..\..\Libraries\includes" -I"..\..\..\Hardware\AVR_AT" -I"\WinAVR-20100110\avr\include" -Wall -g2 -gdwarf-2 -O1 -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega2560 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


