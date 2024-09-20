PROJECT_ROOT := $(CURDIR)

# Bootloader source and binary files
STAGE1_SRC := $(PROJECT_ROOT)/Boot/Stage1/stage1.asm
STAGE2_SRC := $(PROJECT_ROOT)/Boot/Stage2/stage2.asm
STAGE1_BIN := $(PROJECT_ROOT)/bin/stage1.bin
STAGE2_BIN := $(PROJECT_ROOT)/bin/stage2.bin

# Kernel source and object files
KERNEL_SRC := $(wildcard $(PROJECT_ROOT)/Kernel/source/*.c)
KERNEL_OBJS := $(KERNEL_SRC:.c=.o) $(KERNEL_ASM_OBJS)
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
NASM_FLAGS := -f elf32
GCC_FLAGS := -ffreestanding -mno-red-zone -m32 -g
LD_FLAGS := -T $(PROJECT_ROOT)/link.ld
BIN_DIR := $(PROJECT_ROOT)/bin

# Create bin directory if not exists
$(BIN_DIR):
	@echo "Creating bin directory..."
	@mkdir -p $(BIN_DIR)

# Assemble Stage 1 bootloader
$(STAGE1_BIN): $(STAGE1_SRC) | $(BIN_DIR)
	@echo "Assembling Stage 1..."
	@$(NASM) -f bin $(STAGE1_SRC) -o $(STAGE1_BIN)
	@echo "Stage 1 assembled."

# Assemble Stage 2 bootloader
$(STAGE2_BIN): $(STAGE2_SRC) | $(BIN_DIR)
	@echo "Assembling Stage 2..."
	@$(NASM) -f bin $(STAGE2_SRC) -o $(STAGE2_BIN)
	@echo "Stage 2 assembled."

# Compile kernel C source files to object files
%.o: %.c | $(BIN_DIR)
	@echo "Compiling $<..."
	@$(GCC) $(GCC_FLAGS) -c $< -o $@
	@echo "Compiled $< to $@"

# Link all object files into the kernel binary
$(KERNEL_BIN): $(KERNEL_OBJS) | $(BIN_DIR)
	@echo "Linking kernel and load_kernel.o..."
	@$(LD) $(LD_FLAGS) -o $(KERNEL_BIN) $(KERNEL_OBJS)
	@echo "Linking completed."

# Create the bootloader image and write all components
$(IMG): $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_BIN)
	@$(QEMU_IMG) create -f raw $(IMG) 10M
	@$(DD) if=$(STAGE1_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	@$(DD) if=$(STAGE2_BIN) of=$(IMG) bs=512 seek=1 conv=notrunc
	@$(DD) if=$(KERNEL_BIN) of=$(IMG) bs=512 seek=2 conv=notrunc

# Build everything: bootloader and kernel
output: $(BIN_DIR) $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_BIN) $(IMG)
	@echo "Build complete. Final contents of bin directory:"
	@ls -l $(BIN_DIR)

# Clean all generated files
clean:
	@echo "Cleaning up..."
	@rm -rf $(BIN_DIR) $(IMG)
