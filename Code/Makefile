PROJECT_ROOT := $(CURDIR)

# Bootloader source and binary files
STAGE1_SRC := $(PROJECT_ROOT)/Boot/Stage1/stage1.asm
STAGE2_SRC := $(PROJECT_ROOT)/Boot/Stage2/stage2.asm
STAGE1_BIN := $(PROJECT_ROOT)/bin/stage1.bin
STAGE2_BIN := $(PROJECT_ROOT)/bin/stage2.bin

# Kernel source and object files
KERNEL_SRC := $(wildcard $(PROJECT_ROOT)/Kernel/source/*.c)
KERNEL_ASM_SRC := $(wildcard $(PROJECT_ROOT)/Kernel/source/*.asm)
KERNEL_OBJS := $(KERNEL_SRC:.c=.o) $(KERNEL_ASM_SRC:.asm=.o)
KERNEL_BIN := $(PROJECT_ROOT)/bin/kernel.bin

# Disk image
IMG := $(PROJECT_ROOT)/bootloader.img

# Tools
NASM := nasm
GCC := i686-elf-gcc
LD := i686-elf-ld
QEMU_IMG := qemu-img
DD := dd

# Flags
NASM_FLAGS := -f bin
GCC_FLAGS := -Ttext 0x8000 -ffreestanding -mno-red-zone -m32
LD_FLAGS := -T $(PROJECT_ROOT)/link.ld

# Ensure the bin directory exists
BIN_DIR := $(PROJECT_ROOT)/bin

# Build everything: bootloader and kernel
output: clean $(BIN_DIR) $(STAGE1_BIN) $(STAGE2_BIN) kernel $(IMG)

# Create bin directory if not exists
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Assemble bootloader stages
$(STAGE1_BIN): $(STAGE1_SRC) | $(BIN_DIR)
	@echo "Assembling Stage 1..."
	$(NASM) $(NASM_FLAGS) "$(STAGE1_SRC)" -o "$(STAGE1_BIN)"

$(STAGE2_BIN): $(STAGE2_SRC) | $(BIN_DIR)
	@echo "Assembling Stage 2..."
	$(NASM) $(NASM_FLAGS) "$(STAGE2_SRC)" -o "$(STAGE2_BIN)"

# Compile kernel C source files to object files
%.o: %.c | $(BIN_DIR)
	@echo "Compiling $<..."
	$(GCC) $(GCC_FLAGS) -c "$<" -o "$@"

# Assemble kernel assembly source files to object files
%.o: %.asm | $(BIN_DIR)
	@echo "Assembling $<..."
	$(NASM) -f elf32 "$<" -o "$@"

# Link all object files into the kernel binary
kernel: $(KERNEL_OBJS) | $(BIN_DIR)
	@echo "Linking kernel..."
	$(LD) $(LD_FLAGS) -o $(KERNEL_BIN) $(KERNEL_OBJS)

# Create the bootloader image and write all components
$(IMG): $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_BIN)
	@echo "Creating bootloader.img..."
	$(QEMU_IMG) create -f raw "$(IMG)" 10M
	@echo "Writing Stage 1..."
	$(DD) if=$(STAGE1_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	@echo "Writing Stage 2..."
	$(DD) if=$(STAGE2_BIN) of=$(IMG) bs=512 seek=1 conv=notrunc
	@echo "Writing Kernel..."
	$(DD) if=$(KERNEL_BIN) of=$(IMG) bs=512 seek=100 conv=notrunc

# Clean all generated files
clean:
	@echo "Cleaning up..."
	rm -f $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_OBJS) $(KERNEL_BIN) $(IMG)
