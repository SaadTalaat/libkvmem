/*-
 * Copyrights (c) 2013 
 *		Saad Talaat <saadtalaat[at]gmail[dot]com
 */

#ifndef _KVMEM_IO_H_
#define _KVMEM_IO_H_

ssize_t kvmem_read(
kvmem_t *kd,
#ifdef KVMEM_32
unsigned long int vaddr,
#else
unsigned int vaddr,
#endif
void *buf,
size_t len);
 #endif /* _KVMEM_IO_H_ */