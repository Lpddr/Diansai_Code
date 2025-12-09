################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"F:/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"F:/1Microcontroller/workplace/24h/TemplateCar" -I"F:/1Microcontroller/workplace/24h/TemplateCar/Driver" -I"F:/1Microcontroller/workplace/24h/TemplateCar/MyApp" -I"F:/1Microcontroller/workplace/24h/TemplateCar/MSPM0" -I"F:/1Microcontroller/workplace/24h/TemplateCar/Debug" -I"F:/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"F:/mspm0_sdk_2_05_01_00/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1485784559: ../empty.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/sysconfig_1.23.1/sysconfig_cli.bat" --script "F:/1Microcontroller/workplace/24h/TemplateCar/empty.syscfg" -o "." -s "F:/mspm0_sdk_2_05_01_00/.metadata/product.json" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-1485784559 ../empty.syscfg
device.opt: build-1485784559
device.cmd.genlibs: build-1485784559
ti_msp_dl_config.c: build-1485784559
ti_msp_dl_config.h: build-1485784559
Event.dot: build-1485784559

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"F:/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"F:/1Microcontroller/workplace/24h/TemplateCar" -I"F:/1Microcontroller/workplace/24h/TemplateCar/Driver" -I"F:/1Microcontroller/workplace/24h/TemplateCar/MyApp" -I"F:/1Microcontroller/workplace/24h/TemplateCar/MSPM0" -I"F:/1Microcontroller/workplace/24h/TemplateCar/Debug" -I"F:/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"F:/mspm0_sdk_2_05_01_00/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: F:/mspm0_sdk_2_05_01_00/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"F:/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"F:/1Microcontroller/workplace/24h/TemplateCar" -I"F:/1Microcontroller/workplace/24h/TemplateCar/Driver" -I"F:/1Microcontroller/workplace/24h/TemplateCar/MyApp" -I"F:/1Microcontroller/workplace/24h/TemplateCar/MSPM0" -I"F:/1Microcontroller/workplace/24h/TemplateCar/Debug" -I"F:/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"F:/mspm0_sdk_2_05_01_00/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


