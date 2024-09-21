# Makefile for building a simple 32-bit OS

# Directories and files
BIN_DIR := bin
BOOT_DIR := Boot
KERNEL_DIR := Kernel
IMG := bootloader.img

# Tools
NASM := nasm
GCC := i686-elf-gcc
LD := i686-elf-ld
QEMU_IMG := qemu-img
DD := dd

# Compiler and linker flags
GCC_FLAGS := -ffreestanding -m32 -g
LD_FLAGS := -T linker.ld

# Bootloader and kernel files
STAGE1_SRC := $(BOOT_DIR)/Stage1/stage1.asm
STAGE2_SRC := $(BOOT_DIR)/Stage2/stage2.asm
STAGE1_BIN := $(BIN_DIR)/stage1.bin
STAGE2_BIN := $(BIN_DIR)/stage2.bin
KERNEL_SRC := $(wildcard $(KERNEL_DIR)/source/*.c)
KERNEL_ASM_SRC := $(KERNEL_DIR)/source/Kernel_Entery.asm
KERNEL_OBJ := $(BIN_DIR)/kernel_entry.o $(patsubst $(KERNEL_DIR)/source/%.c, $(BIN_DIR)/%.o, $(KERNEL_SRC))
KERNEL_BIN := $(BIN_DIR)/kernel.bin

# Calculate the number of sectors required for stage2.bin and kernel.bin
STAGE2_SECTORS := 63
KERNEL_SECTORS := $(shell echo $$(( ($(shell stat -c%s $(KERNEL_BIN)) + 511) / 512 )))
KERNEL_START_LBA := $(shell echo $$((1 + $(STAGE2_SECTORS))))

# Build everything
all: output

# Create bin directory
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Stage 1 bootloader
$(STAGE1_BIN): $(STAGE1_SRC) | $(BIN_DIR)
	$(NASM) -f bin $< -o $@

# Stage 2 bootloader
$(STAGE2_BIN): $(STAGE2_SRC) $(KERNEL_BIN) | $(BIN_DIR)
	$(NASM) -f bin $< -o $@ -D KERNEL_SECTORS=$(KERNEL_SECTORS) -D KERNEL_START_LBA=$(KERNEL_START_LBA)

# Kernel entry assembly
$(BIN_DIR)/kernel_entry.o: $(KERNEL_ASM_SRC) | $(BIN_DIR)
	$(NASM) -f elf32 $< -o $@

# Kernel C files
$(BIN_DIR)/%.o: $(KERNEL_DIR)/source/%.c | $(BIN_DIR)
	$(GCC) $(GCC_FLAGS) -c $< -o $@

# Link kernel
$(KERNEL_BIN): $(KERNEL_OBJ)
	$(LD) $(LD_FLAGS) -o $@ $^

# Create disk image
$(IMG): $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_BIN)
	$(QEMU_IMG) create $(IMG) 10M
	$(DD) if=$(STAGE1_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	$(DD) if=$(STAGE2_BIN) of=$(IMG) bs=512 seek=1 count=$(STAGE2_SECTORS) conv=notrunc
	$(DD) if=$(KERNEL_BIN) of=$(IMG) bs=512 seek=$(KERNEL_START_LBA) count=$(KERNEL_SECTORS) conv=notrunc

# Output the contents of the bin directory
output: $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_BIN) $(IMG)
	@echo "Build complete. Final contents of bin directory:"
	@ls -l $(BIN_DIR)

# Clean
clean:
	rm -f $(BIN_DIR)/* $(IMG)
