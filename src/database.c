//**************************** Temparature Pressure Monitoring System ************************************** 
//  Copyright (c) 2021 Trenser 
//  All Rights Reserved 
//********************************************************************************************************** 
// 
// File   : FileName.cpp 
// Summary : Sensor.c file Exchanges polling data for processing .
// Note    : 
// Author  : Mimi C.S
// Date    : 19/12/2025
// 
//**********************************************************************************************************
#include "database.h"
#include <pthread.h>
#include <string.h>
#include <time.h>
#include<unistd.h>
#include <sys/time.h>
#include<stdlib.h>

static pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;
int32_t *g_lTemperatureData = NULL;
int32_t *g_lPressureData = NULL;
 
//******************************.GetTimeMs.****************************** 
//Purpose : To get the current System Time in Ms
//Return  : Updated system time in Ms 
//********************************************************************************** 
long long GetTimeMs() 
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long milliseconds = (long long)tv.tv_sec * 1000 + (long long)tv.tv_usec / 1000;
    return milliseconds;
}

 //******************************.SetPolledValue.*************************************
//Purpose : Updates the sensor data for a specific parameter ID in a Memory
//          in a thread-safe manner.
//Inputs  : eId - The unique identifier for the parameter to be updated.
//          pcstSensorData - Pointer to the constant sensor data structure.
//Notes   : Requires g_mtxDataLock to be initialized before calling.
//********************************************************************************** 
Read_Data_Status_t SetPolledValue(ParamId_t eId,const SensorResult *pcSetSensorData)
{
    if(eId>=PARAM_MAX || pcSetSensorData == NULL)
    {
        return DATA_ERROR;
    }
    pthread_mutex_lock(&gMutex);
    if(pcSetSensorData->m_eParam == PARAM_TEMP)
    {
        if(g_lTemperatureData == NULL)
        {
            g_lTemperatureData = (int32_t*)malloc(sizeof(int32_t));
            if(g_lTemperatureData == NULL)
            {
                pthread_mutex_unlock(&gMutex);
                return DATA_ALLOCATION_FAILED;
            }
        }
        *g_lTemperatureData = pcSetSensorData->m_Value.lIntValue;
    }    
    else if(pcSetSensorData->m_eParam == PARAM_PRESSURE)
    {
        if(g_lPressureData == NULL)
        {
            g_lPressureData = (int32_t*)malloc(sizeof(int32_t));
            if(g_lPressureData == NULL)
            {
                pthread_mutex_unlock(&gMutex);
                return DATA_ALLOCATION_FAILED;
            }
        }
        *g_lPressureData = pcSetSensorData->m_Value.lIntValue;
    }
    pthread_mutex_unlock(&gMutex);
    return DATA_ALLOCATION_SUCCESS;
}

//******************************.GetPolledValue.************************************ 
//Purpose : 
//Inputs  : eId - The unique identifier for the parameter to be updated.
//          pGetSensorData - Pointer to which Sensordata is updated.  
//Notes   :  Requires g_mtxDataLock to be initialized before calling. 
//********************************************************************************** 

Read_Data_Status_t GetPolledValue(ParamId_t eId,SensorResult *pGetSensorData)
{
    if(eId>=PARAM_MAX)
    {
        return DATA_ERROR;
    }
    pthread_mutex_lock(&gMutex);
    if(eId == PARAM_TEMP)
    {
        if(g_lTemperatureData == NULL)
        {
            pthread_mutex_unlock(&gMutex);
            return DATA_ERROR;
        }
        pGetSensorData->m_Value.lIntValue = *g_lTemperatureData;
        pGetSensorData->m_eParam = PARAM_TEMP;
    }
    if(eId == PARAM_PRESSURE)
    {
        if(g_lPressureData == NULL)
        {
            pthread_mutex_unlock(&gMutex);
            return DATA_ERROR;
        }
        pGetSensorData->m_Value.lIntValue = *g_lPressureData;
        pGetSensorData->m_eParam = PARAM_PRESSURE;
    }
    pthread_mutex_unlock(&gMutex);  
    return DATA_ALLOCATION_SUCCESS;
}