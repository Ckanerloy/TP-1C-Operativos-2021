################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Mongo-Store.c \
../src/bitArray.c \
../src/iniciarFileSystem.c \
../src/sabotajes.c 

OBJS += \
./src/Mongo-Store.o \
./src/bitArray.o \
./src/iniciarFileSystem.o \
./src/sabotajes.o 

C_DEPS += \
./src/Mongo-Store.d \
./src/bitArray.d \
./src/iniciarFileSystem.d \
./src/sabotajes.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2021-1c-UTNIX/utils" -I"/home/utnso/commons" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


