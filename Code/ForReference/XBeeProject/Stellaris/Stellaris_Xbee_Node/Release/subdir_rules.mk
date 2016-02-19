################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
I2C_Stellaris_API.obj: ../I2C_Stellaris_API.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="C:/StellarisWare" --include_path="C:/StellarisWare/boards/ek-lm4f120xl" --define=PART_LM4F120H5QR --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="I2C_Stellaris_API.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

jsmn.obj: ../jsmn.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="C:/StellarisWare" --include_path="C:/StellarisWare/boards/ek-lm4f120xl" --define=PART_LM4F120H5QR --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="jsmn.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="C:/StellarisWare" --include_path="C:/StellarisWare/boards/ek-lm4f120xl" --define=PART_LM4F120H5QR --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


