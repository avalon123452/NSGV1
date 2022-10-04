################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Other/Src/INA219.c \
../Drivers/Other/Src/ds3231_for_stm32_hal.c \
../Drivers/Other/Src/m24c32.c 

OBJS += \
./Drivers/Other/Src/INA219.o \
./Drivers/Other/Src/ds3231_for_stm32_hal.o \
./Drivers/Other/Src/m24c32.o 

C_DEPS += \
./Drivers/Other/Src/INA219.d \
./Drivers/Other/Src/ds3231_for_stm32_hal.d \
./Drivers/Other/Src/m24c32.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Other/Src/%.o Drivers/Other/Src/%.su: ../Drivers/Other/Src/%.c Drivers/Other/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/Other/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Other-2f-Src

clean-Drivers-2f-Other-2f-Src:
	-$(RM) ./Drivers/Other/Src/INA219.d ./Drivers/Other/Src/INA219.o ./Drivers/Other/Src/INA219.su ./Drivers/Other/Src/ds3231_for_stm32_hal.d ./Drivers/Other/Src/ds3231_for_stm32_hal.o ./Drivers/Other/Src/ds3231_for_stm32_hal.su ./Drivers/Other/Src/m24c32.d ./Drivers/Other/Src/m24c32.o ./Drivers/Other/Src/m24c32.su

.PHONY: clean-Drivers-2f-Other-2f-Src

