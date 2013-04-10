/*-
 * Copyrights (c) 2013 
 *		Saad Talaat <saadtalaat[at]gmail[dot]com
 */

#ifndef _KVMEM_H_
#define _KVMEM_H_

#include <a.out.h>
 
#define DT_SONAME "libkvmem"
typedef struct _kvmem
{

	int pmfd;	/* physical memory file descriptor /dev/mem */
	int nlfd;	/* linux binary file descriptor /vmlinuz */
	char *ebuf;

	unsigned char syms;
} kvmem_t;

kvmem_t *kvmem_openfiles(const char * kern_binary, const char *mem_dev, unsigned int access, unsigned int verbose);
int kvmem_nlist(kvmem_t *kd, struct nlist *l);
unsigned int kvmem_close(kvmem_t *kd);
char *kvmem_err(kvmem_t*, const char*);
#endif /* _KVMEM_H_ */
