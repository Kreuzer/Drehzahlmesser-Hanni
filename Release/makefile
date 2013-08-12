################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
Drehzahlmesser-Hanni.lss \

FLASH_IMAGE += \
Drehzahlmesser-Hanni.hex \

EEPROM_IMAGE += \
Drehzahlmesser-Hanni.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: Drehzahlmesser-Hanni.elf secondary-outputs

# Tool invocations
Drehzahlmesser-Hanni.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,Drehzahlmesser-Hanni.map -mmcu=atmega88 -o "Drehzahlmesser-Hanni.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Drehzahlmesser-Hanni.lss: Drehzahlmesser-Hanni.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S Drehzahlmesser-Hanni.elf  >"Drehzahlmesser-Hanni.lss"
	@echo 'Finished building: $@'
	@echo ' '

Drehzahlmesser-Hanni.hex: Drehzahlmesser-Hanni.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex Drehzahlmesser-Hanni.elf  "Drehzahlmesser-Hanni.hex"
	@echo 'Finished building: $@'
	@echo ' '

Drehzahlmesser-Hanni.eep: Drehzahlmesser-Hanni.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex Drehzahlmesser-Hanni.elf  "Drehzahlmesser-Hanni.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: Drehzahlmesser-Hanni.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega88 Drehzahlmesser-Hanni.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) Drehzahlmesser-Hanni.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
