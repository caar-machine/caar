SUBDIRS := src/vm src/as src/scm

all: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@ --no-print-directory


.PHONY: all $(SUBDIRS)

clean:
	@$(MAKE) -C src/vm clean --no-print-directory
	@$(MAKE) -C src/as clean --no-print-directory
	@$(MAKE) -C src/scm clean --no-print-directory
	@echo "Cleaned."



