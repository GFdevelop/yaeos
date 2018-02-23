include makefile.inc

DIRS= src example

all: $(DIRS)

$(DIRS):src example
	$(MAKE) -C $^

.PHONY: $(DIRS)

#.PHONY: clean all
