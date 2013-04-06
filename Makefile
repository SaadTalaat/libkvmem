INCDIR = include
CC_OPTIONS = -I $(INCDIR) -fPIC
LINKER_OPTIONS = -shared -soname=libkvmem
KVMEM_CFILES = $(wildcard src/*.c)
KVMEM_OBJFILES = $(foreach obj,$(KVMEM_CFILES),$(obj:src/%.c=bin/%.o))
KVMEM_SRCDIR = src
KVMEM_OBJDIR = bin
KVMEM_LIB = $(KVMEM_OBJDIR)/libkvmem.so
CC = gcc
LINKER = ld

.PHONY: install all

$(KVMEM_LIB):$(KVMEM_OBJFILES)
	@echo [KVMEM] Linking
	$(LINKER) $(LINKER_OPTIONS) $(KVMEM_OBJFILES) -o $@

$(KVMEM_OBJDIR):
	@echo [KVMEM] Initiating
	@mkdir bin

$(KVMEM_OBJDIR)/%.o:$(KVMEM_SRCDIR)/%.c $(KVMEM_OBJDIR)
	@echo [KVMEM] Compiling
	$(CC) $(CC_OPTIONS) -c -o $@ $<
	
clean:
	@echo [KVMEM] cleaning
	@rm -rf bin
