#include "iostream"
#include "vector"
#include "ctime"
#include "omp.h"
#include<bits/stdc++.h>
using namespace std;

extern vector< pair<int, int> > calcConvexHull(vector< vector<int> > image, int num_threads);

int main(int argc,char **argv) {
    string file_headers;
    getline(cin, file_headers);
    getline(cin, file_headers);

    int num_threads;
    cin >> num_threads;
    num_threads=atoi(argv[1]);
    int m, n;
    cin >> m >> n;
    vector< vector<int> > image;
    image.resize(m);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            int a;
            cin >> a;
            image[i].push_back(a);
        }
    }

    vector< pair<int, int> > convexHull;
    double start_time = omp_get_wtime();
    convexHull = calcConvexHull(image, num_threads);
    double time_taken = omp_get_wtime() - start_time;

    // Printing stats and results
    cout<< time_taken << endl;
    cout<< convexHull.size() << endl;

    for (int i = 0; i < convexHull.size(); i++){
        cout << convexHull[i].first << " " << convexHull[i].second << endl;
    }

    return 0;
}