################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32g0b1cetx.s 

OBJS += \
./Core/Startup/startup_stm32g0b1cetx.o 

S_DEPS += \
./Core/Startup/startup_stm32g0b1cetx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m0plus -g3 -DDEBUG -c -I../Core/Inc -I../USBPD/App -I../USBPD/Target -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Drivers/STM32G0xx_HAL_Driver/Inc -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Middlewares/Third_Party/FreeRTOS/Source/include -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0 -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Middlewares/ST/STM32_USBPD_Library/Core/inc -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Middlewares/ST/STM32_USBPD_Library/Devices/STM32G0XX/inc -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Drivers/CMSIS/Device/ST/STM32G0xx/Include -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Drivers/CMSIS/Include -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32g0b1cetx.d ./Core/Startup/startup_stm32g0b1cetx.o

.PHONY: clean-Core-2f-Startup

