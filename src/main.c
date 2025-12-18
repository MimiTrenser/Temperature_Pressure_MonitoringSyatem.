#include <pthread.h>
#include<stdio.h>
//#include<stdlib.h>
#include<sensor.h>
#include<unistd.h>



int main(void)
{
    pthread_t pthread1;
    pthread_t pthread2;

    pthread_create(&pthread1,NULL,PollingThread,NULL);
    pthread_create(&pthread2,NULL,ProcessingThread,NULL);

    pthread_join(pthread1,NULL);
    pthread_join(pthread2,NULL);

    return 0;

}