#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "mpi.h"
char *data;
// char ***sortData;
int comparator(const void *p,const void *q)
{
	char t=(const char *)p;
	if(t=='\0')
		return -1;
	char t1=(const char *)q;
	if(t1=='\0')
		return 1;
	float l=*(const float *)p;
	// printf("%d ",l);
	float r=*(const float *)q;
	if(l<r)
		return -1;
	else if(l>r)
		return 1;
	else return 0;
}
// void contigous_array(int x1,int x2,int x3)
// {
// 	char *data1=(char *)malloc(x1*x2*x3*sizeof(char));
// 	data=(char ***)malloc((x1)*sizeof(char **));
// 	for(int i=0;i<x1;i++)
// 	{
// 		data[i]=(char **)malloc((x2)*sizeof(char *));
// 		for(int j=0;j<x2;j++)
// 		{
// 			data[i][j]=&(data1[(((i*x2)+j)*x3)]);
// 		}
// 	}
// }
// void contigous_array2(char *data1,int x1,int x2,int x3)
// {
// 	// char *data1=(char *)malloc(x1*x2*x3*sizeof(char));
// 	sortData=(char ***)malloc((x1)*sizeof(char **));
// 	for(int i=0;i<x1;i++)
// 	{
// 		sortData[i]=(char **)malloc((x2)*sizeof(char *));
// 		for(int j=0;j<x2;j++)
// 		{
// 			sortData[i][j]=&(data1[(((i*x2)+j)*x3)]);
// 		}
// 	}
// }
// void row_extract(int i,int max_rows,int number_column,int max_value_length,char **column)
// {
// 	int s=max_rows*max_value_length;

// 	for(int i=0;i<number_column;i++)
// 	{

