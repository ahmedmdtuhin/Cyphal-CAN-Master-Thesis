################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Cyphalincludes/libcanard/canard.c 

OBJS += \
./Drivers/Cyphalincludes/libcanard/canard.o 

C_DEPS += \
./Drivers/Cyphalincludes/libcanard/canard.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Cyphalincludes/libcanard/%.o Drivers/Cyphalincludes/libcanard/%.su Drivers/Cyphalincludes/libcanard/%.cyclo: ../Drivers/Cyphalincludes/libcanard/%.c Drivers/Cyphalincludes/libcanard/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/Thesis/Cyphal_CAN_implementation_with_IRQ_final/Drivers/Oled_display_driver" -I"D:/Thesis/Cyphal_CAN_implementation_with_IRQ_final/Drivers/Cyphalincludes" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Cyphalincludes-2f-libcanard

clean-Drivers-2f-Cyphalincludes-2f-libcanard:
	-$(RM) ./Drivers/Cyphalincludes/libcanard/canard.cyclo ./Drivers/Cyphalincludes/libcanard/canard.d ./Drivers/Cyphalincludes/libcanard/canard.o ./Drivers/Cyphalincludes/libcanard/canard.su

.PHONY: clean-Drivers-2f-Cyphalincludes-2f-libcanard

