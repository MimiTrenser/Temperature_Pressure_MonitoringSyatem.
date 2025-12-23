//****************************Temperature Pressure Monitoring System ************************************** 
//  Copyright (c) 2021 Trenser 
//  All Rights Reserved 
//***************************************************************************** 
// 
// File    : main.c 
// Summary : Main.c file includes thread creation for polling and process
// Note    : 
// Author  : Mimi C.S
// Date    : 19/12/2025
// 
//***************************************************************************** 
#include <pthread.h>
#include <stdio.h>
#include <database.h>
#include <unistd.h>

#define SUCCESS 0

int main(void)
{
    pthread_t pthread1;
    pthread_t pthread2;
    int status = SUCCESS;

    status = pthread_create(&pthread1, NULL, PollingThread, NULL);/* Polling Thread create */
    if(status != SUCCESS)
    {
        printf("Thread Creation Failed\n");
    }

    status = pthread_create(&pthread2, NULL, ProcessingThread, NULL);/* Processing Thread Create */
    if(status != SUCCESS)
    {
        printf("Thread Creation Failed\n");
    }

    pthread_join(pthread1, NULL);
    pthread_join(pthread2, NULL);

    return 0;

}