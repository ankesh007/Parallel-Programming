#include <bits/stdc++.h>

using namespace std;

int main(int argc,char**argv)
{
	srand(time(NULL));
	cout<<"Hello\n";
	cout<<"World\n";


	int n,m;
	int thresh=95;
	n=atoi(argv[1]);
	m=70;
	cout<<5<<"\n";
	cout<<n<<" "<<m<<"\n";

	for(int i=0;i<n;i++)
	{
		for(int j=0;j<m;j++)
		{
			int k=rand()%100;
			if(k>thresh)
				cout<<1<<" ";
			else
				cout<<0<<" ";
		}
		cout<<"\n";
	}
}