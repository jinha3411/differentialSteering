################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
MSP_EXP432P401R.obj: ../MSP_EXP432P401R.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/home/jin/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="/home/jin/ti/ccsv6/ccs_base/arm/include" --include_path="/home/jin/ti/ccsv6/ccs_base/arm/include/CMSIS" --include_path="/home/jin/workspace_v6_2/haha" --include_path="/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/msp432_driverlib_3_21_00_05/inc/CMSIS" --include_path="/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/msp432_driverlib_3_21_00_05/inc" --include_path="/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/msp432_driverlib_3_21_00_05/driverlib/MSP432P4xx" --include_path="/home/jin/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --advice:power=all --advice:power_severity=suppress -g --gcc --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs --define=MSP432WARE --display_error_number --diag_warning=225 --diag_warning=255 --diag_wrap=off --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="MSP_EXP432P401R.d" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

empty_min.obj: ../empty_min.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/home/jin/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="/home/jin/ti/ccsv6/ccs_base/arm/include" --include_path="/home/jin/ti/ccsv6/ccs_base/arm/include/CMSIS" --include_path="/home/jin/workspace_v6_2/haha" --include_path="/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/msp432_driverlib_3_21_00_05/inc/CMSIS" --include_path="/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/msp432_driverlib_3_21_00_05/inc" --include_path="/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/msp432_driverlib_3_21_00_05/driverlib/MSP432P4xx" --include_path="/home/jin/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --advice:power=all --advice:power_severity=suppress -g --gcc --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs --define=MSP432WARE --display_error_number --diag_warning=225 --diag_warning=255 --diag_wrap=off --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="empty_min.d" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

build-814524527: ../empty_min.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"/home/jin/ti/xdctools_3_32_01_22_core/xs" --xdcpath="/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/packages;/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/bios_6_46_00_23/packages;/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/tidrivers_msp43x_2_20_00_08/packages;/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/uia_2_00_06_52/packages;/home/jin/ti/ccsv6/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.msp432:MSP432P401R -r release -c "/home/jin/ti/ccsv6/tools/compiler/arm_15.12.3.LTS" --compileOptions "-mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path=\"/home/jin/ti/ccsv6/ccs_base/arm/include\" --include_path=\"/home/jin/ti/ccsv6/ccs_base/arm/include/CMSIS\" --include_path=\"/home/jin/workspace_v6_2/haha\" --include_path=\"/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/msp432_driverlib_3_21_00_05/inc/CMSIS\" --include_path=\"/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/msp432_driverlib_3_21_00_05/inc\" --include_path=\"/home/jin/ti/tirex-content/tirtos_msp43x_2_20_00_06/products/msp432_driverlib_3_21_00_05/driverlib/MSP432P4xx\" --include_path=\"/home/jin/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/include\" --advice:power=all --advice:power_severity=suppress -g --gcc --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs --define=MSP432WARE --display_error_number --diag_warning=225 --diag_warning=255 --diag_wrap=off --gen_func_subsections=on --abi=eabi  " "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: build-814524527
configPkg/compiler.opt: build-814524527
configPkg/: build-814524527


