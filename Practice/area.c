#include <omp.h>
#include<stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	// double step_size=atoi(argv[1]);
	double step_size=100000000;
	double width=1/step_size;

	double sum=0;
	int k=atoi(argv[1]);

	omp_set_num_threads(k);
	int len_per_thread=step_size/k;

	double ss=omp_get_wtime();

	#pragma omp parallel
	{
		// printf("HI\n");
		double loc=0;
		int tid=omp_get_thread_num();
		int start=len_per_thread*tid;
		int end=(tid+1)*len_per_thread;
		for(int i=start;i<end;i++)
		{
			double x=(i+1)*width;
			loc+=(4/(1+x*x))*width;
		}
		sum+=loc;
	}
	ss=omp_get_wtime()-ss;
	printf("%lf %lf\n",sum,ss);
	return 0;
}