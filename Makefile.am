lib_LTLIBRARIES = libkvmem.la
libkvmem_la_SOURCES = src/kvmem.c src/kvmem_elf.c src/kvmem_io.c
nodist_libkvmem_la_SOURCES = $(srcdir)/include/kvmem.h
BUILT_SOURCES = $(srcdir)/include/kvmem.h
include_HEADERS= \
	 $(srcdir)/include/kvmem.h\
	 $(srcdir)/include/kvmem_io.h

libkvmem_la_HEADERS =\
	 $(srcdir)/include/kvmem.h\
	 $(srcdir)/include/kvmem_elf.h\
	 $(srcdir)/include/kvmem_io.h

libkvmem_ladir = .
libkvmem_la_CFLAGS = -I$(srcdir)/include
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool

