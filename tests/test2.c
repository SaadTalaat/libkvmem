#include <stdio.h>
#include <kvmem.h>
#include <stdlib.h>
#include <a.out.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <kvmem_io.h>

int 
main(void)
{
	int readx;
	char *buf;
	int i;
	buf = malloc(0x400);
	struct nlist l[] = {{NULL},{NULL},{NULL},};
	kvmem_t *kd;
	printf("kd %p\n",kd);
	l[1].n_un.n_name = "sys_call_table";
	kd = kvmem_openfiles (NULL, NULL, O_RDWR, 1);
	printf("kd %p\n",kd);
	kvmem_nlist(kd, l);
	printf("syscall table: 0x%lx\n", l[1].n_value);
	readx = kvmem_read(kd,l[1].n_value,buf,0x400);
	printf("Read %x bytes\n",readx);
	printf("Dumping======\n");
	for(i = 0; i < readx; i+=8)
	{
		printf("\n");
		printf("%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x",
			(unsigned char) *(buf+i+7),(unsigned char) *(buf+i+6),
			(unsigned char) *(buf+i+5),(unsigned char) *(buf+i+4),
			(unsigned char) *(buf+i+3),(unsigned char) *(buf+i+2),
			(unsigned char) *(buf+i+1),(unsigned char) *(buf+i));
	}
	printf("\n");
}
