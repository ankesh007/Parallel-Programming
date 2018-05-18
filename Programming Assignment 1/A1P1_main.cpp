#include "iostream"
#include "vector"
#include "ctime"
#include "omp.h"
#include<bits/stdc++.h>
using namespace std;


extern vector<int> calcPrefixSum(vector<int> input, int num_threads);

int main(int argc,char **argv) {
    int num_threads;
	cin>>num_threads;
    // cin >> num_threads;
    int n;
    // n=200000000;
    // n=atoi(argv[1]);
    // num_threads=atoi(argv[1]);
    cin >> n;
    swap(n,num_threads);
    // n=1000;
    vector<int> input;
    input.resize(n);
    for (int i = 0; i < n; i++) {
        cin >> input[i];
        // input[i]=1;
    }

    vector<int> prefixSum;
    double start_time = omp_get_wtime();
    prefixSum = calcPrefixSum(input, num_threads);
    double time_taken = omp_get_wtime() - start_time;

    // Printing stats and results
    // cout<< time_taken << endl;
    // cout<< prefixSum.size() << endl;

    for (int i = 0; i < prefixSum.size(); i++){
        cout << prefixSum[i] << " " ;
    }
    cout << endl;

    return 0;
}
