#include <math.h>
#include <stdio.h>

int main()
{
	float f=INFINITY;
	float g=5;
	printf("%d",(int)(f==INFINITY));
	printf("%d",(int)isinf(f));
	printf("%d",(int)isinf(g));
}
