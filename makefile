include makefile.inc

# in future we add new p2test in $(EXE)
all: $(EXE) static

# for future p2test
p%test:
	$(MAKE) -C example $@
	mkdir -p bin
# copy p%test in bin/ only if date of file is newer
	#cp -u example/$@ bin/
	cp example/$@ bin/

static $(LIBS):
	$(MAKE) -C src $@
	mkdir -p lib
	#cp -u src/*.a lib/
	cp src/*.a lib/

source:
	tar --exclude=*.tar.gz \
		--exclude=*.sha1sum \
		--exclude=*.o \
		--exclude=*.a \
		--exclude=p1test \
		--exclude=bin \
		--exclude=lib \
		-czf $(SRCTAR).tar.gz *
		gzip -t $(SRCTAR).tar.gz
		sha1sum $(SRCTAR).tar.gz > $(SRCTAR).sha1sum
	echo [OK] $(SRCTAR).tar.gz

release: all
	tar --exclude=*.tar.gz \
		--exclude=*.sha1sum \
		--exclude=*.o \
		--exclude=example \
		--exclude=src \
		--exclude=makefile \
		--exclude=makefile* \
		--transform='s|^bin|usr/bin|' \
		--transform='s|^lib|usr/lib/$(PROJ)|' \
		--transform='s|^doc|usr/share/doc/$(PROJ)|' \
		--transform='s|^include|usr/include/$(PROJ)|' \
		-czf $(RELTAR).tar.gz *
		gzip -t $(RELTAR).tar.gz
		sha1sum $(RELTAR).tar.gz > $(RELTAR).sha1sum
	echo [OK] $(RELTAR).tar.gz

clean:
	$(MAKE) -C src clean
	$(MAKE) -C example clean

cleanall: clean
	-rm -f -r bin lib *tar.gz *.sha1sum

help:
	echo "OPTIONS:   [all][p1test][static][libpcb][libasl]"
	echo "           [source][release][cleanall][help]"
	echo
	echo "[all]            p1test and static"
	echo "[p1test]         executable p1test"
	echo "[static]         static libraries libpcb and libasl"
	echo "[libpcb]         static library libpcb"
	echo "[libasl]         static library libasl"
	echo "[source]         tar.gz of source with sha1sum (require optional deps)"
	echo "[release]        tar.gz of release with sha1sum (require optional deps)"
	echo "[clean]          clean subdirectory example/ and src/"
	echo "[cleanall]       clean subdirectory and remove bin/ lib/ *tar.gz *.sha1sum"
	echo "[help]           show this message"
	echo

.PHONY: all p%test static source release clean cleanall help
