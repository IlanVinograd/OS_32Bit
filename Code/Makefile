PROJECT_ROOT := $(CURDIR)

STAGE1_SRC := $(PROJECT_ROOT)\Boot\Stage1\stage1.asm
STAGE2_SRC := $(PROJECT_ROOT)\Boot\Stage2\stage2.asm

STAGE1_BIN := $(PROJECT_ROOT)\Boot\Stage1\stage1.bin
STAGE2_BIN := $(PROJECT_ROOT)\Boot\Stage2\stage2.bin

output:
	@echo $(STAGE1_SRC)
	@echo $(STAGE2_SRC)
	nasm -f bin "$(STAGE1_SRC)" -o "$(STAGE1_BIN)"
	nasm -f bin "$(STAGE2_SRC)" -o "$(STAGE2_BIN)"

clean:
	@echo $(STAGE1_BIN)
	@echo $(STAGE2_BIN)
	if exist "$(STAGE1_BIN)" echo "Found $(STAGE1_BIN)" && del "$(STAGE1_BIN)"
	if exist "$(STAGE2_BIN)" echo "Found $(STAGE2_BIN)" && del "$(STAGE2_BIN)"