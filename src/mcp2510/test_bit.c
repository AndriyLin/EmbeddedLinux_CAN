#include <stdio.h>

/**
用来测试板子是高位还是低位，结果：低位 by @Lorenzo.di.*****
*/
int main() 
{
	union c {
		int i;
		char j;
	};

	union c d;
	d.i = 0;
	d.j = 1;

	printf("%d", d.i);
	return 0;
}
