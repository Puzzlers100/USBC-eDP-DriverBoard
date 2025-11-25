################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../USBPD/Target/usbpd_dpm_user.c \
../USBPD/Target/usbpd_pwr_user.c \
../USBPD/Target/usbpd_vdm_user.c 

OBJS += \
./USBPD/Target/usbpd_dpm_user.o \
./USBPD/Target/usbpd_pwr_user.o \
./USBPD/Target/usbpd_vdm_user.o 

C_DEPS += \
./USBPD/Target/usbpd_dpm_user.d \
./USBPD/Target/usbpd_pwr_user.d \
./USBPD/Target/usbpd_vdm_user.d 


# Each subdirectory must supply rules for building sources it contributes
USBPD/Target/%.o USBPD/Target/%.su USBPD/Target/%.cyclo: ../USBPD/Target/%.c USBPD/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G0B1xx -DUSE_FULL_LL_DRIVER '-DCMSIS_device_header=<stm32g0xx.h>' -DUSBPD_PORT_COUNT=1 -D_RTOS -D_SNK -DUSBPDCORE_LIB_PD3_FULL -c -I../Core/Inc -I../USBPD/App -I../USBPD/Target -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Drivers/STM32G0xx_HAL_Driver/Inc -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Middlewares/Third_Party/FreeRTOS/Source/include -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0 -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Middlewares/ST/STM32_USBPD_Library/Core/inc -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Middlewares/ST/STM32_USBPD_Library/Devices/STM32G0XX/inc -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Drivers/CMSIS/Device/ST/STM32G0xx/Include -I/home/jakub/STM32Cube/Repository/STM32Cube_FW_G0_V1.6.2/Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-USBPD-2f-Target

clean-USBPD-2f-Target:
	-$(RM) ./USBPD/Target/usbpd_dpm_user.cyclo ./USBPD/Target/usbpd_dpm_user.d ./USBPD/Target/usbpd_dpm_user.o ./USBPD/Target/usbpd_dpm_user.su ./USBPD/Target/usbpd_pwr_user.cyclo ./USBPD/Target/usbpd_pwr_user.d ./USBPD/Target/usbpd_pwr_user.o ./USBPD/Target/usbpd_pwr_user.su ./USBPD/Target/usbpd_vdm_user.cyclo ./USBPD/Target/usbpd_vdm_user.d ./USBPD/Target/usbpd_vdm_user.o ./USBPD/Target/usbpd_vdm_user.su

.PHONY: clean-USBPD-2f-Target

