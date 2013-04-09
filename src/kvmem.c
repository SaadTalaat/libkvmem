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
#include <a.out.h>

#include "kvmem_private.h"


/**
 * description: a kvmem_openfiles helper
 *
 */
char *
kvmem_err(kvmem_t *kd, const char* error)
{
	int len;
	char *err = "[Error]: kvmem_err: Internal error.\n";
	if(kd == NULL)
		return NULL;
	if(error == NULL)
		return kd->ebuf;
	if(kd->ebuf == NULL)
	{
		len = strlen(error);
		if( (kd->ebuf = (char *) malloc(len+2)) == NULL)
			return err;
		memcpy(kd->ebuf,error, len);
		kd->ebuf[len]='\n';
		kd->ebuf[len+1] = '\0';
	}
	else
	{
		free(kd->ebuf);
		len = strlen(error);
		if( (kd->ebuf = (char *) malloc(len+2)) == NULL)
			return err;
		memcpy(kd->ebuf, error, len);
		kd->ebuf[len] = '\n';
		kd->ebuf[len+1] = '\0';
	}

};


kvmem_t*
_kvmem_open(kvmem_t *kd, const char* kern_binary, const char* mem_dev, unsigned int access, unsigned int verbose)
{

	struct stat st;
	if(kd == NULL)
		return NULL;
	if( kern_binary == NULL)
		kern_binary = "/home/saad/vmlinux";
	else if ( strlen(kern_binary) >= PATH_MAX)
	{
		kvmem_err(kd, "[Error]: kernel binary path is too big");
		return NULL;
	}

	if( access & ~O_RDWR)
	{
		kvmem_err(kd, "[Error]: access flags.");
		return NULL;
	}
	if( mem_dev == NULL)
		mem_dev = "/dev/mem";
	else if ( strlen(mem_dev) >= PATH_MAX)
	{
		kvmem_err(kd, "[Error]: /dev/mem path too big");
		return NULL;
	}

	if(( kd->pmfd = open(mem_dev,access, 0)) < 0)
	{
		kvmem_err(kd, "[Error]: superuser privilege required");
		return NULL;
	}

	if(fcntl(kd->pmfd, F_SETFD, FD_CLOEXEC) < 0){
		kvmem_err(kd,"[Error]: could not control file");
		return NULL;
	}
	
	if( (kd->nlfd = open(kern_binary, O_RDONLY, 0))	< 0)
	{
		kvmem_err(kd, "[Error]: superuser privilege required");
		return NULL;
	}
	if(fcntl(kd->nlfd, F_SETFD, FD_CLOEXEC) < 0)
	{
		kvmem_err(kd,"[Error]: could not control file");
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
			kvmem_err(kd,"[Error]: cannot allocate memory.");
		return NULL;
	}
	kd->ebuf = malloc(1024);	
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

static int
kvmem_fndlist_prefix(kvmem_t *kd, struct nlist *nl, int missing, const char *prefix)
{
	struct nlist *n,*np,*p;
	char *cp,*ce;
	const char *ccp;
	size_t len;
	int slen, unresolved;



	len = 0;
	unresolved = 0;

	for(p = nl; p->n_un.n_name && p->n_un.n_name[0]; ++p)
	{
		if(p->n_type != N_UNDF)
			continue;
		len += sizeof(struct nlist) + strlen(prefix) + 2*(strlen(p->n_un.n_name +1));
		unresolved ++;
	}
	if(unresolved == 0)
		return 0;
	len += sizeof(struct nlist);
	unresolved++;

	n = np = malloc(len);
	memset(n,0,len);
	if(n == NULL)
	{
		kvmem_err(kd,"[Error]: could not allocate memory");
		return 0;
	}
	cp = ce = (char *) np;
	cp += unresolved * sizeof(struct nlist);
	ce += len;

	unresolved = 0;
	for(p = nl;p->n_un.n_name && p->n_un.n_name[0]; ++p)
	{
		if(p->n_type != N_UNDF)
			continue;
		memcpy(np,p,sizeof(struct nlist));
		slen = snprintf(cp, ce-cp, "%s%s%c%s",prefix,(prefix[0] != '\0' && p->n_un.n_name[0] == '_'? (p->n_un.n_name+1):p->n_un.n_name),'\0',p->n_un.n_name);
		if( slen < 0 || slen >= ce-cp)
			continue;
		np->n_un.n_name = cp;
		cp += slen +1;
		np++;
		unresolved ++;
	}
	np = n;
	unresolved = _elf_fdnlist(kd->nlfd, np);
	return unresolved;
}

int
_kvmem_nlist(kvmem_t *kd, struct nlist *nl, int init)
{
	struct nlist *p;
	int invalid, error;
	char symname[1024];
	const char *prefix = "";
	if(kd == NULL || nl == NULL)
	{
		kvmem_err(kd,"[Error]: invalid arguments");
		return -1;
	}
	if(!(kd->pmfd >= 0) )
	{
		// to be implemented
		error = _elf_fdnlist(kd->nlfd, nl);
		if(error <= 0)
			return error;
		
	}
	error = _elf_fdnlist(kd->nlfd, nl);
	for(p= nl; p->n_un.n_name && p->n_un.n_name[0]; ++p)
	{
		if(p->n_type != N_UNDF)
			continue;
		error = snprintf(symname, sizeof(symname), "%s%s", prefix, (prefix[0] != '\0' && p->n_un.n_name[0] == '_')? (p->n_un.n_name +1 ): p->n_un.n_name);
		if(error < 0 || error >= (int) sizeof(symname))
			continue;
		// Meh, probably will right elf parser first..
		p->n_un.n_name = symname;
	}
	return p - nl;
}

int 
kvmem_nlist(kvmem_t *kd, struct nlist *nl)
{
	return _kvmem_nlist(kd,nl,0);
}
