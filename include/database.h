//**************************** Temparature Pressure Monitoring System ***********
//  Copyright (c) 2021 Trenser 
//  All Rights Reserved 
//********************************************************************************
// 
// Summary    : The objective is to develop a multi-threaded embedded application
//              to monitor environmental parameters (Temperature and Pressure) and
//              system metadata (Configuration Version). 
// Note       : 
// 
//******************************************************************************* 
#ifndef SENSOR_SYSTEM_H
#define SENSOR_SYSTEM_H

#include <stdint.h>
#include <sys/time.h>
#include <stdbool.h>

#define STR_LEN 5

typedef enum
{
    DATA_ALLOCATION_SUCCESS,
    DATA_ERROR,
    DATA_ALLOCATION_FAILED
}Read_Data_Status_t;

typedef enum
{
    PARAM_TEMP ,
    PARAM_PRESSURE ,
    PARAM_CONFIG_VERSION,
    PARAM_MAX
} ParamId_t;

typedef enum
{
    PARAM_TYPE_INT,
    PARAM_TYPE_STRING
}ParamType_t;

//********************************.Polling Configuration.******************************* 
//.Description   : Includes polling Configuration parameters
//************************************************************************************** 
typedef struct
{
    ParamId_t m_eParam;                           /*Parameters for Polling*/
    uint64_t m_ullPollIntervalMs;                 /*Polling Interval in ms*/
    ParamType_t m_eType;                          /*Prameter Type -> Int or String*/
    union 
    {
        int32_t (*pfnReadInt)(bool *ucReadStatus); /*Read Temp and Read Pressure*///uint8_t *cReadStatus
        void (*pfnReadstr)(char *buf);             /*Read Config Version*/
    }m_ReadFn;
    uint64_t m_ullLastPollTime;                    /*Last poll time*/
    uint8_t m_ulPolledOnce;                        /*Config verion need to be polled once*/
} PollingConfig_t;

//********************************.Process Configuration.******************************* 
//.Description   : Includes Process Configuration parameters
//************************************************************************************** 
typedef struct
{
    ParamId_t m_eParam;              /*Parametrs for Processing*/
    uint64_t m_ullProcessIntervalMs; /*Process interval in ms*/
    int32_t m_lMiniThreshold;        /*Mini Threshold value*/
    int64_t m_llMaxThreshold;        /*Max Threshold Value*/
    uint32_t m_ulSamplingTime;       /*Sampling Time(After sampling time violation notification should sent)*/
    uint32_t m_ulViolationTime;      /*In every violation increment by process time*/
    uint64_t m_ullLastProcessTime;   /*Last proccess time */
}ProcessConfig_t;

//********************************.Sensor Result.*************************************** 
//.Description   : Includes members which stores polled results and used for processing
//************************************************************************************** 
typedef struct
{
    ParamType_t m_eType;
    ParamId_t m_eParam;              /*Parameter Type -> Int or String*/
    union 
    {
        int32_t lIntValue;
        char StringValue[STR_LEN+1];
    }m_Value;                        /* Union for storing polled values*/
}SensorResult_t;

int32_t sReadTemperature(bool *pReadStatus);
int32_t sReadPressure(bool *pReadStatus);
void sReadConfigVersion(char *pBuffer);
long long GetTimeMs();
Read_Data_Status_t SetPolledValue(ParamId_t Id, const SensorResult_t *pcSetSensorData);
Read_Data_Status_t GetPolledValue(ParamId_t Id, SensorResult_t *pGetSensorData);

void* PollingThread(void *arg);
void* ProcessingThread(void *arg);

#endif