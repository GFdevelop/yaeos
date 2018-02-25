include ../makefile.inc

all static: $(LIBS)

lib%: lib%.a
	# fake recipe

lib%.a: %.o
	$(AR) rcs $@ $<
	echo [OK] lib$*

clean:
	-rm -f *.o *.a

help:
	echo "OPTIONS:   [all][static][libpcb][libasl][help]"
	echo
	echo "[all][static]    for make libpcb and libasl"
	echo "[libpcb]         for make libpcb"
	echo "[libasl]         for make libasl"
	echo "[clean]          remove *.o and *.a"
	echo "[help]           show this message"
	echo

.PHONY: all static lib% clean help
