#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<semaphore.h>
#include<fcntl.h>
#include<pthread.h>
#include<time.h>
#include<assert.h>
#include<errno.h>
#include<unistd.h>

void* run(void* param);
void* detector(void* param);
int matrix[20][20];
FILE *fp[20];
int t,f,counter2;
sem_t sem[20];
int flags=0; int flagd=0; int counter=0;

void main()
{
	int i,j,k;

	for(i=0;i<20;i++)
		for(j=0;j<20;j++)
			matrix[i][j]=0;

	printf("\nEnter number of threads: ");
	scanf("%d",&t);

	printf("\nEnter number  of files: ");
	scanf("%d",&f);

	char data[4];
	for(i=0; i<f; i++)
	{
		snprintf(data,sizeof(data),"%d",i+1);
		fp[i] = fopen(data,"a+");
	}
	pthread_t th[t];
	counter2=t;

	for(i=0;i<f;i++)
		sem_init(&sem[i],0,1);
	for(i=0;i<t;i++)
	{
		printf("\nThread #%d! Enter files for this thread (type -1 to end): \n",i+1);
		for(j=0;j<f;j++)
		{
			scanf("%d",&k);
			if(k==-1)
				break;
			matrix[i][k]=1;
		}
	}
	printf("\nDependency matrix: Rows (Files), Columns (Threads based on priority): \n");
	for(i=0;i<t;++i)
	{
		for(j=0;j<f;j++)
			printf("%d ",matrix[i][j]);
		printf("\n");
	} 
	int ptr[t];
	for(i=0;i<t;i++)
	{
		ptr[i]=i;
		pthread_create(&th[i],NULL,run,(void*)&ptr[i]);
		sleep(1);
	}
	pthread_t d;
	pthread_create(&d,NULL,detector,NULL);
	
	for(i=0;i<t;i++)
		pthread_join(th[i],NULL);
	printf("\n\nNo Deadlock Exists\n");
	for(i=0;i<t;++i)
	{
		for(j=0;j<f;j++)
			printf("%d ",matrix[i][j]);
		printf("\n");
	} 

}

void* detector(void *param)
{
	int r=0;int i,j;int count;
	for(count=0;count<t;count++)
	{
		sleep(10);
		for(i=0;i<t;++i)
			for(j=0;j<f;j++)
				if(matrix[i][j]==1)
					r=1;
		
		if((count%5==0)&&(count!=0))
		{
			printf("\nStarvation exists!\n");
			flags=1;
			sleep(3);
		}

		else
			if(r==1)
			{
				printf("Potential DeadLock!!\n");
				counter++;

				flagd=1;
			}
	}
}

void* run(void *param)
{
	int i = *((int*)param);
	int p,q;
	int count=0;

	struct timespec stamp;

	double time;
	while(1)
	{
		if(flagd==1)
		{
			if(i+1>counter)
			{
				printf("%d\n",i);
				for(p=0;p<f;p++)
					if(matrix[i][p]==2)
					{
						matrix[i][p]=1;
						sem_post(&sem[p]);
					}
				sleep(5);
				flagd=0;
			}
		}
		if(flags==1)
		{

			if(i<counter2-1)
			{
				printf("%d\n",i);
				for(p=0;p<f;p++)
					if(matrix[i][p]==2)
					{
						matrix[i][p]=1;
						sem_post(&sem[p]);
					}

				sleep(5);
				flags=0;
				counter2--;
			}
		}

		count=0;
		for(p=0;p<f;p++)
		{
			int s=2;
			
			if(matrix[i][p]==1)
				s = sem_trywait(&sem[p]);
			if((s==0)||(matrix[i][p]==2))
			{
				matrix[i][p]=2;
				clock_gettime(CLOCK_MONOTONIC,&stamp);
				time=stamp.tv_sec;
				fprintf(fp[p],"%d %lf sec\n",i,time);
				sleep(1);
			}
		}
		for(q=0;q<f;q++)
			if(matrix[i][q]!=1)
				count++;
		if(count==f)
		{

			for(q=0;q<f;q++)
				if(matrix[i][q]==2)
					sem_post(&sem[q]);
			pthread_exit(0);
		}

	}
}
