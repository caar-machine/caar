MAIN_FILE = main.lisp
BINARY = ../../bios.bin
AS = ../../as.elf

$(BINARY): $(MAIN_FILE)
	@$(AS) $< -o $@
	@echo "Built $(shell basename $(BINARY))"

clean:
	rm -rf $(BINARY)


