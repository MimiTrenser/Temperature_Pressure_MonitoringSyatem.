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
    ParamId_t m_eParam;
    uint64_t m_ullPollIntervalMs;
    ParamType_t m_eType;
    union 
    {
        int32_t (*pfnReadInt)(bool *ucReadStatus);
        void (*pfnReadstr)(char *buf);
    }m_ReadFn;
    uint64_t m_ullLastPollTime;
    uint8_t m_ulPolledOnce;
} PollingConfig_t;

//********************************.Process Configuration.******************************* 
//.Description   : Includes Process Configuration parameters
//************************************************************************************** 
typedef struct
{
    ParamId_t m_eParam;
    uint64_t m_ullProcessIntervalMs;
    int32_t m_lMiniThreshold;
    int64_t m_llMaxThreshold;
    uint32_t m_ulSamplingTime;
    uint32_t m_ulViolationTime;
    uint64_t m_ullLastProcessTime;
}ProcessConfig_t;

//********************************.Sensor Result.*************************************** 
//.Description   : Includes members which stores polled results and used for processing
//************************************************************************************** 
typedef struct
{
    ParamType_t m_eType;
    ParamId_t m_eParam;
    union 
    {
        int32_t lIntValue;
        char cStringValue[STR_LEN+1];
    }m_Value;
}SensorResult_t;

int32_t sReadTemperature(bool *pReadStatus);
int32_t sReadPressure(bool *pReadStatus);
void sReadConfigVersion(char *pBuffer);
long long GetTimeMs();
Read_Data_Status_t Sensor_PolledValue_Set(ParamId_t Id, const SensorResult_t *pcSetSensorData);
Read_Data_Status_t Sensor_PolledValue_Get(ParamId_t Id, SensorResult_t *pGetSensorData);

void* PollingThread(void *arg);
void* ProcessingThread(void *arg);

#endif