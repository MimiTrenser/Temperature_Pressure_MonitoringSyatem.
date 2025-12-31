//**************************** Temparature Pressure Monitoring System ************************************** 
//  Copyright (c) 2021 Trenser 
//  All Rights Reserved 
//********************************************************************************************************** 
// 
// File    : FileName.cpp 
// Summary : Sensor.c file Exchanges polling data for processing .
// Note    : 
// Author  : Mimi C.S
// Date    : 19/12/2025
// 
//**********************************************************************************************************
#include "../include/database.h"
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>

static pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;
static int32_t *s_plDataBase = NULL;
 
//******************************.GetTimeMs.****************************** 
//Purpose : To get the current System Time in Ms
//Return  : Updated system time in Ms 
//********************************************************************************** 
long long GetTimeMs() 
{
    struct timeval tv = {0};
    gettimeofday(&tv, NULL);
    long long milliseconds = (long long)tv.tv_sec * 1000 + (long long)tv.tv_usec / 1000;
    return milliseconds;
}

//******************************.Sensor_PolledValue_Set.*************************************
//Purpose : Updates the sensor data for a specific parameter ID in a Memory
//          in a thread-safe manner.
//Inputs  : eId - The unique identifier for the parameter to be updated.
//          pcstSensorData - Pointer to the constant sensor data structure.
//Return  : It will return Data allocation status an enum
//Notes   : Requires g_mtxDataLock to be initialized before calling.
//************************************************************************************ 
Read_Data_Status_t Sensor_PolledValue_Set(ParamId_t eId, const SensorResult_t *pcSetSensorData)
{
    if(eId >= PARAM_MAX || pcSetSensorData == NULL)
    {
        return DATA_ERROR;
    }
    pthread_mutex_lock(&gMutex);
    if(s_plDataBase == NULL)
    {
        s_plDataBase = (int32_t*)malloc(sizeof(int32_t)*PARAM_MAX);
        if(s_plDataBase == NULL)
        {
            free(s_plDataBase);
            s_plDataBase = NULL;
            pthread_mutex_unlock(&gMutex);
            return DATA_ALLOCATION_FAILED;
        }
    }
    s_plDataBase[eId] = pcSetSensorData->m_Value.lIntValue;
    pthread_mutex_unlock(&gMutex);
    return DATA_ALLOCATION_SUCCESS;
}

//******************************.Sensor_PolledValue_Get.************************************ 
//Purpose : Getting polled values.
//Inputs  : eId - The unique identifier for the parameter to be updated.
//          pGetSensorData - Pointer to which Sensordata is updated. 
//Return  : Return data allocation status as enum
//Notes   :  Requires g_mtxDataLock to be initialized before calling. 
//********************************************************************************** 
Read_Data_Status_t Sensor_PolledValue_Get(ParamId_t eId, SensorResult_t *pGetSensorData)
{
    if(eId >= PARAM_MAX || pGetSensorData == NULL)
    {
        return DATA_ERROR;
    }
    pthread_mutex_lock(&gMutex);
    if(s_plDataBase == NULL)
    {
        pthread_mutex_unlock(&gMutex);
        return DATA_ERROR;
    }
    pGetSensorData->m_Value.lIntValue = s_plDataBase[eId];
    pGetSensorData->m_eParam = eId;
    pthread_mutex_unlock(&gMutex);  
    return DATA_ALLOCATION_SUCCESS;
}