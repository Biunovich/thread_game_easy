#include "mylib.h"
#define SKLAD 5
#define CLIENT 10
#define MAX_SKLAD 500
int sklad[SKLAD];
int client[CLIENT];
pthread_mutex_t mutex[SKLAD];
void change_try(int *skl,int val,int *push,int id)
{
	if ((pthread_mutex_trylock(&mutex[id])) == EBUSY)
		return;
	*push = *skl + val;
	if (*push > MAX_SKLAD)
	{
		*skl = MAX_SKLAD;
		*push = val - (*push - MAX_SKLAD);
	}
	else
	{
		if(*push<0)
		{
			*push = -*skl;
			*skl = 0;
		}
		else
		{
			*skl +=val;
			*push = val;
		}
	}
	pthread_mutex_unlock(&mutex[id]);
}
void *load_came()
{
	int s_id,push;
	while(1)
	{
		s_id = rand()%SKLAD;
		push=0;
		change_try(&sklad[s_id],rand()%300,&push,s_id);
		printf("Loader: push %d to Sklad %d. Summ %d\n", push, s_id, sklad[s_id]);
		sleep(1);
	}
}
void *client_came(void * arg)
{
	int id = *(int*)arg, s_id ,push;
	while(client[id]>0)
	{
		s_id = rand()%SKLAD;
		push = 0;
		change_try(&sklad[s_id],-rand()%100,&push,s_id);
		client[id] +=push;
		printf("Client(%d): pop %d from Sklad %d(%d)\n", id, -push, s_id, sklad[s_id]);
		sleep(1);
	}
	printf("Client(%d) Done!\n", id);
	pthread_exit(0);
}
int main()
{
	int i, taskids[CLIENT];
	for (i=0;i<SKLAD;i++)
	{
		sklad[i] = 0;
		pthread_mutex_init(&mutex[i],NULL);
	}
	for (i=0;i<CLIENT;i++)
		client[i] = rand()%500;
	pthread_t cl_id[CLIENT], loader;
	for (i=0;i<CLIENT;i++)
	{
		taskids[i] = i;
		pthread_create(&cl_id[i],NULL,client_came,&taskids[i]);
	}
	pthread_create(&loader,NULL,load_came,NULL);
	for (i=0;i<CLIENT;i++)
		pthread_join(cl_id[i],0);
	pthread_cancel(loader);
	return 0;
}