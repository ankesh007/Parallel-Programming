#include <bits/stdc++.h>
#include <mpi.h>
#include <omp.h>
#define pb push_back
#define mp make_pair
#define x first
#define y second
#define nodes 10000
#define NUM_THREADS 4
using namespace std; 

int TOTAL_COLUMN;
int TOTAL_ROW;
int TO_MASTER=0;
int FROM_MASTER=1;

int taskid,numprocs,rows_per_proc,cols_per_proc;
int data_per_proc;
int COLUMNS;
int total_elements;
int *row_count,*col_count;
int *row_pointers,*col_pointers;
int *rowPrefixSum,*columnPrefixSum;
vector<vector<pair<int,char*> > > row_data,col_data;
vector<vector<pair<int,char*> > > temp_row_data;
pair<int,char*> itr;

char *buffer_row,*buffer_col;
int *R2C_mapping,*C2R_mapping;
char *putRow,*putCol;
/*int row_sendcount[nodes];
int col_sendcount[nodes];
int row_disp[nodes];
int col_disp[nodes];
int row_start[nodes];
int row_end[nodes];
int col_start[nodes];
int col_end[nodes];
*/

int *row_sendcount;
int *col_sendcount;
int *row_disp;
int *col_disp;
int *row_start;
int *row_end;
int *col_start;
int *col_end;


/* Self Reference */
// Calloc(num,size)
// void * memcpy ( void * destination, const void * source, size_t num );
// void * memset ( void * ptr, int value, size_t num );

int cmpfunc1(const void *a,const void *b)
{
	char *c=(char *)a;
	char *d=(char *)b;
	float *e=(float *)(c+4);
	float *f=(float *)(d+4);
	if((*e)<(*f))
		return -1;
	else if((*e)>(*f))
		return 1;
	int *g=(int *)(c+8);
	int *h=(int *)(d+8);
	if((*g)<(*h))
		return -1;
	else if((*g)>(*h))
		return 1;
	else return 0;
}

int cmpfunc2(const void *a,const void *b)
{
	char *c=(char *)a;
	char *d=(char *)b;
	int *e=(int *)c;
	int *f=(int *)d;
	if((*e)<(*f))
		return -1;
	else if((*e)>(*f))
		return 1;
	int *g=(int *)(c+8);
	int *h=(int *)(d+8);
	if((*g)<(*h))
		return -1;
	else if((*g)>(*h))
		return 1;
	else return 0;
}

void flattenData(vector<vector<pair<int,char*> > > &data,int TOTAL,char *buffer)
{
	int place=0;
	int *col;
	col=(int *)malloc(sizeof(int));
	for(int i=0;i<TOTAL;i++)
	{
		// for(auto itr:data[i])
		int x=data[i].size();
		for(int j=0;j<x;j++)
		{
			itr=data[i][j];
			*col=itr.x;
			memcpy(buffer+place,itr.y,8);
			memcpy(buffer+place+8,col,4);
			place+=12;
		}
	}
}

void backProject(vector<vector<pair<int,char*> > > &data,int TOTAL,char *buffer)
{
	int place=0;
	for(int i=0;i<TOTAL;i++)
	{
		int x=data[i].size();
		for(int j=0;j<x;j++)
		{
			memcpy(data[i][j].y,buffer+place,8);
			place+=12;
		}
	}
}

void projectTempRowtoCol()
{
	memset(col_pointers,0,TOTAL_COLUMN*4);

	for(int i=0;i<TOTAL_ROW;i++)
	{
		// for(auto itr:row_data[i])
		// {
		int x=temp_row_data[i].size();
		for(int j=0;j<x;j++)
		{
			itr=temp_row_data[i][j];
			col_data[itr.x][col_pointers[itr.x]].y=itr.y;
			col_pointers[itr.x]++;
		}
	}
}

void projectBufToBuf(char *from,char *to,int *mapping,char *putter)
{
	int cur=0;
	for(int i=0;i<total_elements;i++)
	{
		memcpy(to+mapping[i]*12,from+cur,8);
		memcpy(to+mapping[i]*12+8,putter+4*mapping[i],4);
		cur+=12;
	}
}

