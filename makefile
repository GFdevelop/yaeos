include makefile.inc

# trick: in future we add new p%test in $(EXE)
all: $(EXE) static

# trick: for future p2test or p3test
p%test: bin
	$(MAKE) -C example $@
# copy p%test in bin/ only if date of file is newer
	cp -u example/$@ $</

static: $(LIBS)

lib%: lib
	$(MAKE) -C src $@
	cp -u src/$@.a $</

# this is "a check", if you do 'make libpcb libasl' it make "lib" directory only one time
bin lib:
	mkdir -p $@

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
	$(info [OK] $(SRCTAR).tar.gz)

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
	$(info [OK] $(RELTAR).tar.gz)

clean:
	$(MAKE) -C src clean
	$(MAKE) -C example clean

cleanall: clean
	-rm -f -r bin lib *tar.gz *.sha1sum

.PHONY: all p%test static source release clean cleanall
