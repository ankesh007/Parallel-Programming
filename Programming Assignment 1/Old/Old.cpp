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
    n=10000000;
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

    for (int i = 0; i < prefixSum.size(); i++){
        cout << prefixSum[i] << " " ;
    }
    cout << endl;

    return 0;
}

vector<int> func_for_me(vector<int> input)
{
	int x=input.size();
	// cout<<x<<endl;
	int xx=x;
	if(xx&1)
		xx--;

	if(x<=2)
	{
		if(x==2)
			input[1]+=input[0];
		return input;
	}

	vector<int> temp;
	int y=x>>1;
	temp.resize(y);


	int i;
	#pragma omp parallel for
		for(i=0;i<xx;i+=2)
			temp[i>>1]=input[i]+input[i+1];

	// for(int i=0;i<y;i++)
	// 	cout<<temp[i]<<" ";
	// cout<<endl;
	temp=func_for_me(temp);
	// for(int i=0;i<y;i++)
	// 	cout<<temp[i]<<" ";
	// cout<<endl;
	i=1;
	#pragma omp parallel for
		for(i=1;i<x;i++)
		{
			if(i&1)
				input[i]=temp[i>>1];
			else
				input[i]+=temp[(i-1)>>1];
		}
	return input;
}

vector<int> calcPrefixSum(vector<int> input,int num_threads)
{
	omp_set_num_threads(num_threads);
	return func_for_me(input);
}