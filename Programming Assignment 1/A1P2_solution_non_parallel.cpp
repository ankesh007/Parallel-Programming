#include "omp.h"
#include <bits/stdc++.h>
#define x first
#define y second
#define pb push_back
#define mp make_pair

using namespace std;
typedef pair<int,int> iPair;

vector<iPair> convex_hull;
int currently_available;

int evalLine(iPair p1,iPair p2,iPair p)
{
	return ((p.y - p1.y) * (p2.x - p1.x) -(p2.y - p1.y) * (p.x - p1.x));
}

int distLine(iPair p1,iPair p2,iPair p)
{
	return abs(evalLine(p1,p2,p));
}

bool is_right(iPair p1,iPair p2,iPair p)
{
	return (evalLine(p1,p2,p)<0);
}

void printPair(iPair a)
{
	cout<<a.x<<" "<<a.y<<endl;
}

int request_threads()
{
	int val=0;
	#pragma omp critical
	{
		if(currently_available)
		{
			currently_available--;
			val=1;
		}
	}
	return val;
}

void free_threads(int threads)
{
	#pragma omp critical
	{
		currently_available+=threads;
	}
}

void quickHull(vector<iPair> &points,iPair p1,iPair p2)
{
	int index=-1;
	int max_dis=0;
	int i;
	int n=points.size();

	for(i=0;i<n;i++)
	{
		int dis=distLine(p1,p2,points[i]);

		if(dis>max_dis)
		{
			max_dis=dis;
			index=i;
		}
	}

	if(index==-1)
	{
		return;
	}
	#pragma omp critical
	{
		convex_hull.pb(points[index]);
	}

	vector<iPair> rig1,rig2;


	for(int i=0;i<n;i++)
	{
		if(is_right(p1,points[index],points[i]))
			rig1.pb(points[i]);
		else if(is_right(points[index],p2,points[i]))
			rig2.pb(points[i]);
	}

	int received_threads=request_threads();	
	// cout<<received_threads<<" "<<currently_available<<"hey"<<endl;

	#pragma omp parallel num_threads(received_threads+1)
	{
		#pragma omp single nowait
			quickHull(rig1,p1,points[index]);
		#pragma omp single nowait
			quickHull(rig2,points[index],p2);
	}
	if(received_threads)
		free_threads(received_threads);
}

void solveHull(vector<iPair> &points,int num_threads)
{
	int n=points.size();

	int maxx_ind=-1,minx_ind=-1;
	int maxx_val=-1,minx_val=1e9;
	// int per_thread=(n+num_threads-1)/num_threads;

	// #pragma omp parallel num_threads(num_threads)
	// {
		// int tid=omp_get_thread_num();
		// int nn=omp_get_num_threads();
		// per_thread=(n+nn-1)/nn;
		int i;
		int maxx_ind_l=-1,minx_ind_l=-1;
		int maxx_val_l=-1,minx_val_l=1e9;
		// int s=tid*per_thread;int e=min(n,s+per_thread);
		int s=0,e=n;
		for(i=s;i<e;i++)
		{
			if(points[i].x>maxx_val_l)
			{
				maxx_ind_l=i;
				maxx_val_l=points[i].x;
			}
			if(points[i].x<minx_val_l)
			{
				minx_ind_l=i;
				minx_val_l=points[i].x;
			}
		}

	// 	#pragma omp critical
	// 	{
	// 		if(maxx_val_l>maxx_val)
	// 		{
	// 			maxx_val=maxx_val_l;
	// 			maxx_ind=maxx_ind_l;
	// 		}
	// 		if(minx_val_l<minx_val)
	// 		{
	// 			minx_val=minx_val_l;
	// 			minx_ind=minx_ind_l;
	// 		}
	// 	}
	// }

	minx_ind=minx_ind_l;
	maxx_ind=maxx_ind_l;

	convex_hull.pb(points[minx_ind]);
	convex_hull.pb(points[maxx_ind]);

	vector<iPair> rig1,rig2;

	for(int i=0;i<n;i++)
	{
		int eval=evalLine(points[minx_ind],points[maxx_ind],points[i]);
		if(eval<0)
			rig1.pb(points[i]);
		else if(eval>0)
			rig2.pb(points[i]);
	}

	int received_threads=request_threads();
	// cout<<received_threads<<"hi"<<endl;

	#pragma omp parallel num_threads(received_threads+1)
	{
		// int tid=omp_get_thread_num();
		// cout<<tid<<"tiduo"<<endl;
		#pragma omp single nowait
			quickHull(rig1,points[minx_ind],points[maxx_ind]);
		// cout<<tid<<"tid"<<endl;
		#pragma omp single nowait
			quickHull(rig2,points[maxx_ind],points[minx_ind]);
	}
	free_threads(received_threads);
}

vector< pair<int, int> > calcConvexHull(vector< vector<int> > image, int num_threads) {

	omp_set_nested(1);
	// cout<<num_threads<<endl;
	vector<pair<int,int> > points;
	currently_available=num_threads-1;

	int x=image.size();
	int y=image[0].size();
	int i,j;
	// # pragma omp parallel for private(j) num_threads(num_threads)
	for(i=0;i<x;i++)
	{
		for(j=0;j<y;j++)
		{
			if(image[i][j]==1)
			{
				// #pragma omp critical
				points.pb(mp(i,j));
			}
			// cout<<image[i][j]<<" ";
		}
		// cout<<endl;
	}
	// cout<<endl;
	solveHull(points,num_threads);
	// for(int i=0;i<x;i++)
	// {
	// 	for(int j=0;j<y;j++)
	// 	{
	// 		if(convex_hull.find(mp(i,j))!=convex_hull.end())
	// 			cout<<1<<" ";
	// 		else
	// 			cout<<0<<" ";
	// 	}
	// 	cout<<endl;
	// }
	// cout<<endl;

	// vector<pair<int,int> > convexHull(convex_hull.begin(),convex_hull.end());
	return convex_hull;
}