PROJECT_ROOT := $(CURDIR)

STAGE1_SRC := $(PROJECT_ROOT)/Boot/Stage1/stage1.asm
STAGE2_SRC := $(PROJECT_ROOT)/Boot/Stage2/stage2.asm

STAGE1_BIN := $(PROJECT_ROOT)/Boot/Stage1/stage1.bin
STAGE2_BIN := $(PROJECT_ROOT)/Boot/Stage2/stage2.bin

IMG := $(PROJECT_ROOT)/bootloader.img

QEMU_IMG := qemu-img
DD := dd

# Always clean before building
output: clean $(STAGE1_BIN) $(STAGE2_BIN) $(IMG)

$(STAGE1_BIN): $(STAGE1_SRC)
	@echo "Assembling Stage 1..."
	nasm -f bin "$(STAGE1_SRC)" -o "$(STAGE1_BIN)"

$(STAGE2_BIN): $(STAGE2_SRC)
	@echo "Assembling Stage 2..."
	nasm -f bin "$(STAGE2_SRC)" -o "$(STAGE2_BIN)"

$(IMG): $(STAGE1_BIN) $(STAGE2_BIN)
	@echo "Creating bootloader.img..."
	$(QEMU_IMG) create -f raw "$(IMG)" 10M
	$(DD) if=$(STAGE1_BIN) of=$(IMG) bs=512 count=1
	$(DD) if=$(STAGE2_BIN) of=$(IMG) bs=512 seek=1

clean:
	@echo "Cleaning up..."
	rm -f "$(STAGE1_BIN)" "$(STAGE2_BIN)" "$(IMG)"