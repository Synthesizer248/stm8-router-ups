PROJECT := router_ups
MCU := stm8s103f3
CC := sdcc
FLASH := stm8flash

SRC := src/main.c
INC := -Iinc
BUILD := build
IHX := $(BUILD)/$(PROJECT).ihx

CFLAGS := -mstm8 --std-c11 --opt-code-size --out-fmt-ihx $(INC)

all: $(IHX)

$(BUILD):
	mkdir -p $(BUILD)

$(IHX): $(SRC) | $(BUILD)
	$(CC) $(CFLAGS) -o $(IHX) $(SRC)

flash: $(IHX)
	$(FLASH) -c stlinkv2 -p $(MCU) -w $(IHX)

clean:
	$(RM) -f $(BUILD)/*

.PHONY: all flash clean
