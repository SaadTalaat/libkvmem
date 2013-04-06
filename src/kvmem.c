/*-
 * Copyrights (c) 2013 
 *              Saad Talaat <saadtalaat[at]gmail[dot]com
 */
#include <stdio.h>
#include <stdlib.h>
#include <kvmem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <unistd.h>
#include <string.h>

#include "kvmem_private.h"

/**
 * description: a kvmem_openfiles helper
 *
 */
kvmem_t*
_kvmem_open(kvmem_t *kd, const char* kern_binary, const char* mem_dev, unsigned int access, unsigned int verbose)
{

	struct stat st;

	if( kern_binary == NULL)
		kern_binary = "/vmlinuz";
	else if ( strlen(kern_binary) >= PATH_MAX)
		return NULL;

	if( access & ~O_RDWR)
		return NULL;

	if( mem_dev == NULL)
		mem_dev = "/dev/mem";
	else if ( strlen(mem_dev) >= PATH_MAX)
		return NULL;
	
	if(( kd->pmfd = open(mem_dev,access, 0)) < 0)
	{
		fprintf(stderr,"[error]: This file must have root privilege.\n");
		return NULL;
	}

	if( fstat(kd->pmfd,&st) < 0)
	{
		return NULL;
	}
	if(S_ISREG(st.st_mode) && st.st_size <= 0)
	{
		return NULL;	
	}

	if(fnctl(kd->pmfd, F_SETFD, FD_CLOEXEC) < 0){
		return NULL;
	}
	
	if( (kd->nlfd = open(kern_binary, O_RDONLY, 0))	< 0)
	{
		fprintf(stderr,"[error]: This file must have root privilege.\n");
		return NULL;
	}
	if(fnctl(kd->nlfd, F_SETFD, FD_CLOEXEC) < 0){
		return NULL;
	}
	// finally return kd!
	return kd;
	
}


/**
 * description: opens kernel binary and mem port
 * return: success, opened kvmem_t*. fail, returns null
 */
kvmem_t*
kvmem_openfiles(const char* kern_binary, const char* mem_dev, unsigned int access, unsigned int verbose)
{
	kvmem_t *kd;
	if( (kd = (kvmem_t *) malloc(sizeof(kvmem_t))) == NULL)
	{
		if(verbose != 0)
			fprintf(stderr,"[error]: cannot allocate memory.\n");
		return NULL;
	}
	return _kvmem_open(kd, kern_binary, mem_dev, access, 0);
}

unsigned int
kvmem_close(kvmem_t *kd)
{
	unsigned int error = 0;
	if( kd->pmfd)
		error |= close(kd->pmfd);
	if( kd->nlfd)
		error |= close(kd->nlfd);
	return error;
}
