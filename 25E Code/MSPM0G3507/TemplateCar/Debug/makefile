################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := F:/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS

GEN_OPTS__FLAG := @"./device.opt" 
GEN_CMDS__FLAG := -Wl,-l"./device_linker.cmd" 

ORDERED_OBJS += \
"./empty.o" \
"./ti_msp_dl_config.o" \
"./startup_mspm0g350x_ticlang.o" \
"./Driver/No_Mcu_Ganv_Grayscale_Sensor.o" \
"./Driver/encoder_driver.o" \
"./Driver/motor_driver.o" \
"./Driver/oled_driver.o" \
"./Driver/pid_driver.o" \
"./Driver/wit_imu.o" \
"./MSPM0/clock.o" \
"./MSPM0/interrupt.o" \
"./MSPM0/scheduler.o" \
"./MyApp/ADC.o" \
"./MyApp/gray_app.o" \
"./MyApp/key_app.o" \
"./MyApp/motor_app.o" \
"./MyApp/oled_app.o" \
"./MyApp/pid_app.o" \
"./MyApp/uart_app.o" \
$(GEN_CMDS__FLAG) \
-Wl,-ldevice.cmd.genlibs \
-Wl,-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Driver/subdir_vars.mk
-include MSPM0/subdir_vars.mk
-include MyApp/subdir_vars.mk
-include subdir_rules.mk
-include Driver/subdir_rules.mk
-include MSPM0/subdir_rules.mk
-include MyApp/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
IMU_Test.out 

EXE_OUTPUTS__QUOTED += \
"IMU_Test.out" 


# All Target
all: $(OBJS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "IMU_Test.out"

# Tool invocations
IMU_Test.out: $(OBJS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"F:/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -gdwarf-3 -Wl,-m"IMU_Test.map" -Wl,-i"F:/mspm0_sdk_2_05_01_00/source" -Wl,-i"F:/1Microcontroller/workplace/24h/TemplateCar" -Wl,-i"F:/1Microcontroller/workplace/24h/TemplateCar/Debug/syscfg" -Wl,-i"F:/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/lib" -Wl,--diag_wrap=off -Wl,--display_error_number -Wl,--warn_sections -Wl,--xml_link_info="IMU_Test_linkInfo.xml" -Wl,--rom_model -o "IMU_Test.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "empty.o" "ti_msp_dl_config.o" "startup_mspm0g350x_ticlang.o" "Driver\No_Mcu_Ganv_Grayscale_Sensor.o" "Driver\encoder_driver.o" "Driver\motor_driver.o" "Driver\oled_driver.o" "Driver\pid_driver.o" "Driver\wit_imu.o" "MSPM0\clock.o" "MSPM0\interrupt.o" "MSPM0\scheduler.o" "MyApp\ADC.o" "MyApp\gray_app.o" "MyApp\key_app.o" "MyApp\motor_app.o" "MyApp\oled_app.o" "MyApp\pid_app.o" "MyApp\uart_app.o" 
	-$(RM) "empty.d" "ti_msp_dl_config.d" "startup_mspm0g350x_ticlang.d" "Driver\No_Mcu_Ganv_Grayscale_Sensor.d" "Driver\encoder_driver.d" "Driver\motor_driver.d" "Driver\oled_driver.d" "Driver\pid_driver.d" "Driver\wit_imu.d" "MSPM0\clock.d" "MSPM0\interrupt.d" "MSPM0\scheduler.d" "MyApp\ADC.d" "MyApp\gray_app.d" "MyApp\key_app.d" "MyApp\motor_app.d" "MyApp\oled_app.d" "MyApp\pid_app.d" "MyApp\uart_app.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

