include ../makefile.inc

all libs: $(LIBS)
	$(info [OK] libraries)

lib%: %.o
	$(AR) rcs $@.a $<

clean:
	-rm -f *.o *.a

.PHONY: all libs clean
