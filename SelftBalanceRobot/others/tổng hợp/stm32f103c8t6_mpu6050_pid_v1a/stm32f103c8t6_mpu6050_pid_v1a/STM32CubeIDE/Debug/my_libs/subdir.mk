################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../my_libs/MPU6050.c \
../my_libs/fonts.c \
../my_libs/motor.c \
../my_libs/motor_config.c \
../my_libs/pid.c 

OBJS += \
./my_libs/MPU6050.o \
./my_libs/fonts.o \
./my_libs/motor.o \
./my_libs/motor_config.o \
./my_libs/pid.o 

C_DEPS += \
./my_libs/MPU6050.d \
./my_libs/fonts.d \
./my_libs/motor.d \
./my_libs/motor_config.d \
./my_libs/pid.d 


# Each subdirectory must supply rules for building sources it contributes
my_libs/%.o my_libs/%.su my_libs/%.cyclo: ../my_libs/%.c my_libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../../Core/Inc -I../../Drivers/STM32F1xx_HAL_Driver/Inc -I../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../Drivers/CMSIS/Include -I"F:/ThanhBangLe/03_STM32_PROJECTS/stm32f103c8t6_mpu6050_pid_v1a/STM32CubeIDE/my_libs" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-my_libs

clean-my_libs:
	-$(RM) ./my_libs/MPU6050.cyclo ./my_libs/MPU6050.d ./my_libs/MPU6050.o ./my_libs/MPU6050.su ./my_libs/fonts.cyclo ./my_libs/fonts.d ./my_libs/fonts.o ./my_libs/fonts.su ./my_libs/motor.cyclo ./my_libs/motor.d ./my_libs/motor.o ./my_libs/motor.su ./my_libs/motor_config.cyclo ./my_libs/motor_config.d ./my_libs/motor_config.o ./my_libs/motor_config.su ./my_libs/pid.cyclo ./my_libs/pid.d ./my_libs/pid.o ./my_libs/pid.su

.PHONY: clean-my_libs

