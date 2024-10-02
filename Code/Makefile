PROJECT_ROOT := $(CURDIR)

# Folder structure
INCLUDES := $(PROJECT_ROOT)/Kernel/Includes
OBJECTS := $(PROJECT_ROOT)/Kernel/Objects
SOURCES := $(PROJECT_ROOT)/Kernel/Sources
BINS := $(PROJECT_ROOT)/Kernel/Bins

# Source files
STAGE1_SRC := $(PROJECT_ROOT)/Boot/Stage1/stage1.asm
STAGE2_SRC := $(PROJECT_ROOT)/Boot/Stage2/stage2.asm
KERNEL_SRC := $(SOURCES)/kernel.c
KERNEL_ENTRY := $(SOURCES)/kernel_entry.s
VGA_SRC := $(SOURCES)/vga.c
IO_SRC := $(SOURCES)/io.c
LINKER_SCRIPT := $(PROJECT_ROOT)/linker.ld

# Output binaries and objects
STAGE1_BIN := $(PROJECT_ROOT)/Boot/Stage1/stage1.bin
STAGE2_BIN := $(PROJECT_ROOT)/Boot/Stage2/stage2.bin
KERNEL_BIN := $(BINS)/kernel.bin
KERNEL_ELF := $(BINS)/kernel.elf
IMG := $(PROJECT_ROOT)/bootloader.img

# Object files
KERNEL_OBJ := $(OBJECTS)/kernel.o
VGA_OBJ := $(OBJECTS)/vga.o
IO_OBJ := $(OBJECTS)/io.o
KERNEL_ENTRY_OBJ := $(OBJECTS)/kernel_entry.o

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

# Compile and link the kernel, generate kernel.elf and kernel.bin
$(KERNEL_BIN): $(OBJECTS) $(BINS) $(KERNEL_SRC) $(KERNEL_ENTRY) $(VGA_SRC) $(IO_SRC) $(LINKER_SCRIPT)
	@echo "Compiling and linking Kernel..."
	i686-elf-gcc $(CFLAGS) -c "$(KERNEL_SRC)" -o "$(KERNEL_OBJ)"
	i686-elf-gcc $(CFLAGS) -c "$(KERNEL_ENTRY)" -o "$(KERNEL_ENTRY_OBJ)"
	i686-elf-gcc $(CFLAGS) -c "$(VGA_SRC)" -o "$(VGA_OBJ)"  # Compile vga.c
	i686-elf-gcc $(CFLAGS) -c "$(IO_SRC)" -o "$(IO_OBJ)"  # Compile io.c
	i686-elf-ld -o $(KERNEL_ELF) -T $(LINKER_SCRIPT) $(KERNEL_OBJ) $(KERNEL_ENTRY_OBJ) $(VGA_OBJ) $(IO_OBJ)  # Link io.o
	i686-elf-objcopy -O binary $(KERNEL_ELF) $(KERNEL_BIN)

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
	      "$(KERNEL_OBJ)" "$(KERNEL_ENTRY_OBJ)" "$(VGA_OBJ)" "$(IO_OBJ)"

# Phony targets
.PHONY: clean output