PROJECT_ROOT := $(CURDIR)

STAGE1_SRC := $(PROJECT_ROOT)\Boot\Stage1\stage1.asm
STAGE2_SRC := $(PROJECT_ROOT)\Boot\Stage2\stage2.asm

STAGE1_BIN := $(PROJECT_ROOT)\Boot\Stage1\stage1.bin
STAGE2_BIN := $(PROJECT_ROOT)\Boot\Stage2\stage2.bin

IMG := C:\Users\ilanv\OS_32Bit\Code\bootloader.img

QEMU_IMG := "C:/QEMU/qemu-img.exe"
DD := "C:/dd/dd.exe"

output: $(STAGE1_BIN) $(STAGE2_BIN) $(IMG)

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
	if exist "$(STAGE1_BIN)" del "$(STAGE1_BIN)"
	if exist "$(STAGE2_BIN)" del "$(STAGE2_BIN)"
	if exist "$(IMG)" del "$(IMG)"