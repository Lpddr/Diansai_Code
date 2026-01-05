################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Encoder/%.o: ../Encoder/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"F:/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"F:/1Microcontroller/workplace/IMU_Test/IMU_Test" -I"F:/1Microcontroller/workplace/IMU_Test/IMU_Test/Encoder" -I"F:/1Microcontroller/workplace/IMU_Test/IMU_Test/Motor" -I"F:/1Microcontroller/workplace/IMU_Test/IMU_Test/MSPM0" -I"F:/1Microcontroller/workplace/IMU_Test/IMU_Test/Oled_HardWare" -I"F:/1Microcontroller/workplace/IMU_Test/IMU_Test/Wit_IMU" -I"F:/1Microcontroller/workplace/IMU_Test/IMU_Test/Debug" -I"F:/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"F:/mspm0_sdk_2_05_01_00/source" -gdwarf-3 -MMD -MP -MF"Encoder/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


