SUBDIRS = src/vm src/as src/scm src/bios src/bootloader 

all: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@ --no-print-directory

.PHONY: all $(SUBDIRS)

run:
	./vm.elf -rom bios.bin -disk disk.img
clean:
	@$(MAKE) -C src/vm clean --no-print-directory
	@$(MAKE) -C src/as clean --no-print-directory
	@$(MAKE) -C src/scm clean --no-print-directory
	@$(MAKE) -C src/bios clean --no-print-directory
	@$(MAKE) -C src/bootloader clean --no-print-directory
	@echo "Cleaned."


