#include <mpi.h>
#include <bits/stdc++.h>
using namespace std;
#define long long long int

int main(int argc,char **argv)
{
	int count=atoi(argv[1]);
	char *basefile=argv[2];

	FILE *inp_open=fopen(strcat(basefile,argv[1]),"rb");

	int n;
	fseek(inp_open,0,SEEK_END);
	long size=ftell(inp_open);

	printf("%lld\n",size);
	fseek(inp_open,0,SEEK_SET);

	fread(&n,4,1,inp_open);
	printf("%d\n",n);
	float temp=0;
	char *temp_val;
	

	while(fread(&temp,4,1,inp_open))
	{
		printf("%f ",temp);
		fread(temp_val,n,1,inp_open);
		printf("%s\n",temp_val);
	}
	MPI_Finalize();
}