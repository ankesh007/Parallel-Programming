#include <omp.h>
#include <bits/stdc++.h>
int main(){
	omp_set_num_threads(100);
	omp_set_nested(1); 
	#pragma omp parallel
	{
		int total_outer = omp_get_num_threads();
		int outer_tid = omp_get_thread_num();
		omp_set_num_threads(4);
		#pragma omp parallel
		{
			int total = omp_get_num_threads();
			int tid = omp_get_thread_num();
			printf("Outer: %d of %d, Inner: %d of %d\n",outer_tid,total_outer,tid,total);
		}
	}
	return 0;
}