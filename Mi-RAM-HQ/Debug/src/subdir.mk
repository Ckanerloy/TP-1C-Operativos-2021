################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Mi-RAM.c \
../src/dump_memoria.c \
../src/mapa.c \
../src/paginacion.c \
../src/segmentacion.c \
../src/swap.c 

OBJS += \
./src/Mi-RAM.o \
./src/dump_memoria.o \
./src/mapa.o \
./src/paginacion.o \
./src/segmentacion.o \
./src/swap.o 

C_DEPS += \
./src/Mi-RAM.d \
./src/dump_memoria.d \
./src/mapa.d \
./src/paginacion.d \
./src/segmentacion.d \
./src/swap.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/LibMap" -I"/home/utnso/commons" -I"/home/utnso/tp-2021-1c-UTNIX/utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


