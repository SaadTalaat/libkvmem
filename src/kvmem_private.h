/*-
 * Copyrights (c) 2013 
 *              Saad Talaat <saadtalaat[at]gmail[dot]com
 */

#ifndef _KVMEM_PRIVATE_H_
#define _KVMEM_PRIVATE_H_
#include <kvmem.h>

kvmem_t* _kvmem_open(kvmem_t* kd, const char* kern_binary, const char* mem_dev, unsigned int access, unsigned int verbose);
int _elf_fdnlist(int fd, struct nlist* list);



#endif /* _KVMEM_PRIVATE_H_ */
