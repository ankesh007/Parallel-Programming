#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#define long long long int

int max(int i,int j)
{
	if(i>j)
		return i;
	return j;
}

void printError()
{
	printf("Exiting\n");
	exit(0);
}

int taskid,numprocs;
char *basefile;
int files_per_proc;
int col_start=0,col_end=0;
char *data,*recv_data,*temp_sort_row,*temp_sort_col;

int TOTAL_COLUMN;
int MEM_ROW=0;
int MEM_COL=0;
int COLUMNS;
char filename[500];
int row_for_each_process;
int AUGMENTED_TOTAL_COLUMN;
int TO_MASTER=0;
int FROM_MASTER=1;

int getIndex(int i,int j,int k)
{
	return i*(MEM_COL*COLUMNS)+j*MEM_COL+k;
}

int cmpfunc(const void *a,const void *b)
{
	char *c=(char *)a;
	char *d=(char *)b;
	float *e=c;
	float *f=d;
	if((*e)<(*f))
		return -1;
	else if((*e)>(*f))
		return 1;
	else return 0;
}

bool sorted()
{
	int sorted=1;
	int i,j,k;

	for(i=0;i<COLUMNS && sorted==1;i++)
	{
		// printf("%d\n",i);
		int pointer=0;
		for(j=0;j<MEM_ROW;j++)
		{
			for(k=0;k<MEM_COL;k++)
			{
				temp_sort_col[pointer++]=data[getIndex(j,i,k)];
			}
		}
		int temp=MEM_COL;
		for(j=1;j<MEM_ROW;j++)
		{
			float *f=&(temp_sort_col[temp]);
			float *g=&(temp_sort_col[temp-MEM_COL]);
			if(*f<*g)
			{
				sorted=0;
				break;
			}
			temp+=MEM_COL;
		}
	}

	// printf("Here********\n" );
	int global_sorted=1;

	MPI_Reduce(&sorted,&global_sorted,1,MPI_INT,MPI_MIN,0,MPI_COMM_WORLD);
	if(taskid==0)
		MPI_Bcast(&global_sorted,1,MPI_INT,0,MPI_COMM_WORLD);

	if(taskid!=0)
		MPI_Bcast(&global_sorted,1,MPI_INT,0,MPI_COMM_WORLD);

	// printf("%dGLobal\n",global_sorted );
	// printf("There2********\n" );

	// return global_sorted;
	if(global_sorted==1)
		return true;
	else
		return false;
}

