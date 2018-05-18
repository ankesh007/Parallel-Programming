#include <bits/stdc++.h>
#include <omp.h>
#define x first
#define y second

typedef pair<int,int> iPair;

using namespace std;

set<iPair> convex_hull;

int evalLine(iPair p1,iPair p2,iPair p)
{
	return ((p.y - p1.y) * (p2.x - p1.x) -
              (p2.y - p1.y) * (p.x - p1.x));
}

int distLine(iPair p1,iPair p2,iPair p)
{
	return abs(evalLine(p1,p2,p));
}

int is_right(iPair p1,iPair p2,iPair p)
{
	return (evalLine(p1,p2,p)>0);
}

int getSide(iPair p1,iPair p2,iPair p)
{
	int temp=evalLine(p1,p2,p);
	if(temp<0)
		return -1;
	else if(temp>0)
		return 1;
	else
		return 0;
}

int square_dist(iPair p,iPair q)
{
	return (p.y-q.y)*(p.y-q.y)+(p.x-q.x)*(p.x-q.x);
}

void quickHull(vector<iPair> &points,iPair p1,iPair p2)
{
	int index=-1;
	int max_dis=0;
	int i;
	int n=points.size();

	// #pragma omp parallel for
	for(i=0;i<n;i++)
	{
		int dis=distLine(p1,p2,points[i]);

		if(dis>max_dis)
		{
			dis=max_dis;
			index=i;
		}
	}

	if(index!=-1)
	{
		#pragma omp critical
		{
			convex_hull.insert(points[index]);
		}
		return;
	}

	vector<iPair> rig1,rig2;

	for(int i=0;i<n;i++)
	{
		if(is_right(p1,points[index],points[i]))
			rig1.pb(points[i]);
		if(is_right(points[index],p2,points[i]))
			rig2.pb(points[i]);
	}

	#pragma omp parallel
	{
		#pragma omp single nowait
			quickHull(rig1,p1,points[index]);
		#pragma omp single nowait
			quickHull(rig2,points[index],p2);
	}
}

void solveHull(vector<iPair> &points)
{
	int n=points.size();

	int maxx_ind=-1,minx_ind=-1;
	int maxx_val=-1,minx_val=1e9;
	int i;

	#pragma omp parallel for
	{
		int maxx_ind_l=-1,minx_ind_l=-1;
		int maxx_val_l=-1,minx_val_l=1e9;
		for(i=0;i<n;i++)
		{
			if(points[i].x>maxx_val_l)
			{
				maxx_ind_l=i;
				maxx_val_l=points[i].x;
			}
			if(points[i].x<minx_val_l)
			{
				minx_val_l=i;
				minx_val_l=points[i].x;
			}
		}
		#pragma omp critical
		{
			if(maxx_val_l>maxx_val)
			{
				maxx_val=maxx_val_l;
				maxx_ind=maxx_ind_l;
			}
			if(minx_val_l<minx_val)
			{
				minx_val=minx_val_l;
				minx_ind=minx_ind_l;
			}
		}
	}
	convex_hull.insert(points[minx_ind]);
	convex_hull.insert(points[maxx_ind]);

	vector<iPair> rig1,rig2;

	for(int i=0;i<n;i++)
	{
		int eval=evalLine(points[minx_ind],points[maxx_ind],points[i]);
		if(eval>0)
			rig1.pb(points[i]);
		else if(eval<0)
			rig2.pb(points[i]);
	}

	#pragma omp parallel
	{
		#pragma omp single
			quickHull(rig1,points[minx_ind],points[maxx_ind]);
		#pragma omp single
			quickHull(rig2,points[maxx_ind],points[minx_ind]);
	}
}




