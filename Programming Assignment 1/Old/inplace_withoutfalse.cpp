#include "iostream"
#include "vector"
#include "ctime"
#include "omp.h"

using namespace std;

extern vector<int> calcPrefixSum(vector<int> input, int num_threads);

int main() {
    int num_threads;
    cin >> num_threads;
    int n;
    // cin >> n;
    n=1000000;
    vector<int> input;
    input.resize(n);
    for (int i = 0; i < n; i++) {
        // cin >> input[i];
        input[i]=1;
    }

    vector<int> prefixSum;
    double start_time = omp_get_wtime();
    prefixSum = calcPrefixSum(input, num_threads);
    double time_taken = omp_get_wtime() - start_time;

    // Printing stats and results
    cout<< time_taken << endl;
    cout<< prefixSum.size() << endl;

    // for (int i = 0; i < prefixSum.size(); i++){
    //     cout << prefixSum[i] << " " ;
    // }
    // cout << endl;

    return 0;
}

int input[1000001][8];
int x;
void func_for_me(int jumper)
{
	if(2*jumper>x)
	{
		return;
	}

	int i;
	int twice_jump=2*jumper;
	#pragma omp parallel for
		for(i=twice_jump;i<=x;i+=twice_jump)
			input[i-1][0]=input[i-1][0]+input[i-jumper-1][0];

	// for(int i=0;i<y;i++)
	// 	cout<<input[i][0]<<" ";
	// cout<<endl;
	func_for_me((jumper<<1));
	// for(int i=0;i<y;i++)
	// 	cout<<input[i][0]<<" ";
	// cout<<endl;
	// i=1;
	#pragma omp parallel for
		for(i=3*jumper;i<=x;i+=twice_jump)
		{
			int l=i/jumper;
			if(l&1)
				input[i-1][0]+=input[(l-1)*jumper-1][0];
		}
}

vector<int> calcPrefixSum(vector<int> input1,int num_threads)
{
	omp_set_num_threads(num_threads);
	x=input1.size();
	int i;
	#pragma omp parallel for
		for(i=0;i<x;i++)
			input[i][0]=input1[i];
	func_for_me(1);
	#pragma omp parallel for
		for(i=0;i<x;i++)
			input1[i]=input[i][0];	
	return input1;
}