int main(int argc,char **argv)
{
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);

	TOTAL_COLUMN=atoi(argv[1]);
	basefile=argv[2];
	files_per_proc=(TOTAL_COLUMN+numprocs-1)/numprocs;
	AUGMENTED_TOTAL_COLUMN=files_per_proc*numprocs;
	// printf("%dFiles\n",files_per_proc );
	col_start=0;
	col_end=0;

	int max_n=0;
	int max_r=0;

	int col_start=taskid*files_per_proc;
	int col_end=(taskid+1)*files_per_proc;
	col_start++;
	col_end++;
	// To check Division of Columns
	// printf("%d %d\n",col_start,col_end);
	COLUMNS=col_end-col_start;
	int i,j,k;

	for(i=col_start;i<col_end;i++)
	{
		if(i>TOTAL_COLUMN)
			continue;
		sprintf(filename,"%s%d",basefile,i);
		FILE *inp_open=fopen(filename,"rb");
		int n;
		int rem_warning=fread(&n,4,1,inp_open);
		fseek(inp_open,0,SEEK_END);
		int size=ftell(inp_open);
		size-=4;
		size/=(4+n);
		max_n=max(max_n,n);
		max_r=max(max_r,size);
		fclose(inp_open);
	}

	MPI_Bcast(&max_n,1,MPI_INT,taskid,MPI_COMM_WORLD);

	for(i=0;i<numprocs;i++)
	{
		if(i==taskid)
			continue;
		int temp=0;
		MPI_Bcast(&temp,1,MPI_INT,i,MPI_COMM_WORLD);
		max_n=max(max_n,temp);
	}

	MPI_Bcast(&max_r,1,MPI_INT,taskid,MPI_COMM_WORLD);

	for(i=0;i<numprocs;i++)
	{
		if(i==taskid)
			continue;
		int temp=0;
		MPI_Bcast(&temp,1,MPI_INT,i,MPI_COMM_WORLD);
		max_r=max(max_r,temp);
	}

	// For Debugging Broadcast 
	// printf("%dmax_n\n",max_n);
	// printf("%dmax_r\n",max_r);

	MEM_COL=max_n+5;
	MEM_ROW=((max_r+numprocs-1)/numprocs)*numprocs;
	row_for_each_process=MEM_ROW/numprocs;

	int total=MEM_COL*MEM_ROW*COLUMNS;
	data=(char *)calloc(MEM_COL*MEM_ROW*COLUMNS,sizeof(char));
	recv_data=(char *)calloc(MEM_COL*MEM_ROW*COLUMNS,sizeof(char));
	temp_sort_row=(char *)calloc(MEM_COL*AUGMENTED_TOTAL_COLUMN,sizeof(char));
	temp_sort_col=(char *)calloc(MEM_COL*MEM_ROW,sizeof(char));
	int pointer=0;

	// printf("%d\n",total);

	// float f=INFINITY;
	// inf=&f;
	for(i=0;i<MEM_ROW;i++)
	{
		for(j=0;j<COLUMNS;j++)
		{
			float *f=&(data[getIndex(i,j,0)]);
			*f=INFINITY;
		}
	}

	for(i=0;i<COLUMNS;i++)
	{
		if(i+col_start>TOTAL_COLUMN)
			continue;

		// printf("%d Hi Opening\n",i+col_start);
		sprintf(filename,"%s%d",basefile,i+col_start);
		FILE *inp_open=fopen(filename,"rb");
		int n;
		int rem_warning=fread(&n,4,1,inp_open);

		if(rem_warning==0)
			printError();

		int cur_row=0;
		bool flag=true;
		for(j=0;j<MEM_ROW && flag;j++)
		{
			for(k=0;k<n+4;k++)
			{
				int read_flag=fread(&(data[getIndex(j,i,k)]),1,1,inp_open);

				if(!read_flag)
				{
					flag=false;
					break;
				}
			}
		}
		fclose(inp_open);
	}

	int adder=row_for_each_process*COLUMNS*MEM_COL;
	int col_mem=COLUMNS*MEM_COL;
	
	while(true)
	{
		MPI_Alltoall(data,total/numprocs,MPI_CHAR,recv_data,total/numprocs,MPI_CHAR,MPI_COMM_WORLD);
		// for(int i=0;i<MEM_ROW;i++)
		// {
		// 	// float *f=&(recv_data[getIndex(i,0,0)]);
		// 	float *f2=&(data[getIndex(i,0,0)]);
		// 	printf("%f %d\n",*f2,taskid);
		// }
		// printf("Diff Pros\n");
		// for(int i=0;i<MEM_ROW;i++)
		// {
		// 	// float *f=&(recv_data[getIndex(i,1,0)]);
		// 	float *f2=&(data[getIndex(i,1,0)]);
		// 	printf("%f %d\n",*f2,taskid);
		// }

		for(i=0;i<row_for_each_process;i++)
		{
			int pointer=0;
			for(j=i*COLUMNS*MEM_COL;j<total;j+=adder)
			{
				for(k=j;k<j+COLUMNS*MEM_COL;k++)
					temp_sort_row[pointer++]=recv_data[k];
			}
			qsort(temp_sort_row,pointer/MEM_COL,MEM_COL,cmpfunc);
			pointer=0;
			for(j=i*COLUMNS*MEM_COL;j<total;j+=adder)
			{
				for(k=j;k<j+COLUMNS*MEM_COL;k++)
					recv_data[k]=temp_sort_row[pointer++];
			}
		}

		MPI_Alltoall(recv_data,total/numprocs,MPI_CHAR,data,total/numprocs,MPI_CHAR,MPI_COMM_WORLD);

		// for(int i=0;i<MEM_ROW;i++)
		// {
		// 	// float *f=&(recv_data[getIndex(i,0,0)]);
		// 	float *f2=&(data[getIndex(i,0,0)]);
		// 	printf("%f %d\n",*f2,taskid);
		// }
		// printf("Diff Pros\n");

		// for(int i=0;i<MEM_ROW;i++)
		// {
		// 	// float *f=&(recv_data[getIndex(i,1,0)]);
		// 	float *f2=&(data[getIndex(i,1,0)]);
		// 	printf("%f %d\n",*f2,taskid);
		// }


		// printf("Entering Sort\n");
		if(sorted())
			break;
		// printf("Exit Sort\n");

		for(i=0;i<COLUMNS;i++)
		{
			int pointer=0;
			for(j=0;j<MEM_ROW;j++)
			{
				for(k=0;k<MEM_COL;k++)
				{
					temp_sort_col[pointer++]=data[getIndex(j,i,k)];
				}
			}
			qsort(temp_sort_col,pointer/MEM_COL,MEM_COL,cmpfunc);
			pointer=0;
			for(j=0;j<MEM_ROW;j++)
			{
				for(k=0;k<MEM_COL;k++)
				{
					data[getIndex(j,i,k)]=temp_sort_col[pointer++];
				}
			}
		}
		// for(i=0;i<MEM_ROW;i++)
		// {
		// 	// float *f=&(recv_data[getIndex(i,0,0)]);
		// 	float *f2=&(data[getIndex(i,0,0)]);
		// 	printf("%f %d\n",*f2,taskid);
		// }
		// printf("Diff Pros\n");

		// for(i=0;i<MEM_ROW;i++)
		// {
		// 	// float *f=&(recv_data[getIndex(i,1,0)]);
		// 	float *f2=&(data[getIndex(i,1,0)]);
		// 	printf("%f %d\n",*f2,taskid);
		// }
		break;
	}

	// printf("Reached Here\n");
	MPI_Alltoall(data,total/numprocs,MPI_CHAR,recv_data,total/numprocs,MPI_CHAR,MPI_COMM_WORLD);
	//Bringing Data in row Order

	if(taskid!=0)
	{
		MPI_Send(recv_data,total,MPI_CHAR,0,TO_MASTER,MPI_COMM_WORLD);
	}

	if(taskid==0)
	{
		sprintf(filename,"%s0",basefile);
		FILE *writer=fopen(filename,"wb");
		int p;
		for(p=0;p<numprocs;p++)
		{
			MPI_Status *status;
			if(p!=0)
				MPI_Recv(recv_data,total,MPI_CHAR,p,TO_MASTER,MPI_COMM_WORLD,status);

			for(i=0;i<row_for_each_process;i++)
			{
				int pointer=0;
				for(j=i*COLUMNS*MEM_COL;j<total;j+=adder)
				{
					for(k=j;k<j+COLUMNS*MEM_COL;k+=MEM_COL)
					{
						float *printer=&(recv_data[k]);
						// if(isinf(*printer))
						if((*printer)==INFINITY)
							continue;
						// printf("%f ",(*printer));
						int l;
						for(l=k+MEM_COL-1;l>=k;l--)
						{
							if(recv_data[l]!='\0')
								break;
						}
						fwrite(&(recv_data[k]),1,l-k+1,writer);
					}
				}
				// printf("\n");
			}
		}
		fclose(writer);
	}

	MPI_Finalize();
}