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

# Generate object files for all .c files
C_OBJECTS := $(patsubst $(SOURCES)/%.c, $(OBJECTS)/%.o, $(C_SOURCES))

# Add kernel_entry.s separately, since this is an assembler file
ASM_SOURCES := $(SOURCES)/kernel_entry.s $(SOURCES)/gdt_flush.s
ASM_OBJECTS := $(patsubst $(SOURCES)/%.s, $(OBJECTS)/%.o, $(ASM_SOURCES))

# IDT source files
IDT_C_SRC := $(SOURCES)/idt.o
IDT_ASM_SRC := $(SOURCES)/idt_setup.asm

# Object files for IDT
IDT_C_OBJ := $(OBJECTS)/idt_handler.o
IDT_ASM_OBJ := $(OBJECTS)/idt_setup.o

# Output binaries and objects
STAGE1_BIN := $(PROJECT_ROOT)/Boot/Stage1/stage1.bin
STAGE2_BIN := $(PROJECT_ROOT)/Boot/Stage2/stage2.bin
KERNEL_ELF := $(BINS)/kernel.elf
KERNEL_BIN := $(BINS)/kernel.bin
IMG := $(PROJECT_ROOT)/bootloader.img

# Compiler and Assembler Flags
CFLAGS := -I$(INCLUDES) -ffreestanding
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

# Compile IDT C code
$(IDT_C_OBJ): $(IDT_C_SRC)
	@echo "Compiling IDT C code..."
	i686-elf-gcc $(CFLAGS) -c $< -o $@

# Assemble IDT ASM code
$(IDT_ASM_OBJ): $(IDT_ASM_SRC)
	@echo "Assembling IDT ASM code..."
	$(NASM) -f elf32 $(IDT_ASM_SRC) -o $(IDT_ASM_OBJ)

# Compile and link the kernel, include IDT object files
$(KERNEL_BIN): $(OBJECTS) $(BINS) $(C_OBJECTS) $(ASM_OBJECTS) $(IDT_ASM_OBJ) $(LINKER_SCRIPT)
	@echo "Compiling and linking Kernel with IDT..."
	i686-elf-ld -o $(KERNEL_ELF) -T $(LINKER_SCRIPT) $(C_OBJECTS) $(ASM_OBJECTS) $(IDT_ASM_OBJ)
	i686-elf-objcopy -O binary $(KERNEL_ELF) $(KERNEL_BIN)

# Rule to compile C source files into object files
$(OBJECTS)/%.o: $(SOURCES)/%.c
	@echo "Compiling $<..."
	i686-elf-gcc $(CFLAGS) -c $< -o $@

# Rule to assemble assembly files into object files
$(OBJECTS)/%.o: $(SOURCES)/%.s
	@echo "Assembling $<..."
	i686-elf-gcc $(CFLAGS) -c $< -o $@

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
	      $(C_OBJECTS) $(ASM_OBJECTS) $(IDT_C_OBJ) $(IDT_ASM_OBJ)

# Phony targets
.PHONY: clean output