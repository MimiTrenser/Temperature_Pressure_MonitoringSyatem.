//****************************Temperature Pressure Monitoring System ***********
// Copyright (c) 2026 Trenser Technology Solutions (P) Ltd 
// All Rights Reserved 
//****************************************************************************** 
// 
// File    : main.c 
// Summary : Main.c file includes thread creation for polling and process
// Note    : 
// Author  : Mimi C.S
// Date    : 19/12/2025
// 
//****************************************************************************** 
#include "database.h"

#define SUCCESS (0)

int main(void)
{
    pthread_t pthread1 = {0};
    pthread_t pthread2 = {0};
    int status = SUCCESS;
    status = pthread_create(&pthread1, NULL, PollingThread, NULL);

    if(status != SUCCESS)
    {
        printf("Thread Creation Failed\n");
    }

    status = pthread_create(&pthread2, NULL, ProcessingThread, NULL);

    if(status != SUCCESS)
    {
        printf("Thread Creation Failed\n");
    }

    pthread_join(pthread1, NULL);
    pthread_join(pthread2, NULL);

    return 0;
}