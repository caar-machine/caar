
CC ?= gcc
CFILES += $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)
HFILES += $(wildcard $(SRC_DIR)/*.h) $(wildcard $(SRC_DIR)/*/*.h)
OFILES += $(patsubst %.c, $(BUILD_DIR)/%.o, $(CFILES))
CFLAGS += -O2  

BINARY = ../../$(TARGET).elf

BUILD_DIR = ../../build/$(TARGET)

all: $(BINARY)

$(BUILD_DIR)/%.o: %.c $(HFILES)
	@mkdir -p $(@D)
	@$(call ECHO,CC $@)
	@$(CC) $(CFLAGS) -c -o $@ $< 

$(BINARY): $(OFILES)
	@mkdir -p $(@D)
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo "Built $(TARGET)"

clean:
	 @-rm -rf $(BUILD_DIR) $(BINARY)