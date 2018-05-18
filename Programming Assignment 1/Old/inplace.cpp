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
    n=100;
    vector<int> input;
    input.resize(n);
    for (int i = 0; i < n; i++) {
        // cin >> input[i];
        input[i]=i+1;
    }
    cout<<"Hi"<<endl;

    vector<int> prefixSum;
    double start_time = omp_get_wtime();
    prefixSum = calcPrefixSum(input, num_threads);
    double time_taken = omp_get_wtime() - start_time;

    // Printing stats and results
    cout<< time_taken << endl;
    cout<< prefixSum.size() << endl;

    for (int i = 0; i < prefixSum.size(); i++){
        cout << prefixSum[i] << " " ;
    }
    cout << endl;

    return 0;
}

void func_for_me(vector<int> &input,int jumper)
{
	int x=input.size();

	if(2*jumper>x)
	{
		return;
	}

	int i;
	int twice_jump=2*jumper;
	#pragma omp parallel for
		for(i=twice_jump;i<=x;i+=twice_jump)
			input[i-1]=input[i-1]+input[i-jumper-1];

	func_for_me(input,(jumper<<1));
	#pragma omp parallel for
		for(i=3*jumper;i<=x;i+=twice_jump)
		{
			int l=i/jumper;
			if(l&1)
				input[i-1]+=input[(l-1)*jumper-1];
		}
}

vector<int> calcPrefixSum(vector<int> input,int num_threads)
{
	omp_set_num_threads(num_threads);
	func_for_me(input,1);
	return input;
}