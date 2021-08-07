################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../so-nivel-gui-library/nivel-gui-test/main.c 

OBJS += \
./so-nivel-gui-library/nivel-gui-test/main.o 

C_DEPS += \
./so-nivel-gui-library/nivel-gui-test/main.d 


# Each subdirectory must supply rules for building sources it contributes
so-nivel-gui-library/nivel-gui-test/%.o: ../so-nivel-gui-library/nivel-gui-test/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


