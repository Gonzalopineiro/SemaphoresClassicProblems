#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#define ARRAY_SIZE 10

sem_t mutex, hombres, mujeres, vacio;

void *hombre()
{
	while(1)
	{
		sleep(2);
		sem_wait(&mutex);
		int bool1 = sem_trywait(&hombres)==0;		//hay hombres en el baño 
		int bool2 = !sem_trywait(&vacio);			//el baño esta vacio 
		if (bool1 || bool2)							//si hay hombres o esta vacio entro 
		{
				sem_post(&hombres);					//señalo que hay un hombre mas en el baño 
				sem_post(&mutex);					//libero el mutex por si alguien mas quiere tratar de entrar 
				printf("Hombre entra al baño\n");
				sleep(1);
				printf("Hombre sale del baño\n");
				sleep(1);
				sem_wait(&hombres);					//quito mi señal de que hay hombres en el baño 
				sem_wait(&mutex);
				
				if (!sem_trywait(&hombres))			//miro si quedan mas señales de hombres en el baño 
				{
					sem_post(&hombres);				//si vi una señal la devuelvo 
				}
				else
				{
					sem_post(&vacio);				//si no vi mas señales de hombres en el baño activo la señal de que el baño esta vacio 
				}
			
			
		}
		sem_post(&mutex);
	}
	return NULL;
}

void *mujer()
{
	while(1)
	{
		sleep(2);
		sem_wait(&mutex);
		int bool1 = sem_trywait(&mujeres)==0;		//hay mujeres en el baño 
		int bool2 = !sem_trywait(&vacio);			//el baño esta vacio 
		if (bool1 || bool2)
		{		
			sem_post(&mujeres);						//señalo que hay una mujer mas en el baño 
			sem_post(&mutex);						//libero el mutex por si alguien mas quiere tratar de entrar 
			printf("Mujer entra al baño\n");
			sleep(1);
			printf("Mujer sale del baño\n");
			sleep(1);
			sem_wait(&mujeres);						//quito mi señal de que hay mujeres en el baño 
			sem_wait(&mutex);
			if (!sem_trywait(&mujeres))				//miro si quedan mas señales de mujeres en el baño 
			{
				sem_post(&mujeres);					//si vi una señal la devuelvo 
			}
			else
			{
				sem_post(&vacio);					//si no vi mas señales de mujeres en el baño activo la señal de que el baño esta vacio 

			}
	
		
	}
	sem_post(&mutex);
}
	return NULL;
}
int main()
{
	
	sem_init(&hombres,0,0);
	sem_init(&mujeres,0,0);
	sem_init(&mutex,0,1);
	sem_init(&vacio,0,1);
	
	pthread_t array_hombres[ARRAY_SIZE], array_mujeres[ARRAY_SIZE];
	
	for (int i = 0; i<ARRAY_SIZE ; i++)
	{ 
		pthread_create(&array_hombres[i],NULL,hombre,NULL);
	}
	
	for (int j = 0; j<ARRAY_SIZE ; j++)
	{
		pthread_create(&array_mujeres[j],NULL,mujer,NULL);
	}
	
	for (int i = 0; i<ARRAY_SIZE ; i++)
	{ 
		pthread_join(array_hombres[i],NULL);
	}
	
	for (int k = 0; k<ARRAY_SIZE ; k++)
	{ 
		pthread_join(array_mujeres[k],NULL);
	}
	return 0;
}
