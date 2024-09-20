# Makefile for building a simple 32-bit OS

PROJECT_ROOT := $(CURDIR)

# Bootloader source and binary files
STAGE1_SRC := $(PROJECT_ROOT)/Boot/Stage1/stage1.asm
STAGE2_SRC := $(PROJECT_ROOT)/Boot/Stage2/stage2.asm
STAGE1_BIN := $(PROJECT_ROOT)/bin/stage1.bin
STAGE2_BIN := $(PROJECT_ROOT)/bin/stage2.bin

# Kernel source and object files
KERNEL_SRC := $(wildcard $(PROJECT_ROOT)/Kernel/source/*.c)
KERNEL_OBJS := $(patsubst $(PROJECT_ROOT)/Kernel/source/%.c,$(PROJECT_ROOT)/bin/%.o,$(KERNEL_SRC))
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
GCC_FLAGS := -ffreestanding -mno-red-zone -m32 -g
LD_KERNEL := -T $(PROJECT_ROOT)/linkerForKernal.ld
BIN_DIR := $(PROJECT_ROOT)/bin


KERNEL_SIZE := $(shell wc -c < $(KERNEL_BIN))
KERNEL_SECTORS := $(shell echo $$(( ($(KERNEL_SIZE) + 511) / 512 )))

# Create bin directory if it doesn't exist
$(BIN_DIR):
	@echo "Creating bin directory..."
	@mkdir -p $(BIN_DIR)

# Assemble Stage 1 bootloader into a raw binary
$(STAGE1_BIN): $(STAGE1_SRC) | $(BIN_DIR)
	@echo "Assembling Stage 1..."
	@$(NASM) -f bin $(STAGE1_SRC) -o $(STAGE1_BIN)
	@echo "Stage 1 assembled."

# Assemble Stage 2 bootloader with ATA_PIO included into a flat binary
$(STAGE2_BIN): $(STAGE2_SRC) $(PROJECT_ROOT)/Boot/Stage2/ATA_PIO.asm $(KERNEL_BIN) | $(BIN_DIR)
	@echo "Assembling Stage 2 with ATA_PIO..."
	@$(NASM) -f bin $(STAGE2_SRC) -o $(STAGE2_BIN) -D KERNEL_SECTORS=$(KERNEL_SECTORS) -i $(dir $(STAGE2_SRC))
	@echo "Stage 2 assembled."
	
# Compile kernel C source files to object files
$(PROJECT_ROOT)/bin/%.o: $(PROJECT_ROOT)/Kernel/source/%.c | $(BIN_DIR)
	@echo "Compiling $<..."
	@$(GCC) $(GCC_FLAGS) -c $< -o $@
	@echo "Compiled $< to $@"

# Link all object files together for the final kernel binary
$(KERNEL_BIN): $(KERNEL_OBJS)
	@echo "Linking kernel..."
	@$(LD) $(LD_KERNEL) -o $(KERNEL_BIN) $(KERNEL_OBJS)
	@echo "Kernel linked."

# Build the bootloader disk image
$(IMG): $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_BIN)
	@echo "Creating disk image..."
	@$(QEMU_IMG) create $(IMG) 10M
	@$(DD) if=$(STAGE1_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	@$(DD) if=$(STAGE2_BIN) of=$(IMG) bs=512 seek=1 count=2 conv=notrunc
	@$(DD) if=$(KERNEL_BIN) of=$(IMG) bs=512 seek=3 conv=notrunc
	@echo "Disk image created."

# Build everything: bootloader and kernel
output: $(BIN_DIR) $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_BIN) $(IMG)
	@echo "Build complete. Final contents of bin directory:"
	@ls -l $(BIN_DIR)

# Clean all generated files
clean:
	@echo "Cleaning up..."
	@rm -f $(BIN_DIR)/*
	@rm -f $(IMG)
	@echo "Cleanup completed."
