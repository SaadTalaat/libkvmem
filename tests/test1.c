#include <stdio.h>
#include <kvmem.h>
#include <stdlib.h>
#include <a.out.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int 
main(void)
{
	struct nlist l[] = {{NULL},{NULL},{NULL},};
	kvmem_t *kd;
	l[1].n_un.n_name = "sys_call_table";
	kd = kvmem_openfiles (NULL, NULL, O_RDWR, 1);
	printf("KD2 %p\n",kd);
	kvmem_nlist(kd, l);
	printf("%lx\n", l[1].n_value);


}