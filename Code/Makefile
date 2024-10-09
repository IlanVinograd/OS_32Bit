# Makefile

PROJECT_ROOT := $(CURDIR)

# Folder structure
INCLUDES := $(PROJECT_ROOT)/Kernel/Includes
OBJECTS := $(PROJECT_ROOT)/Kernel/Objects
SOURCES := $(PROJECT_ROOT)/Kernel/Sources
BINS := $(PROJECT_ROOT)/Kernel/Bins

# Source files
STAGE1_SRC := $(PROJECT_ROOT)/Boot/Stage1/stage1.asm
STAGE2_SRC := $(PROJECT_ROOT)/Boot/Stage2/stage2.asm
LINKER_SCRIPT := $(PROJECT_ROOT)/linker.ld

# Find all .c files in the Sources folder
C_SOURCES := $(wildcard $(SOURCES)/*.c)

# Exclude idt.c from C_SOURCES if you prefer to handle it separately
# Alternatively, let it be included and remove separate handling
# C_SOURCES := $(filter-out $(SOURCES)/idt.c, $(C_SOURCES))

# Generate object files for all .c files
C_OBJECTS := $(patsubst $(SOURCES)/%.c, $(OBJECTS)/%.o, $(C_SOURCES))

# GAS assembly source files
GAS_SOURCES := $(SOURCES)/kernel_entry.s $(SOURCES)/gdt_flush.s

# NASM assembly source files
NASM_SOURCES :=

# Generate object files for assembly files
GAS_OBJECTS := $(patsubst $(SOURCES)/%.s, $(OBJECTS)/%.o, $(GAS_SOURCES))
NASM_OBJECTS := $(patsubst $(SOURCES)/%.asm, $(OBJECTS)/%.o, $(NASM_SOURCES))

# IDT assembly source file
IDT_ASM_SRC := $(SOURCES)/idt_setup.asm

# Object file for IDT assembly
IDT_ASM_OBJ := $(OBJECTS)/idt_setup.o

# Output binaries and objects
STAGE1_BIN := $(PROJECT_ROOT)/Boot/Stage1/stage1.bin
STAGE2_BIN := $(PROJECT_ROOT)/Boot/Stage2/stage2.bin
KERNEL_ELF := $(BINS)/kernel.elf
KERNEL_BIN := $(BINS)/kernel.bin
IMG := $(PROJECT_ROOT)/bootloader.img

# Compiler and Assembler Flags
CFLAGS := -I$(INCLUDES) -ffreestanding -m32 -O2 -Wall -Wextra
QEMU_IMG := qemu-img
DD := dd
NASM := nasm

# Always clean before building
output: clean $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_BIN) $(IMG)

# Ensure that necessary directories exist
$(OBJECTS):
	@mkdir -p $(OBJECTS)

$(BINS):
	@mkdir -p $(BINS)

# Assemble stage1.bin
$(STAGE1_BIN): $(STAGE1_SRC)
	@echo "Assembling Stage 1..."
	$(NASM) -f bin "$(STAGE1_SRC)" -o "$(STAGE1_BIN)"

# Assemble stage2.bin
$(STAGE2_BIN): $(STAGE2_SRC)
	@echo "Assembling Stage 2..."
	$(NASM) -f bin "$(STAGE2_SRC)" -o "$(STAGE2_BIN)"

# Assemble IDT ASM code
$(IDT_ASM_OBJ): $(IDT_ASM_SRC) | $(OBJECTS)
	@echo "Assembling IDT ASM code..."
	$(NASM) -f elf32 $(IDT_ASM_SRC) -o $(IDT_ASM_OBJ)

# Compile and link the kernel
$(KERNEL_BIN): $(OBJECTS) $(BINS) $(C_OBJECTS) $(GAS_OBJECTS) $(IDT_ASM_OBJ) $(LINKER_SCRIPT)
	@echo "Compiling and linking Kernel..."
	i686-elf-ld -o $(KERNEL_ELF) -T $(LINKER_SCRIPT) $(C_OBJECTS) $(GAS_OBJECTS) $(IDT_ASM_OBJ)
	i686-elf-objcopy -O binary $(KERNEL_ELF) $(KERNEL_BIN)

# Rule to compile C source files into object files
$(C_OBJECTS): $(OBJECTS)/%.o : $(SOURCES)/%.c | $(OBJECTS)
	@echo "Compiling $<..."
	i686-elf-gcc $(CFLAGS) -c $< -o $@

# Rule to assemble GAS assembly files (.s) into object files
$(GAS_OBJECTS): $(OBJECTS)/%.o : $(SOURCES)/%.s | $(OBJECTS)
	@echo "Assembling (GAS) $<..."
	i686-elf-as --32 $< -o $@

# Rule to assemble NASM assembly files (.asm) into object files
$(NASM_OBJECTS): $(OBJECTS)/%.o : $(SOURCES)/%.asm | $(OBJECTS)
	@echo "Assembling (NASM) $<..."
	$(NASM) -f elf32 $< -o $@

# Create bootloader.img
$(IMG): $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_BIN)
	@echo "Creating bootloader.img..."
	$(QEMU_IMG) create -f raw "$(IMG)" 10M
	$(DD) if=$(STAGE1_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	$(DD) if=$(STAGE2_BIN) of=$(IMG) bs=512 seek=1 conv=notrunc
	$(DD) if=$(KERNEL_BIN) of=$(IMG) bs=512 seek=2 conv=notrunc

# Clean up all the generated files
clean:
	@echo "Cleaning up..."
	rm -f "$(STAGE1_BIN)" "$(STAGE2_BIN)" "$(KERNEL_BIN)" "$(KERNEL_ELF)" "$(IMG)" \
	      $(C_OBJECTS) $(GAS_OBJECTS) $(NASM_OBJECTS) $(IDT_ASM_OBJ)
	rm -rf $(OBJECTS)
	rm -rf $(BINS)

# Phony targets
.PHONY: clean output