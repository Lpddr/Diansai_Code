################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
MSPM0/%.o: ../MSPM0/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"F:/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"F:/1Microcontroller/workplace/Last_Templater" -I"F:/1Microcontroller/workplace/Last_Templater/Driver" -I"F:/1Microcontroller/workplace/Last_Templater/MyApp" -I"F:/1Microcontroller/workplace/Last_Templater/MSPM0" -I"F:/1Microcontroller/workplace/Last_Templater/Debug" -I"F:/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"F:/mspm0_sdk_2_05_01_00/source" -gdwarf-3 -MMD -MP -MF"MSPM0/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


