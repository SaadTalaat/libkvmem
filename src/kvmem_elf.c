/*-
 * Copyrights (c) 2013 
 *              Saad Talaat <saadtalaat[at]gmail[dot]com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <a.out.h>
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <kvmem_elf.h>


static void
_elf_to_nlist_sym(list, syms, shdr, shnum)
struct nlist *list;
#ifdef KVMEM_32
Elf32_Sym *syms;
Elf32_Shdr *shdr;
#else
Elf64_Sym *syms;
Elf64_Shdr *shdr;
#endif
int shnum;
{
	list->n_value = syms->st_value;
	switch(syms->st_shndx)
	{
		case SHN_UNDEF:
		case SHN_COMMON:
			list->n_type = N_UNDF;
			break;
		case SHN_ABS:
#ifdef KVMEM_32
			list->n_type = ELF32_ST_TYPE(syms->st_info) == STT_FILE? N_FN : N_ABS;
#else
			list->n_type = ELF64_ST_TYPE(syms->st_info) == STT_FILE? N_FN : N_ABS;
#endif
			break;
		default:
			if(syms->st_shndx >= shnum)
				list->n_type = N_UNDF;
			else
			{
#ifdef KVMEM_32
				Elf32_Shdr *sh = shdr + syms->st_shndx;
#else
				Elf64_Shdr *sh = shdr + syms->st_shndx;
#endif
				list->n_type = sh->sh_type == SHT_PROGBITS ? 
						(sh->sh_flags & SHF_WRITE? N_DATA : N_TEXT):
						(sh->sh_type == SHT_NOBITS ? N_BSS : N_UNDF);
			} 
			break;
	}

#ifdef KVMEM_32
	if(ELF32_ST_BIND(syms->st_info) == STB_GLOBAL || ELF32_ST_BIND(syms->st_info) == STB_WEAK)
#else
	if(ELF64_ST_BIND(syms->st_info) == STB_GLOBAL || ELF64_ST_BIND(syms->st_info) == STB_WEAK)
#endif
		list->n_type |= N_EXT;

}


static int
_check_elf(elf)
#ifdef KVMEM_32
Elf32_Ehdr *elf;
#else
Elf64_Ehdr *elf;
#endif
{
	if(elf == NULL)
		return 0;
	if( (strncmp(elf->e_ident, ELFMAG,SELFMAG) == 0) && elf->e_ident[EI_CLASS] == ELF_TARGET_CLASS &&
		elf->e_ident[EI_DATA] == ELF_TARGET_DATA && elf->e_ident[EI_VERSION] == EV_CURRENT)
	{
		if(elf->e_machine == ELF_TARGET_MACHINE && elf->e_version == EV_CURRENT)
			return 1;

	}
	return 0;
}


int
_elf_fdnlist(int fd, struct nlist* list)
{

	struct nlist *p;	//tmp
#ifdef KVMEM_32
	Elf32_Off symoff = 0, symstroff = 0;
	Elf32_Word symsize = 0, symstrsize = 0;
	Elf32_Sword i,j;
	Elf32_Sym syms[1024], *syms_2;	//1024 symbol, doesn't 81xxx symbol but we'll see later
	Elf32_Ehdr ehdr;
	Elf32_Shdr *shdr = NULL;
	Elf32_Word shdr_size;
#else
	Elf64_Off symoff = 0, symstroff = 0;
	Elf64_Word symsize = 0, symstrsize = 0;
	Elf64_Sword i,j;
	Elf64_Sym syms[1024], *syms_2;	//1024 symbol, doesn't 81xxx symbol but we'll see later
	Elf64_Ehdr ehdr;
	Elf64_Shdr *shdr = NULL;
	Elf64_Word shdr_size;
#endif
	unsigned int entrynum = 0;
	void *base;
	char *strtab = NULL;
	struct stat st;


	if(fd <= 0 )
		return -1;
	if(lseek(fd, (off_t) 0, SEEK_SET) == -1 || read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr) || !_check_elf(&ehdr) || fstat(fd, &st) < 0)
		return -1;

	shdr_size = ehdr.e_shentsize * ehdr.e_shnum;
	base = mmap(NULL, (size_t) shdr_size, PROT_READ, 0, fd, (off_t) ehdr.e_shoff);
	if(base == MAP_FAILED)
	{
		munmap(base, (size_t) shdr_size);
		return -1;
	}
	
#ifdef KVMEM_32
	shdr = (Elf32_Shdr *) base;
#else
	shdr = (Elf64_Shdr *) base;
#endif

	for(i = 0; i < ehdr.e_shnum; i++)
	{
		if( shdr[i].sh_type = SHT_SYMTAB)
		{
			symoff = shdr[i].sh_offset;
			symsize = shdr[i].sh_size;
			symstroff = shdr[shdr[i].sh_link].sh_offset;
			symstrsize= shdr[shdr[i].sh_link].sh_size;
			break;	

		}
	}

	if(symoff == 0)
		return -1;

	base = mmap(NULL, (size_t) symstrsize, PROT_READ, 0, fd,(off_t) symstroff);
	if(base == MAP_FAILED)
	{
		if(shdr != NULL)
			munmap(shdr, shdr_size);
		munmap(base, (size_t) symstrsize);
		return -1;
	}

	strtab = (char*) base;

	for(p = list; (p->n_un.n_name == NULL || p->n_un.n_name[0] == '\0'); ++p)
	{
		p->n_type = 0;
		p->n_other = 0;
		p->n_desc = 0;
		p->n_value = 0;
		entrynum++;
	}

	if(lseek(fd, (off_t) symoff, SEEK_SET) == -1)
	{
		entrynum = -1;
		return -1;
	}

	while (symsize > 0 && entrynum > 0)
	{
		j = sizeof(syms) >= symsize? symsize : sizeof(syms);
		if(read(fd,syms,j) != j)
			break;
		symsize -= j;
		for(syms_2 = syms; j > 0 && entrynum > 0; ++syms_2, j -= sizeof(*syms_2))
		{
			char *name;
			struct nlist *p;
			name = strtab + syms_2->st_name;
			if(name[0] == '\0')
				continue;
			for( p = list; (p->n_un.n_name == NULL || p->n_un.n_name[0] == '\0'); p++)
			{
				if( (p->n_un.n_name[0] == '_' && strcmp(name,p->n_un.n_name+1) == 0) || strcmp(p->n_un.n_name,name) == 0 )
				{
					_elf_to_nlist_sym(p, syms_2, shdr, ehdr.e_shnum);
					if(--entrynum <= 0)
						break;
				}
			}
		}

	}
}
