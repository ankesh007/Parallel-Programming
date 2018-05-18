#include "iostream"
#include "vector"
#include "ctime"
#include "omp.h"

using namespace std;

const int N=2e5;
int auxillary[N];

void func_for_me(int x,int num_threads)
{
	int i;
	int jumper;
	for(jumper=1;(jumper<<1)<=x;(jumper<<=1))
	{
		num_threads>>=1;
		omp_set_num_threads(num_threads);
		int twice_jump=2*jumper;
		#pragma omp parallel for
			for(i=twice_jump;i<=x;i+=twice_jump)
				auxillary[i]=auxillary[i]+auxillary[i-jumper];
	}
	jumper=(jumper>>1);
	num_threads=1;
	for(;jumper>=1;(jumper>>=1))
	{
		omp_set_num_threads(num_threads);
		num_threads<<=1;
		int twice_jump=2*jumper;
		#pragma omp parallel for
			for(i=3*jumper;i<=x;i+=twice_jump)
			{
				int l=i/jumper;
				if(l&1)
					auxillary[i]+=auxillary[(l-1)*jumper];
			}
	}
}

void func_for_me2(int n)
{
	for(int i=1;i<=n;i++)
		auxillary[i]+=auxillary[i-1];
}


vector<int> calcPrefixSum(vector<int> input, int num_threads)
{
	int n=input.size();
	if(num_threads>n)
		num_threads=n;

	omp_set_num_threads(num_threads);
	int per_thread=(n+num_threads-1)/num_threads;

	#pragma omp parallel 
	{
		int tid=omp_get_thread_num();
		int s=tid*per_thread;
		int e=min(s+per_thread,n);

		for(int i=s+1;i<e;i++)
			input[i]=input[i-1]+input[i];
		auxillary[tid+1]=input[e-1];
	}

	func_for_me(num_threads,num_threads);
	omp_set_num_threads(num_threads);
	#pragma omp parallel 
	{
		int tid=omp_get_thread_num();
		if(tid!=0)
		{
			int s=tid*per_thread;
			int e=min(s+per_thread,n);
			int val=auxillary[tid];
			for(int i=s;i<e;i++)
			{
				input[i]+=val;
			}
		}
	}
	return input;
}
