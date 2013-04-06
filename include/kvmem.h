/*-
 * Copyrights (c) 2013 
 *		Saad Talaat <saadtalaat[at]gmail[dot]com
 */

#ifndef _KVMEM_H_
#define _KVMEM_H_

#define DT_SONAME "libkvmem"
typedef struct _kvmem
{

	int pmfd;	/* physical memory file descriptor /dev/mem */
	int nlfd;	/* linux binary file descriptor /vmlinuz */

} kvmem_t;
kvmem_t *kvmem_openfiles(const char * kern_binary, const char *mem_dev, unsigned int access, unsigned int verbose);
unsigned int kvmem_close(kvmem_t *kd);

#endif /* _KVMEM_H_ */