void divideRowsCols()
{
	int row_modulo=TOTAL_ROW%numprocs;
	int col_modulo=TOTAL_COLUMN%numprocs;

	int row_counter=0;
	int col_counter=0;

	for(int i=0;i<numprocs;i++)
	{
		row_start[i]=row_counter;
		if(row_modulo)
		{
			row_counter+=rows_per_proc+1;
			row_modulo--;
		}
		else
		{
			row_counter+=rows_per_proc;
		}
		row_counter=min(row_counter,TOTAL_ROW);
		row_end[i]=row_counter;

		col_start[i]=col_counter;
		if(col_modulo)
		{
			col_counter+=cols_per_proc+1;
			col_modulo--;
		}
		else
			col_counter+=cols_per_proc;
		col_counter=min(col_counter,TOTAL_COLUMN);
		col_end[i]=col_counter;
		// if(taskid==0)
		// {	
		// 	cout<<row_start[i]<<" "<<row_end[i]<<"RS REnd"<<endl;
		// 	cout<<col_start[i]<<" "<<col_end[i]<<"CS CEnd"<<endl;
		// }
	}	
}

void divideRowsColsBalanced()
{
	int s=0;
	int coun=0;

	for(int i=0;i<numprocs;i++)
	{
		row_start[i]=s;
		while(s<TOTAL_ROW && coun<data_per_proc)
		{
			coun+=row_count[s];
			s++;
		}
		row_end[i]=s;
		coun=0;
	}

	s=0;

	for(int i=0;i<numprocs;i++)
	{
		col_start[i]=s;
		while(s<TOTAL_COLUMN && coun<data_per_proc)
		{
			coun+=col_count[s];
			s++;
		}
		col_end[i]=s;
		coun=0;
	}
}

