/*-
 * Copyrights (c) 2013 
 *              Saad Talaat <saadtalaat[at]gmail[dot]com
 */

#ifndef _KVMEM_ELF_H_
#define _KVMEM_ELF_H_

#include <elf.h>

/** Define current Architecture first **/
#if defined(__i386__)
#define KVMEM_32
#define ELF_TARGET_CLASS ELFCLASS32
#define ELF_TARGET_MACHINE EM_386

#elif defined(__x86_64__)
#define KVMEM_64
#define ELF_TARGET_CLASS ELFCLASS64
#define ELF_TARGET_MACHINE EM_X86_64

#else
#error This Architecture is not supported

#endif

#define ELF_TARGET_DATA ELFDATA2LSB

#define ROUND_DOWN(x,y)\
({\
uint32_t z = (uint32_t)(x); \
(typeof(x)) (z - z % (y)); \
})

#endif
