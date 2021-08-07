################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Discordiador.c \
../src/bitacora.c \
../src/planificador.c \
../src/utilsDiscordiador.c 

OBJS += \
./src/Discordiador.o \
./src/bitacora.o \
./src/planificador.o \
./src/utilsDiscordiador.o 

C_DEPS += \
./src/Discordiador.d \
./src/bitacora.d \
./src/planificador.d \
./src/utilsDiscordiador.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2021-1c-UTNIX/utils" -I"/home/utnso/commons" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


