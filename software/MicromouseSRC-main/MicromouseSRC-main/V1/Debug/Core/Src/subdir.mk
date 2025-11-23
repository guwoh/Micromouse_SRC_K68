################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Algorithm.c \
../Core/Src/Calculation.c \
../Core/Src/Encoder.c \
../Core/Src/Mode.c \
../Core/Src/Motor.c \
../Core/Src/PID.c \
../Core/Src/Sensor.c \
../Core/Src/delay.c \
../Core/Src/flash_store_data.c \
../Core/Src/function_c.c \
../Core/Src/main.c \
../Core/Src/mpu6050.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c \
../Core/Src/wall.c 

OBJS += \
./Core/Src/Algorithm.o \
./Core/Src/Calculation.o \
./Core/Src/Encoder.o \
./Core/Src/Mode.o \
./Core/Src/Motor.o \
./Core/Src/PID.o \
./Core/Src/Sensor.o \
./Core/Src/delay.o \
./Core/Src/flash_store_data.o \
./Core/Src/function_c.o \
./Core/Src/main.o \
./Core/Src/mpu6050.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o \
./Core/Src/wall.o 

C_DEPS += \
./Core/Src/Algorithm.d \
./Core/Src/Calculation.d \
./Core/Src/Encoder.d \
./Core/Src/Mode.d \
./Core/Src/Motor.d \
./Core/Src/PID.d \
./Core/Src/Sensor.d \
./Core/Src/delay.d \
./Core/Src/flash_store_data.d \
./Core/Src/function_c.d \
./Core/Src/main.d \
./Core/Src/mpu6050.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d \
./Core/Src/wall.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Algorithm.cyclo ./Core/Src/Algorithm.d ./Core/Src/Algorithm.o ./Core/Src/Algorithm.su ./Core/Src/Calculation.cyclo ./Core/Src/Calculation.d ./Core/Src/Calculation.o ./Core/Src/Calculation.su ./Core/Src/Encoder.cyclo ./Core/Src/Encoder.d ./Core/Src/Encoder.o ./Core/Src/Encoder.su ./Core/Src/Mode.cyclo ./Core/Src/Mode.d ./Core/Src/Mode.o ./Core/Src/Mode.su ./Core/Src/Motor.cyclo ./Core/Src/Motor.d ./Core/Src/Motor.o ./Core/Src/Motor.su ./Core/Src/PID.cyclo ./Core/Src/PID.d ./Core/Src/PID.o ./Core/Src/PID.su ./Core/Src/Sensor.cyclo ./Core/Src/Sensor.d ./Core/Src/Sensor.o ./Core/Src/Sensor.su ./Core/Src/delay.cyclo ./Core/Src/delay.d ./Core/Src/delay.o ./Core/Src/delay.su ./Core/Src/flash_store_data.cyclo ./Core/Src/flash_store_data.d ./Core/Src/flash_store_data.o ./Core/Src/flash_store_data.su ./Core/Src/function_c.cyclo ./Core/Src/function_c.d ./Core/Src/function_c.o ./Core/Src/function_c.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mpu6050.cyclo ./Core/Src/mpu6050.d ./Core/Src/mpu6050.o ./Core/Src/mpu6050.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su ./Core/Src/wall.cyclo ./Core/Src/wall.d ./Core/Src/wall.o ./Core/Src/wall.su

.PHONY: clean-Core-2f-Src

