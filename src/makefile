include ../makefile.inc

all static: $(LIBS)
	$(info [OK] static libraries)

lib%: lib%.a
	$(info [OK] $@)


lib%.a: %.o
	$(AR) rcs $@ $<

clean:
	-rm -f *.o *.a

.PHONY: all static lib% clean
