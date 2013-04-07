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
#define __USE_LARGEFILE64
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
	off_t page_size;
	page_size = sysconf(_SC_PAGE_SIZE);
	printf("PAGE SIZE %lx\n",page_size);
	printf("Starting.. %s\n",list[1].n_un.n_name);
	if(fd <= 0 )
		return -1;
	if(lseek(fd, (off_t) 0, SEEK_SET) == -1 || read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr) || !_check_elf(&ehdr) )//|| fstat(fd, &st) < 0)
		return -1;

	shdr_size = ehdr.e_shentsize * ehdr.e_shnum;
	lseek(fd,(off_t)0, SEEK_SET);
	base = mmap(NULL, shdr_size*2, PROT_READ, MAP_PRIVATE, fd, (ehdr.e_shoff  & ~(page_size -1)));
	base += ehdr.e_shoff - (ehdr.e_shoff  & ~(page_size -1));
	if(base == MAP_FAILED)
	{
		printf("[Error]: Could not map file.\n");
		goto end;
	}
	
#ifdef KVMEM_32
	shdr = (Elf32_Shdr *) base;
#else
	shdr = (Elf64_Shdr *) base;
#endif
	for(i = 0; i < ehdr.e_shnum; i++)
	{
		if( shdr[i].sh_type == SHT_SYMTAB)
		{
			symoff = shdr[i].sh_offset;
			symsize = shdr[i].sh_size;
			symstroff = shdr[shdr[i].sh_link].sh_offset;
			symstrsize= shdr[shdr[i].sh_link].sh_size;
			break;	

		}
	}

	if(symoff == 0)
		goto end;
	int diff =symstroff - (symstroff & ~(page_size -1));
	base = mmap(NULL, (size_t) symstrsize+diff, PROT_READ, MAP_PRIVATE, fd,(off_t) (symstroff & ~(page_size -1)));
	if(base == MAP_FAILED)
	{
		printf("Could not map Size: %x, Offset %lx\n", symstrsize, (symstroff & ~(page_size -1)));
		if(shdr != NULL)
			munmap(shdr, shdr_size);
		goto end;
	}
	base+=diff;
	printf("%p====\n",base);
	strtab = (char*) base;

	for(p = list; (p->n_un.n_name == NULL || p->n_un.n_name[0] == '\0'); ++p)
	{
		p->n_type = 0;
		p->n_other = 0;
		p->n_desc = 0;
		p->n_value = 0;
		entrynum++;
		printf("Entry %d\n",entrynum);
	}
	printf("Seeking size %lx\n",symstroff);
	if(lseek(fd, (off_t) symoff, SEEK_SET) == -1)
	{
		printf("Fail\n");
		entrynum = -1;
		goto end;
	}

	while (symsize > 0 && entrynum > 0)
	{
		j = sizeof(syms) >= symsize? symsize : sizeof(syms);

		if(read(fd,syms,j) != j)
		{
			printf("Didn't read\n");
			break;
		}

		symsize -= j;
		for(syms_2 = syms; j > 0 && entrynum > 0; ++syms_2, j -= sizeof(*syms_2))
		{
			char *name,c;
			struct nlist *p;
			name = strtab + syms_2->st_name;
			if(name[0] == '\0')
			{
				continue;
			}
			p = list;
			for( p = list+1; p->n_un.n_name != NULL; p++)
			{
				if( (p->n_un.n_name[0] == '_' && 
					strcmp(name,p->n_un.n_name+1) == 0) || 
					strcmp(p->n_un.n_name,name) == 0 )
				{
					printf("MATCH\n");
					_elf_to_nlist_sym(p, syms_2, shdr, ehdr.e_shnum);
					printf("%lx\n",p->n_value);
					if(--entrynum <= 0)
					{
						printf("Breaking\n");
						break;
					}
				}
			}
		}

	}
end:
	if(strtab != NULL)
		munmap(strtab, symstrsize);
	if(shdr != NULL)
		munmap(shdr, symsize);
	return entrynum;
}
