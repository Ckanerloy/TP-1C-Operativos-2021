################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../so-nivel-gui-library/nivel-gui/nivel-gui.c \
../so-nivel-gui-library/nivel-gui/tad_nivel.c 

OBJS += \
./so-nivel-gui-library/nivel-gui/nivel-gui.o \
./so-nivel-gui-library/nivel-gui/tad_nivel.o 

C_DEPS += \
./so-nivel-gui-library/nivel-gui/nivel-gui.d \
./so-nivel-gui-library/nivel-gui/tad_nivel.d 


# Each subdirectory must supply rules for building sources it contributes
so-nivel-gui-library/nivel-gui/%.o: ../so-nivel-gui-library/nivel-gui/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


