
ifneq ($(words $(MAKECMDGOALS)),1)
.DEFAULT_GOAL = all
%:
	@$(MAKE) $@ --no-print-directory -rRf $(firstword $(MAKEFILE_LIST))
else
ifndef ECHO
T := $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory -nrRf $(firstword $(MAKEFILE_LIST)) ECHO="COUNTTHIS" | grep -c "COUNTTHIS")
N := x
C = $(words $N)$(eval N := x $N)
ECHO=printf "[$C/$T] $1     \r"
endif

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
	@$(call ECHO,LD $@)
	@$(CC) -o $@ $^ $(LDFLAGS)

clean:
	 @-rm -rf $(BUILD_DIR) $(BINARY)
endif