// 	}
// }
int main(int argc, char* argv[])
{
	int number_column=atoi(argv[1]);
	// printf("%d\n",number_column);
	MPI_Init(&argc, &argv);
	int numProc, myRank;
	
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);// Group size
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	// printf("%d \n",myRank);
	  // r for read, b for binary
	int s=(number_column+numProc-1)/numProc;
	int r=number_column%numProc;
	int count_begin=0;
	int count_end=0;
	// if(r>0 && myRank==numProc-1)
	// {
	// 	count_begin=(myRank*(s+1));
	// 	count_end=(number_column);
	// }
	// else 
	// if(r>0)
	// {
	// 	count_begin=(myRank*(s+1));
	// 	count_end=(myRank+1)*(s+1);
	// }
	// else
	// {
	// 	count_begin=(myRank*s);
	// 	count_end=(myRank+1)*s;
	// }
	count_begin=myRank*s;
	count_end=(myRank+1)*s;
	count_begin++;
	count_end++;
	// printf("%d %d\n",count_begin,count_end);
	int buffer;
	
	int max=0;
	// char *str1;
	// str1=argv[2];
	int max_length=0;
	for(int j=count_begin;j<(count_end);j++)
	{
		if(j>number_column)
			continue;
		FILE *ptr;
		char str[500];
		sprintf(str, "%s%d",argv[2],j);
		// printf("%s\n",str);
		
		// strcat(str1,str);
		// printf("%s\n",str1);
		ptr = fopen(str,"r");
		int buffer;
		fread(&buffer,sizeof(buffer),1,ptr);
		if(buffer>max)
		{
			max=buffer;
		}
		int p=ftell(ptr);
		fseek(ptr,0,SEEK_END);
		int ur=ftell(ptr);
		int us=(ur-p)/(4+buffer);
		if(us>max_length)
		{
			max_length=us;
		}
		fclose(ptr);
	}
	// printf("%d %d\n",max,max_length);
	MPI_Bcast( &max, 1, MPI_INT, myRank, MPI_COMM_WORLD );
	// int max2=max;
	int max1;
	for(int j=0;j<numProc;j++)
	{
		if(j==myRank)
			continue;
		MPI_Bcast( &max1, 1, MPI_INT, j, MPI_COMM_WORLD );
		if(max1>max)
		{
			max=max1;
		}
	}
	// printf("%d \n",max);
	int max_length1;
	MPI_Bcast( &max_length, 1, MPI_INT, myRank, MPI_COMM_WORLD );
	// int max2=max;
	// int max1;
	for(int j=0;j<numProc;j++)
	{
		if(j==myRank)
			continue;
		MPI_Bcast( &max_length1, 1, MPI_INT, j, MPI_COMM_WORLD );
		if(max_length1>max_length)
		{
			max_length=max_length1;
		}
	}
	
	int max_row=((max_length+numProc-1)/numProc)*numProc;
	
	int max_value_length=max+4;
	// printf("%d \n",max_value_length);
	// contigous_array(max_row,(count_end-count_begin),max_value_length);
	// data=(char ***)malloc((count_end-count_begin)*sizeof(char **));
	// for(int j=0;j<(count_end-count_begin);j++)
	// {
	// 	data[j]=(char **)malloc(max_row*sizeof(char *));
	// 	for(int k=0;k<max_row;k++)
	// 	{
	// 		data[j][k]=(char *)malloc((max_value_length)*sizeof(char));	
	// 	}
	// }
	data=(char *)malloc(max_row*(count_end-count_begin)*max_value_length*sizeof(char));
	int pointer=0;
	for(int j=count_begin;j<count_end;j++)
	{
		FILE *ptr;
		if(j>number_column)
			continue;
		// printf("%d ",j);
		char str[500];
		sprintf(str, "%s%d",argv[2],j);
		ptr = fopen(str,"r");
		int orig_length;
		fread(&orig_length,sizeof(orig_length),1,ptr);
		int count_read_row=0;
		// printf("%d ",orig_length);
		int j1=j-count_begin;
		// fread(&(data[j1][count_read_row]),(4+orig_length),1,ptr);
		// data[j1][count_read_row][0]='c';
		// fread(&(data[j1][count_read_row][0]),1,1,ptr);
		for(int count_read_row=0;count_read_row<max_row;count_read_row++)
		{
			for(int jj=0;jj<max_value_length;jj++)
			{
				if(jj<(orig_length+4)){
					int s=fread(&(data[pointer]),1,1,ptr);
					if(s==0)
					{
						data[pointer]='\0';
						break;
					}
				}
				else
					data[pointer]='\0';
				pointer++;
			}
			
		}
		// while(fread(&(data[j1][count_read_row]),(4+orig_length),1,ptr))
		// {
		// 	// printf("%c ",data[j1][count_read_row][0]);
		// 	int jj=4+orig_length;
		// 	while(jj<max_value_length)
		// 	{
		// 		// data[j1][count_read_row][jj]='c';
		// 		jj++;
		// 	}
		// 	count_read_row++;
		// }
		// while(count)
		fclose(ptr);
	}
	// return 0;
	MPI_Datatype char_array;
	MPI_Type_contiguous((count_end-count_begin)*max_value_length,MPI_CHAR,&char_array);
	MPI_Type_commit(&char_array);

	
	// float *p1=sortData[0][0];
	// printf("%f \n",p1);
	// char_array data_send[max_row];
	// for (int i=0;i<max_row;i++)
	// 	data_send[i]=data[]
	
	// printf("%d \n",(max_row/numProc));
	char *temp=(char *)malloc(max_row*(count_end-count_begin)*max_value_length*sizeof(char));
	float *p=data;
	printf("%f \n",*p);
	// MPI_Alltoall(data, (max_row/numProc)*(count_end-count_begin)*max_value_length, MPI_CHAR, temp, (max_row/numProc)*(count_end-count_begin)*max_value_length, MPI_CHAR, MPI_COMM_WORLD);
	// float *p1=temp;
	// printf("%f \n",p1);
	// contigous_array2(temp,max_row,(count_end-count_begin),max_value_length);
	// // char buf[(count_end-count_begin)][max_value_length];
	// // float *f=sortData[0][0];
	// // printf("%f \n",p);
	// for(int i=0;i<max_row;i++)
	// {
	// 	for(int j=0;j<(count_end-count_begin);j++)
	// 	{
	// 		float *f=sortData[i][j];
	// 		printf("%f \n",f);
	// 	}
	// }
	// char **row=(char **)malloc(number_column*sizeof(char *));
	// for(int i=0;i<number_column;i++)
	// 	row[i]=(char *)malloc(max_value_length*sizeof(char));
	// int row_proc=((count_end-count_begin)*max_row)/(number_column);
	
	// int col1=(count_end-count_begin);
	// for(int i=0;i<(row_proc);i++)
	// {
	// 	int s=0;
	// 	for(int j=i;j<(max_row);j+=row_proc)
	// 	{
	// 		// int a=(i+(j*row_proc))/max_row;
	// 		for(int r=0;r<col1;r++)
	// 		{
	// 			for(int kk=0;kk<max_value_length;kk++)
	// 			{
	// 				// row[s][kk]=sortData[j][r][kk];
	// 				printf("a %c",sortData[0][0][0]);
	// 			}
	// 			s++;
	// 		}
	// 		// row[j]=sortData[s][j];
	// 	}
	// 	// qsort((void*)row, number_column, sizeof(row[0]), comparator);
	// }
	// for(int i=0;i<number_column;i++)
	// {
	// 	// for(int j=0;j<max_row;j++)
	// 	// {

	// 	// }
	// }
	// qsort((void*)sortData, number_column, sizeof(recv[0]), comparator);
	// for(int j=0;j<3;j++)
	// {
	// 	float *key1=key+(j*number_column);
	// 	// printf("%f %d\n",*key1,number_column);
	// 	float recv[2];
	// 	MPI_Alltoall(key1, number_column, MPI_FLOAT, recv, number_column, MPI_FLOAT, MPI_COMM_WORLD);
	// 	// MPI_Barrier();
	// 	printf("%f %f\n",recv[0],recv[1]);
	// 	qsort((void*)recv, number_column, sizeof(recv[0]), comparator);
	// 	MPI_Alltoall(recv, number_column, MPI_FLOAT, key1, number_column, MPI_FLOAT, MPI_COMM_WORLD);
		
	// }
	// // qsort((void*)recv, number_column, sizeof(recv[0]), comparator);
	// // int b
	// // MPI_Type_create_struct(2,)
	// // for(int i = 0; i<4; i++)
 // //    	printf("%u ", buffer[i]);
	// // printf("\n");
	MPI_Finalize();
	return 0;
}