int main(int argc,char **argv)
{
	clock_t timing=clock();
	clock_t timing2=clock();

	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);

	TOTAL_COLUMN=atoi(argv[4]);
	TOTAL_ROW=atoi(argv[3]);

	row_sendcount=(int *)malloc(sizeof(int)*numprocs);
	col_sendcount=(int *)malloc(sizeof(int)*numprocs);
	row_disp=(int *)malloc(sizeof(int)*numprocs);
	col_disp=(int *)malloc(sizeof(int)*numprocs);
	row_start=(int *)malloc(sizeof(int)*numprocs);
	row_end=(int *)malloc(sizeof(int)*numprocs);
	col_start=(int *)malloc(sizeof(int)*numprocs);
	col_end=(int *)malloc(sizeof(int)*numprocs);

	for(int i=0;i<numprocs;i++)
	{
		row_sendcount[i]=0;
		col_sendcount[i]=0;
		row_disp[i]=0;
		col_disp[i]=0;
		row_start[i]=0;
		row_end[i]=0;
		col_start[i]=0;
		col_end[i]=0;
	}

	// cout<<TOTAL_ROW<<" "<<TOTAL_COLUMN<<"TR TC"<<taskid<<" "<<numprocs<<endl;
	
	rows_per_proc=(TOTAL_ROW)/numprocs;
	cols_per_proc=(TOTAL_COLUMN)/numprocs;

	// divideRowsCols();


	row_count=(int *)calloc(TOTAL_ROW,sizeof(int));
	col_count=(int *)calloc(TOTAL_COLUMN,sizeof(int));
	rowPrefixSum=(int *)calloc(TOTAL_ROW,sizeof(int));
	columnPrefixSum=(int *)calloc(TOTAL_COLUMN,sizeof(int));

	if(taskid==0)
	{
		row_pointers=(int *)calloc(TOTAL_ROW,sizeof(int));
		col_pointers=(int *)calloc(TOTAL_COLUMN,sizeof(int));
		row_data.resize(TOTAL_ROW);
		temp_row_data.resize(TOTAL_ROW);
		col_data.resize(TOTAL_COLUMN);


		int row_no=0,col_no=0;
		int success;

		FILE *read_input=fopen(argv[1],"rb");

		while(fread(&row_no,4,1,read_input))
		{
			// printf("%d\n",row_no);
			success=fread(&col_no,4,1,read_input);
			char *temp;
			temp=(char *)calloc(8,1);
			success=fread(temp,1,8,read_input);
			row_data[row_no].pb(mp(col_no,temp));
			temp=(char *)calloc(8,1);
			temp_row_data[row_no].pb(mp(col_no,temp));
			row_count[row_no]++;
			col_count[col_no]++;
			total_elements++;
		}
		// for(int i=0;i<TOTAL_ROW;i++)
		// {
		// 	int x=row_data[i].size();
		// 	temp_row_data[i].resize(x);
		// 	for(int j=0;j<x;j++)
		// 	{
		// 		char* temp=(char *)calloc(8,1);
		// 		temp_row_data[i][j].y=temp;
		// 	}
		// }
		// cout<<"Hhh"<<" "<<taskid<<endl;
		fclose(read_input);

			/* Debugging */
		/* ****************** */
		// for(int i=0;i<TOTAL_ROW;i++)
		// {
		// 	for(auto itr:row_data[i])
		// 	{
		// 		int *p;
		// 		float *f;
		// 		p=(int *)itr.y;
		// 		f=(float *)(itr.y+4);
		// 		cout<<i<<" "<<itr.x<<" "<<(*p)<<" "<<(*f)<<endl;
		// 	}
		// }
	}

	// timing2=clock();
	// timing=timing2-timing;
	// cout<<"TIming bef MPI_Bcast "<<timing/CLOCKS_PER_SEC<<" "<<taskid<<endl;
	// timing=timing2;

	MPI_Bcast(row_count,TOTAL_ROW,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(col_count,TOTAL_COLUMN,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&total_elements,1,MPI_INT,0,MPI_COMM_WORLD);

	// timing2=clock();
	// timing=timing2-timing;
	// cout<<"TIming After MPI_Bcast "<<timing/CLOCKS_PER_SEC<<" "<<taskid<<endl;
	// timing=timing2;

	data_per_proc=(total_elements+numprocs-1)/numprocs;

	divideRowsColsBalanced();

	// cout<<total_elements<<" "<<taskid<<"TE TID"<<taskid<<endl;


	rowPrefixSum[0]=0;
	columnPrefixSum[0]=0;

	for(int i=1;i<TOTAL_ROW;i++)
		rowPrefixSum[i]=rowPrefixSum[i-1]+12*row_count[i-1];
	for(int i=1;i<TOTAL_COLUMN;i++)
		columnPrefixSum[i]=columnPrefixSum[i-1]+12*col_count[i-1];
	
	for(int i=0;i<numprocs;i++)
	{
		for(int j=row_start[i];j<row_end[i];j++)
			row_sendcount[i]+=row_count[j]*12;
		for(int j=col_start[i];j<col_end[i];j++)
			col_sendcount[i]+=col_count[j]*12;
	}

	if(taskid==0)
	{
		buffer_row=(char *)calloc(total_elements*12,1);
		buffer_col=(char *)calloc(total_elements*12,1);
	}
	else
	{
		buffer_row=(char *)calloc(row_sendcount[taskid],1);
		buffer_col=(char *)calloc(col_sendcount[taskid],1);
	}
	// if(taskid==0)
	// {
	// 	cout<<"RC"<<endl;
	// 	for(int i=0;i<TOTAL_ROW;i++)
	// 		cout<<row_count[i]<<" ";
	// 	cout<<endl;

	// 	cout<<"RSC"<<endl;
	// 	for(int i=0;i<numprocs;i++)
	// 		cout<<row_sendcount[i]<<" RSi ";
	// 	cout<<endl;
	// }
	for(int i=1;i<numprocs;i++)
	{
		row_disp[i]=row_disp[i-1]+row_sendcount[i-1];
		col_disp[i]=col_disp[i-1]+col_sendcount[i-1];
	}

	if(taskid==0)
	{
		putRow=(char *)calloc(total_elements*4,1);
		putCol=(char *)calloc(total_elements*4,1);
		for(int i=0;i<TOTAL_ROW;i++)
		{
			int x=row_data[i].size();
			for(int j=0;j<x;j++)
			{
				itr=row_data[i][j];
				col_data[itr.x].pb(mp(i,itr.y));
			}
		}
		
		for(int i=0;i<TOTAL_COLUMN;i++)
		{
			int x=col_data[i].size();
			for(int j=0;j<x;j++)
			{
				itr=col_data[i][j];
				row_data[itr.x][row_pointers[itr.x]].y=itr.y;
				row_data[itr.x][row_pointers[itr.x]].x=i;
				temp_row_data[itr.x][row_pointers[itr.x]].x=i;
				row_pointers[itr.x]++;
			}
		}
			/* Debugging */
		/* ****************** */
		// for(int i=0;i<TOTAL_ROW;i++)
		// {
		// 	for(auto itr:row_data[i])
		// 	{
		// 		int *p;
		// 		float *f;
		// 		p=(int *)itr.y;
		// 		f=(float *)(itr.y+4);
		// 		cout<<i<<" "<<itr.x<<" "<<(*p)<<" "<<(*f)<<endl;
		// 	}
		// }
		int cur=0;
		int *p=(int *)malloc(sizeof(int));
		for(int i=0;i<total_elements;i++)
		{
			*p=i;
			memcpy(buffer_row+cur,p,4);
			cur+=12;
		}
		// cout<<"D1"<<endl;
		backProject(temp_row_data,TOTAL_ROW,buffer_row);
		projectTempRowtoCol();
		flattenData(col_data,TOTAL_COLUMN,buffer_col);
		C2R_mapping=(int *)malloc(sizeof(int)*total_elements);
		R2C_mapping=(int *)malloc(sizeof(int)*total_elements);

		cur=8;

		for(int i=0;i<total_elements;i++)
		{
			memcpy(putCol+i*4,buffer_col+cur,4);
			cur+=12;
		}
		
		cur=0;
		// cout<<"D2"<<endl;

		for(int i=0;i<total_elements;i++)
		{
			int *p;
			p=(int *)(buffer_col+cur);
			R2C_mapping[*p]=i;
			C2R_mapping[i]=*p;
			cur+=12;
		}
		flattenData(row_data,TOTAL_ROW,buffer_row);
		cur=8;
		for(int i=0;i<total_elements;i++)
		{
			memcpy(putRow+i*4,buffer_row+cur,4);
			cur+=12;
		}
		// cout<<"Strange"<<endl;
	}
	
	// timing2=clock();
	// timing=timing2-timing;
	// cout<<"TIming bef sort "<<timing/CLOCKS_PER_SEC<<" "<<taskid<<endl;
	// timing=timing2;

	// cout<<"Hi"<<" "<<taskid<<endl;


	for(int i=0;i<4;i++)
	{
		// if(taskid==0)
		// {
		// 	// cout<<"Enter"<<endl;
		// 	flattenData(row_data,TOTAL_ROW,buffer_row);
		// 	// cout<<"Exit"<<endl;
		// }
		// MPI_Barrier(MPI_COMM_WORLD);


		// cout<<"Bef Scaater"<<taskid<<endl;
		// clock_t hey;
		// hey=clock();
		MPI_Scatterv(buffer_row,row_sendcount,row_disp,MPI_CHAR,buffer_row,row_sendcount[taskid],MPI_CHAR,0,MPI_COMM_WORLD);
		// cout<<"After Scatterv"<<taskid<<endl;
		#pragma omp parallel for 
			for(int j=row_start[taskid];j<row_end[taskid];j++)
				qsort(buffer_row+rowPrefixSum[j]-rowPrefixSum[row_start[taskid]],row_count[j],12,cmpfunc1);
		// MPI_Barrier(MPI_COMM_WORLD);

		MPI_Gatherv(buffer_row,row_sendcount[taskid],MPI_CHAR,buffer_row,row_sendcount,row_disp,MPI_CHAR,0,MPI_COMM_WORLD);
		// cout<<"Sc sor Ga"<<(clock()-hey)/CLOCKS_PER_SEC<<" "<<taskid<<endl;

		if(taskid==0)
		{
			projectBufToBuf(buffer_row,buffer_col,R2C_mapping,putCol);
			// backProject(row_data,TOTAL_ROW,buffer_row);
			// projectRowtoCol();
			// flattenData(col_data,TOTAL_COLUMN,buffer_col);
		}
		// cout<<"hey"<<" "<<taskid<<endl;
		// if(taskid==0)
		// {
		// 		/* Debugging */
		// 	/* ****************** */
		// 	cout<<"*************************"<<endl;
		// 	for(int i=0;i<TOTAL_COLUMN;i++)
		// 	{
		// 		for(auto itr:col_data[i])
		// 		{
		// 			int *p;
		// 			float *f;
		// 			p=(int *)itr.y;
		// 			f=(float *)(itr.y+4);
		// 			cout<<i<<" "<<itr.x<<" "<<(*p)<<" "<<(*f)<<endl;
		// 		}
		// 	}
		// }
		// MPI_Barrier(MPI_COMM_WORLD);
		// if(taskid==0)
		// {
			


		// 	for(int i=0;i<TOTAL_ROW;i++)
		// 	{
		// 		for(auto itr:row_data[i])
		// 		{
		// 			int *p;
		// 			float *f;
		// 			p=(int *)itr.y;
		// 			f=(float *)(itr.y+4);
		// 			cout<<i<<" "<<itr.x<<" "<<(*p)<<" "<<(*f)<<endl;
		// 		}
		// 	}

		// }		
		// MPI_Barrier(MPI_COMM_WORLD);

		// cout<<"Bef 2nd Scaater"<<taskid<<endl;
		MPI_Scatterv(buffer_col,col_sendcount,col_disp,MPI_CHAR,buffer_col,col_sendcount[taskid],MPI_CHAR,0,MPI_COMM_WORLD);
		// cout<<"After 2nd Scatterv"<<taskid<<endl;

		#pragma omp parallel for 
			for(int j=col_start[taskid];j<col_end[taskid];j++)
				qsort(buffer_col+columnPrefixSum[j]-columnPrefixSum[col_start[taskid]],col_count[j],12,cmpfunc2);
		// MPI_Barrier(MPI_COMM_WORLD);
	
		MPI_Gatherv(buffer_col,col_sendcount[taskid],MPI_CHAR,buffer_col,col_sendcount,col_disp,MPI_CHAR,0,MPI_COMM_WORLD);
		if(taskid==0)
		{
			// backProject(col_data,TOTAL_COLUMN,buffer_col);
			// cout<<"*************************"<<endl;
			// for(int i=0;i<TOTAL_COLUMN;i++)
			// {
			// 	for(auto itr:col_data[i])
			// 	{
			// 		int *p;
			// 		float *f;
			// 		p=(int *)itr.y;
			// 		f=(float *)(itr.y+4);
			// 		cout<<i<<" "<<itr.x<<" "<<(*p)<<" "<<(*f)<<endl;
			// 	}
			// }
			// projectColtoRow();
			projectBufToBuf(buffer_col,buffer_row,C2R_mapping,putRow);

		}	
		// timing2=clock();
		// timing=timing2-timing;
		// cout<<"Sorting "<<i<<" "<<timing/CLOCKS_PER_SEC<<" "<<taskid<<endl;
		// timing=timing2;
		// MPI_Barrier(MPI_COMM_WORLD);
	}
	// cout<<"Reached Seg"<<endl;

	// if(taskid==0)
	// {



	// 	for(int i=0;i<TOTAL_ROW;i++)
	// 	{
	// 		for(auto itr:row_data[i])
	// 		{
	// 			int *p;
	// 			float *f;
	// 			p=(int *)itr.y;
	// 			f=(float *)(itr.y+4);
	// 			cout<<i<<" "<<itr.x<<" "<<(*p)<<" "<<(*f)<<endl;
	// 		}
	// 	}

	// }

	if(taskid==0)
	{
		backProject(row_data,TOTAL_ROW,buffer_row);
		FILE *write_output=fopen(argv[2],"wb");
		int *p;
		p=(int *)malloc(sizeof(int));

		for(int i=0;i<TOTAL_ROW;i++)
		{
			int x=row_data[i].size();
			for(int j=0;j<x;j++)
			{
				itr=row_data[i][j];
				*p=i;
				fwrite(p,4,1,write_output);
				*p=itr.x;
				fwrite(p,4,1,write_output);
				fwrite(itr.y,1,8,write_output);
			}
		}
		fclose(write_output);
	}

	MPI_Finalize();
}