################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Oled_display_driver/ssd1306.c \
../Drivers/Oled_display_driver/ssd1306_fonts.c 

OBJS += \
./Drivers/Oled_display_driver/ssd1306.o \
./Drivers/Oled_display_driver/ssd1306_fonts.o 

C_DEPS += \
./Drivers/Oled_display_driver/ssd1306.d \
./Drivers/Oled_display_driver/ssd1306_fonts.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Oled_display_driver/%.o Drivers/Oled_display_driver/%.su Drivers/Oled_display_driver/%.cyclo: ../Drivers/Oled_display_driver/%.c Drivers/Oled_display_driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/Thesis/Cyphal_CAN_implementation_with_IRQ_final2_stringsubscribe2WithService/Drivers/Oled_display_driver" -I"D:/Thesis/Cyphal_CAN_implementation_with_IRQ_final2_stringsubscribe2WithService/Drivers/Cyphalincludes" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Oled_display_driver

clean-Drivers-2f-Oled_display_driver:
	-$(RM) ./Drivers/Oled_display_driver/ssd1306.cyclo ./Drivers/Oled_display_driver/ssd1306.d ./Drivers/Oled_display_driver/ssd1306.o ./Drivers/Oled_display_driver/ssd1306.su ./Drivers/Oled_display_driver/ssd1306_fonts.cyclo ./Drivers/Oled_display_driver/ssd1306_fonts.d ./Drivers/Oled_display_driver/ssd1306_fonts.o ./Drivers/Oled_display_driver/ssd1306_fonts.su

.PHONY: clean-Drivers-2f-Oled_display_driver

