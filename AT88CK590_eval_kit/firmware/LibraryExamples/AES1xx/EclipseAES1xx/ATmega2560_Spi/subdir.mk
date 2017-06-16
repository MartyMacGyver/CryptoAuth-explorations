################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Projects/CryptoToolset/Libraries/AES1xxLibrary/aes132_comm.c \
C:/Projects/CryptoToolset/Libraries/AES1xxLibrary/aes132_commands.c \
C:/Projects/CryptoToolset/Libraries/AES1xxLibrary/aes132_spi.c \
C:/Projects/CryptoToolset/LibraryExamples/AES1xx/MainModules/aes132_test_main.c \
C:/Projects/CryptoToolset/LibraryExamples/AES1xx/MainModules/aes132_test_vectors.c \
C:/Projects/CryptoToolset/LibraryExamples/Hardware/AVR_AT/spi_phys.c \
C:/Projects/CryptoToolset/Libraries/utilities/timer_utilities.c 

OBJS += \
./aes132_comm.o \
./aes132_commands.o \
./aes132_spi.o \
./aes132_test_main.o \
./aes132_test_vectors.o \
./spi_phys.o \
./timer_utilities.o 

C_DEPS += \
./aes132_comm.d \
./aes132_commands.d \
./aes132_spi.d \
./aes132_test_main.d \
./aes132_test_vectors.d \
./spi_phys.d \
./timer_utilities.d 


# Each subdirectory must supply rules for building sources it contributes
aes132_comm.o: C:/Projects/CryptoToolset/Libraries/AES1xxLibrary/aes132_comm.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"..\..\..\..\Libraries\AES1xxLibrary" -I"..\..\..\..\Libraries\utilities" -I"..\..\..\Hardware\Utilities" -I"..\..\..\Hardware\AVR_AT" -I"\WinAVR-20100110\avr\include" -Wall -g2 -gdwarf-2 -O1 -fpack-struct -fshort-enums -std=c99 -funsigned-char -funsigned-bitfields -mmcu=atmega2560 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

aes132_commands.o: C:/Projects/CryptoToolset/Libraries/AES1xxLibrary/aes132_commands.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"..\..\..\..\Libraries\AES1xxLibrary" -I"..\..\..\..\Libraries\utilities" -I"..\..\..\Hardware\Utilities" -I"..\..\..\Hardware\AVR_AT" -I"\WinAVR-20100110\avr\include" -Wall -g2 -gdwarf-2 -O1 -fpack-struct -fshort-enums -std=c99 -funsigned-char -funsigned-bitfields -mmcu=atmega2560 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

aes132_spi.o: C:/Projects/CryptoToolset/Libraries/AES1xxLibrary/aes132_spi.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"..\..\..\..\Libraries\AES1xxLibrary" -I"..\..\..\..\Libraries\utilities" -I"..\..\..\Hardware\Utilities" -I"..\..\..\Hardware\AVR_AT" -I"\WinAVR-20100110\avr\include" -Wall -g2 -gdwarf-2 -O1 -fpack-struct -fshort-enums -std=c99 -funsigned-char -funsigned-bitfields -mmcu=atmega2560 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

aes132_test_main.o: C:/Projects/CryptoToolset/LibraryExamples/AES1xx/MainModules/aes132_test_main.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"..\..\..\..\Libraries\AES1xxLibrary" -I"..\..\..\..\Libraries\utilities" -I"..\..\..\Hardware\Utilities" -I"..\..\..\Hardware\AVR_AT" -I"\WinAVR-20100110\avr\include" -Wall -g2 -gdwarf-2 -O1 -fpack-struct -fshort-enums -std=c99 -funsigned-char -funsigned-bitfields -mmcu=atmega2560 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

aes132_test_vectors.o: C:/Projects/CryptoToolset/LibraryExamples/AES1xx/MainModules/aes132_test_vectors.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"..\..\..\..\Libraries\AES1xxLibrary" -I"..\..\..\..\Libraries\utilities" -I"..\..\..\Hardware\Utilities" -I"..\..\..\Hardware\AVR_AT" -I"\WinAVR-20100110\avr\include" -Wall -g2 -gdwarf-2 -O1 -fpack-struct -fshort-enums -std=c99 -funsigned-char -funsigned-bitfields -mmcu=atmega2560 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

spi_phys.o: C:/Projects/CryptoToolset/LibraryExamples/Hardware/AVR_AT/spi_phys.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"..\..\..\..\Libraries\AES1xxLibrary" -I"..\..\..\..\Libraries\utilities" -I"..\..\..\Hardware\Utilities" -I"..\..\..\Hardware\AVR_AT" -I"\WinAVR-20100110\avr\include" -Wall -g2 -gdwarf-2 -O1 -fpack-struct -fshort-enums -std=c99 -funsigned-char -funsigned-bitfields -mmcu=atmega2560 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

timer_utilities.o: C:/Projects/CryptoToolset/Libraries/utilities/timer_utilities.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"..\..\..\..\Libraries\AES1xxLibrary" -I"..\..\..\..\Libraries\utilities" -I"..\..\..\Hardware\Utilities" -I"..\..\..\Hardware\AVR_AT" -I"\WinAVR-20100110\avr\include" -Wall -g2 -gdwarf-2 -O1 -fpack-struct -fshort-enums -std=c99 -funsigned-char -funsigned-bitfields -mmcu=atmega2560 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


