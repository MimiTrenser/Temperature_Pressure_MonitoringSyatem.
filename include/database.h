//********************** Temparature Pressure Monitoring System ****************
// Copyright (c) 2026 Trenser Technology Solutions (P) Ltd
// All Rights Reserved
//******************************************************************************
//
// Summary      : The objective is to develop a multi-threaded embedded 
//                application to monitor environmental parameters 
//                (Temperature and Pressure) and system metadata
//                (Configuration Version). 
// Note         : None
//
//******************************************************************************

#ifndef DATABASE_H
#define DATABASE_H

//******************************* Include Files ********************************
#include <pthread.h>
#include <stdio.h>
#include "database.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdbool.h>

#define STR_LEN (5)

//***************************** Global Types ***********************************
 
//***************************** Global Constants *******************************
 
//***************************** Global Variables *******************************
 
//***************************** Forward Declarations ***************************

typedef enum
{
    DATA_ALLOCATION_SUCCESS,
    DATA_ERROR,
    DATA_ALLOCATION_FAILED
} READ_DATA_STATUS_t;

typedef enum
{
    PARAM_TEMP ,
    PARAM_PRESSURE ,
    PARAM_CONFIG_VERSION,
    PARAM_MAX
} PARAM_ID_t;

typedef enum
{
    PARAM_TYPE_INT,
    PARAM_TYPE_STRING
} PARAM_TYPE_t;

//******************************** Polling Configuration ***********************
// Description   : Includes polling Configuration parameters
//******************************************************************************
typedef struct
{
    PARAM_ID_t m_eParam;
    uint64_t m_ullPollIntervalMs;
    PARAM_TYPE_t m_eType;
    union 
    {
        int32_t (*pfnReadInt)(bool *ucReadStatus);
        void (*pfnReadstr)(char *buf);
    }m_READFN;
    uint64_t m_ullLastPollTime;
    uint8_t m_ulPolledOnce;
} POLLING_CONFIG_t;

//********************************.Process Configuration.***********************
// Description   : Includes Process Configuration parameters
//******************************************************************************
typedef struct
{
    PARAM_ID_t m_eParam;
    uint64_t m_ullProcessIntervalMs;
    int32_t m_lMiniThreshold;
    int64_t m_llMaxThreshold;
    uint32_t m_ulSamplingTime;
    uint32_t m_ulViolationTime;
    uint64_t m_ullLastProcessTime;
} PROCESS_CONFIG_t;

//********************************.Sensor Result.*******************************
// Description   : Includes members which stores polled results and used for
//                 processing
//******************************************************************************
typedef struct
{
    PARAM_TYPE_t m_eType;
    PARAM_ID_t m_eParam;
    union
    {
        int32_t lIntValue;
        char cStringValue[STR_LEN+1];
    }m_VALUE;
} SENSOR_RESULT_t;

int32_t readTemperature(bool *pblReadStatus);
int32_t readPressure(bool *pblReadStatus);
void readConfigVersion(char *pBuffer);
long long GetTimeMs();
READ_DATA_STATUS_t sensorPolledValueSet(PARAM_ID_t Id, const SENSOR_RESULT_t *pcSetSensorData);
READ_DATA_STATUS_t sensorPolledValueGet(PARAM_ID_t Id, SENSOR_RESULT_t *pGetSensorData);

void* pollingThread(void *arg);
void* processingThread(void *arg);

#endif // _DATABASE_H
//EOF