################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Mi-RAM

dependents:
	-cd /home/utnso/tp-2021-1c-UTNIX/utils/Debug && $(MAKE) all
	-cd /home/utnso/workspace/LibMap/Debug && $(MAKE) all
	-cd /home/utnso/commons/Debug && $(MAKE) all

# Tool invocations
Mi-RAM: $(OBJS) $(USER_OBJS) /home/utnso/tp-2021-1c-UTNIX/utils/Debug/libutils.so /home/utnso/workspace/LibMap/Debug/libLibMap.so /home/utnso/commons/Debug/libcommons.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/workspace/LibMap/Debug" -L"/home/utnso/commons/Debug" -L"/home/utnso/tp-2021-1c-UTNIX/utils/Debug" -o "Mi-RAM" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Mi-RAM
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/tp-2021-1c-UTNIX/utils/Debug/libutils.so:
/home/utnso/workspace/LibMap/Debug/libLibMap.so:
/home/utnso/commons/Debug/libcommons.so:

-include ../makefile.targets
