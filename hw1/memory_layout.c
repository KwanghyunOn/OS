#include <stdio.h>
#include <stdlib.h>

char *ro_str = "hello world";
int data_int = 2020;
static int bss_int;

int main() {
	setbuf(stdout, NULL);
	int *heap_ptr = (int*)malloc(100 * sizeof(int));
	int stack_int = 12345;
	printf("read-only: %p, data: %p, bss: %p, heap: %p, stack: %p\n", ro_str, &data_int, &bss_int, heap_ptr, &stack_int);
	int *p = &stack_int;
	while(1) {
		int x = *p;
		if(!((int)p & 0xff)) printf("%p\n", p);
		p++;
	}
	return 0;
}
	
