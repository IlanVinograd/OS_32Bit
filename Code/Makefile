PROJECT_ROOT := $(CURDIR)

STAGE1_SRC := $(PROJECT_ROOT)/Boot/Stage1/stage1.asm
STAGE2_SRC := $(PROJECT_ROOT)/Boot/Stage2/stage2.asm
KERNEL_SRC := $(PROJECT_ROOT)/Kernel/kernel.c
KERNEL_ENTRY  := $(PROJECT_ROOT)/Kernel/kernel_entry.s
LINKER_SCRIPT := $(PROJECT_ROOT)/linker.ld

STAGE1_BIN := $(PROJECT_ROOT)/Boot/Stage1/stage1.bin
STAGE2_BIN := $(PROJECT_ROOT)/Boot/Stage2/stage2.bin
KERNEL_BIN := $(PROJECT_ROOT)/Kernel/kernel.bin
KERNEL_ELF := $(PROJECT_ROOT)/Kernel/kernel.elf

IMG := $(PROJECT_ROOT)/bootloader.img

QEMU_IMG := qemu-img
DD := dd

# Always clean before building
output: clean $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_BIN) $(IMG)

$(STAGE1_BIN): $(STAGE1_SRC)
	@echo "Assembling Stage 1..."
	nasm -f bin "$(STAGE1_SRC)" -o "$(STAGE1_BIN)"

$(STAGE2_BIN): $(STAGE2_SRC)
	@echo "Assembling Stage 2..."
	nasm -f bin "$(STAGE2_SRC)" -o "$(STAGE2_BIN)"

$(KERNEL_BIN): $(KERNEL_SRC) $(LINKER_SCRIPT)
	@echo "Compiling and linking Kernel..."
	i686-elf-gcc -ffreestanding -c "$(KERNEL_SRC)" -o "$(PROJECT_ROOT)/Kernel/kernel.o"
	i686-elf-gcc -ffreestanding -c "$(KERNEL_ENTRY)" -o "$(PROJECT_ROOT)/Kernel/kernel_entry.o"
	i686-elf-ld -o $(KERNEL_ELF) -T $(LINKER_SCRIPT) $(PROJECT_ROOT)/Kernel/kernel.o \
                    $(PROJECT_ROOT)/Kernel/kernel_entry.o
	i686-elf-objcopy -O binary $(KERNEL_ELF) $(KERNEL_BIN)

$(IMG): $(STAGE1_BIN) $(STAGE2_BIN) $(KERNEL_BIN)
	@echo "Creating bootloader.img..."
	$(QEMU_IMG) create -f raw "$(IMG)" 10M
	$(DD) if=$(STAGE1_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	$(DD) if=$(STAGE2_BIN) of=$(IMG) bs=512 seek=1 conv=notrunc
	$(DD) if=$(KERNEL_BIN) of=$(IMG) bs=512 seek=2 conv=notrunc

clean:
	@echo "Cleaning up..."
	rm -f "$(STAGE1_BIN)" "$(STAGE2_BIN)" "$(KERNEL_BIN)" \
              "$(KERNEL_ELF)" "$(IMG)" "$(PROJECT_ROOT)/Kernel/kernel.o" "$(PROJECT_ROOT)/Kernel/kernel_entry.o"