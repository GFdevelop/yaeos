include ../makefile.inc

all static: $(LIBS)

lib%: lib%.a
	# fake recipe

lib%.a: %.o
	$(AR) rcs $@ $<
	echo [OK] lib$*

clean:
	-rm -f *.o *.a

.PHONY: all static lib% clean
