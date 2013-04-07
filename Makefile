INCDIR = include
CC_OPTIONS = -I $(INCDIR) -fPIC -g
CC_TEST_OPTIONS= -I $(INCDIR)

LINKER_OPTIONS = -shared -soname libkvmem.so.1 -export-dynamic
KVMEM_CFILES = $(wildcard src/*.c)
KVMEM_OBJFILES = $(foreach obj,$(KVMEM_CFILES),$(obj:src/%.c=bin/%.o))
KVMEM_TESTFILES = $(wildcard tests/*.c)
KVMEM_TOBJFILES = $(foreach obj,$(KVMEM_TESTFILES),$(obj:tests/%.c=bin/tests/%))

KVMEM_SRCDIR = src
KVMEM_OBJDIR = bin
KVMEM_TESTDIR = tests

KVMEM_LIB = $(KVMEM_OBJDIR)/libkvmem.so.1

CC = gcc
LINKER = ld

.PHONY: install all clean tests

$(KVMEM_LIB):$(KVMEM_OBJFILES)
	@echo [KVMEM] Linking \\t$(KVMEM_OBJFILES)
	@$(LINKER) $(LINKER_OPTIONS) $(KVMEM_OBJFILES) -o $@

$(KVMEM_OBJDIR):
	@echo [KVMEM] Initiating
	@mkdir bin bin/tests

$(KVMEM_OBJDIR)/%.o:$(KVMEM_SRCDIR)/%.c $(KVMEM_OBJDIR)
	@echo [KVMEM] Compiling \\t$<
	@$(CC) $(CC_OPTIONS) -c -o $@ $<
	
tests:$(KVMEM_TOBJFILES)
	@sudo $<
$(KVMEM_OBJDIR)/$(KVMEM_TESTDIR)/%:$(KVMEM_TESTDIR)/%.c
	@echo [KVMEM] Building Tests
	@$(CC) $< $(KVMEM_LIB) $(CC_TEST_OPTIONS) -o $@

clean:
	@echo [KVMEM] cleaning
	@rm -rf bin
	
install:
	@echo [KVMEM] Installing.
	@cp  -f bin/libkvmem.so.1 /usr/lib/

