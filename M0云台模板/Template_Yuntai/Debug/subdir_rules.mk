################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"F:/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai" -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai/MSPM0" -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai/My_Driver" -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai/My_App" -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai/Debug" -I"F:/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"F:/mspm0_sdk_2_05_01_00/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-749370049: ../empty.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/sysconfig_1.23.1/sysconfig_cli.bat" --script "F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai/empty.syscfg" -o "." -s "F:/mspm0_sdk_2_05_01_00/.metadata/product.json" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-749370049 ../empty.syscfg
device.opt: build-749370049
device.cmd.genlibs: build-749370049
ti_msp_dl_config.c: build-749370049
ti_msp_dl_config.h: build-749370049
Event.dot: build-749370049

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"F:/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai" -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai/MSPM0" -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai/My_Driver" -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai/My_App" -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai/Debug" -I"F:/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"F:/mspm0_sdk_2_05_01_00/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: F:/mspm0_sdk_2_05_01_00/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"F:/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai" -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai/MSPM0" -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai/My_Driver" -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai/My_App" -I"F:/1Microcontroller/workplace/Template_Yuntai_My/Template_Yuntai/Debug" -I"F:/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"F:/mspm0_sdk_2_05_01_00/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


