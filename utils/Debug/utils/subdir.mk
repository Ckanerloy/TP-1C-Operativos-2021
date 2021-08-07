################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../utils/deserializacion.c \
../utils/loader.c \
../utils/serializacion.c \
../utils/sockets.c \
../utils/tareas.c 

OBJS += \
./utils/deserializacion.o \
./utils/loader.o \
./utils/serializacion.o \
./utils/sockets.o \
./utils/tareas.o 

C_DEPS += \
./utils/deserializacion.d \
./utils/loader.d \
./utils/serializacion.d \
./utils/sockets.d \
./utils/tareas.d 


# Each subdirectory must supply rules for building sources it contributes
utils/%.o: ../utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


