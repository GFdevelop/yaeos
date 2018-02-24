include makefile.inc

all: $(EXE) shared static

$(EXE):
	$(MAKE) -C example

shared:
	$(MAKE) -C src shared

static:
	$(MAKE) -C src static

clean: $(addsuffix clean, src example)

%clean: %
	$(MAKE) -C $< clean

.PHONY: all shared static clean
