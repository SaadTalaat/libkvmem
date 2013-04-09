/*-
 * Copyrights (c) 2013 
 *		Saad Talaat <saadtalaat[at]gmail[dot]com
 */

 #include <stdlib.h>
 #include <stdio.h>
 #include <kvmem.h>
 #include <kvmem_elf.h>
 #include <kvmem_io.h>
 #include "kvmem_private.h"


/** Shit this function looks ugly.. **/
#ifdef KVMEM_32
 unsigned int
#else
 unsigned long int
#endif
kvmem_vatop(vaddr)
#ifdef KVMEM_32
unsigned int vaddr;
#else
unsigned long int vaddr;
#endif
{

#ifdef KVMEM_32
	return (vaddr - 0xc0000000);
#else
	printf("VADDR: %lx\n",vaddr);
	return (vaddr & ~0xffffffff80000000);
#endif
}


ssize_t
kvmem_read(kd,vaddr, buf,len)
kvmem_t *kd;
#ifdef KVMEM_32
unsigned long int vaddr;
#else
unsigned int vaddr;
#endif
void *buf;
size_t len;
 {

#ifdef KVMEM_32
 	unsigned int paddr;
#else
 	unsigned long int paddr;
#endif

 	ssize_t readr,readn;
 	void *buf2;
 	readn = len;
 	buf2 = buf;
 	if(kd == NULL || buf == NULL || len == 0)
 	{
		kvmem_err(kd, "[Error]: non-valid arguments");
 		return -1;
 	}
 	else if(kd->pmfd <= 0)
 	{
		kvmem_err(kd, "[Error]: cannot access /dev/mem");
 		return -1;
 	}
	while(len > 0)
	{
	 	paddr = kvmem_vatop(vaddr);
 		if(lseek(kd->pmfd, (off_t) paddr, SEEK_SET) == -1)
 		{
			kvmem_err(kd,"[Error]: cannot access /dev/mem");
 			return -1;
 		}
 		readr = read(kd->pmfd, buf2,len);
 
 		if(readr < 0)
 		{
			kvmem_err(kd,"[Error]: cannot access /dev/mem");
			return -1;
 		}
 	
 		if(readr == 0)
	 		break;
	 	buf2 += readr;
	 	vaddr += readr;
	 	len -= readr;
	}
	return (buf2 - buf);
 }


ssize_t
kvmem_write(kvmem_t *kd,
#ifdef KVMEM_32
unsigned int va,
#else
unsigned long int va,
#endif
const void *buf, size_t len)
{

	unsigned int paddr;
	size_t written;

	if(buf == NULL || len == 0 || va == 0 || kd == NULL)
	{
		kvmem_err(kd, "[Error]: non-valid arguments");
		return -1;
	}
	else if(kd->pmfd <= 0)
	{
		kvmem_err(kd, "[Error]: cannot access /dev/mem");
		return -1;
	}
	paddr = kvmem_vatop(va);
	
	if(lseek(kd->pmfd, paddr, SEEK_SET) == -1){
		kvmem_err(kd, "[Error]: cannot access /dev/mem");
		return -1;
	}
	
	written = write(kd->pmfd, buf, len);
	return written;

}
