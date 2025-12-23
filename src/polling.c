//**************************** Temparature Pressure Monitoring System ************************************** 
//  Copyright (c) 2021 Trenser 
//  All Rights Reserved 
//***************************************************************************** 
// 
// File    : Polling.c
// Summary : Polling.c file includes execution of Polling Thread,Read Temperature and
//           Pressure sensors
// Note    : 
// Author  : Mimi C.S
// Date    : 19/12/2025
// 
//*****************************************************************************
#include "database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>

#define TemperatureLowerBound -20
#define TemperatureUpperBound 100
#define PressureLowerBound 0
#define PressureUpperBound 10000

//******************************.gPollingTable.****************************************** 
//Purpose : Parses the Configuration parameters of Temperature,Pressure & Config version.
//*************************************************************************************** 
PollingConfig_t gPollingTable[] = {{PARAM_TEMP, 50, PARAM_TYPE_INT, .m_ReadFn.pfnReadInt = sReadTemperature, 0, 0},
                                    {PARAM_PRESSURE, 200, PARAM_TYPE_INT, .m_ReadFn.pfnReadInt = sReadPressure, 0, 0},
                                    {PARAM_CONFIG_VERSION, 0, PARAM_TYPE_STRING, .m_ReadFn.pfnReadstr = sReadConfigVersion, 0, 0}};

#define POLL_TABLE_SIZE (sizeof(gPollingTable) / sizeof(PollingConfig_t))

//******************************.sReadTemperature.****************************************** 
//Purpose : Generates a simulated Temperature reading and validates it against bounds.
//Inputs  : pReadStatus - Pointer to a boolean to store the success/failure status.
//Return  : The generated temperature value (-20 to 120).
//Notes   : Simulated via rand(); depends on TemperatureLowerBound and TemperatureUpperBound 
//****************************************************************************************** 
int32_t sReadTemperature(bool *pReadStatus)//todo
{
    if(pReadStatus == NULL)
    {
        return DATA_ERROR;
    }
    static int32_t sTemperature; 
    sTemperature = (TemperatureLowerBound + rand() % 141);                           /* Genereate random values from 0 to 120 */
    if(sTemperature < TemperatureLowerBound || sTemperature > TemperatureUpperBound) /* To check temp values are out of bound */
    {
        *pReadStatus = false;
    }
    else
    {
        *pReadStatus = true;
    }
    return sTemperature;
}
 
//******************************.sReadPressure.**************************************** 
//Purpose : Generates a simulated pressure reading and validates it against bounds.
//Inputs  : pReadStatus - Pointer to a boolean to store the success/failure status.
//Return  : The generated pressure value (0 to 11000).
//Notes   : Simulated via rand(); depends on PressureLowerBound and PressureUpperBound
//************************************************************************************** 
int32_t sReadPressure(bool *pReadStatus)
{
    if(pReadStatus == NULL)
    {
        return DATA_ERROR;
    }
    static int32_t sPressure;
    sPressure = (rand() % 11001);                                        /* Generate Random values from 0 to 11000 */
    if(sPressure < PressureLowerBound|| sPressure > PressureUpperBound)  /* To check pressure values are out of bound */
    {
        *pReadStatus = false;
    }
    else
    {
        *pReadStatus = true;
    }
    return sPressure;
}

//******************************.sReadConfigVersion.******************************** 
//Purpose : To read configuration version one time
//Inputs  : pBuffer : Pointer buffer which stores the Configuration Version
//Notes   : Configuration version read only once 
//********************************************************************************** 
void sReadConfigVersion(char *pBuffer)
{
    if(pBuffer == NULL)
    {
        printf("Failed to access Buffer\n");
    }
    strcpy(pBuffer,"1234");
    pBuffer[4] = '\0';
    printf("Configuration Version : %s\n", pBuffer);
}

//******************************.Polling Thread.********************************** 
//Purpose : Periodically poll sensor data as per requirements
//Notes   : Temperature and pressure have different polling intervals 
//********************************************************************************
void* PollingThread(void *arg)
{
    (void)arg;
    while (1)
    {
        uint64_t ullCurrentTime = GetTimeMs();
        static bool ucReadStatus = true;
        for (int i = 0; i < (int)POLL_TABLE_SIZE; i++)
        {
            PollingConfig_t *Configuration = &gPollingTable[i];

            /****************************************Poll Once****************************************/

            if((ullCurrentTime - Configuration->m_ullLastPollTime) >= Configuration->m_ullPollIntervalMs)
            {
                if(Configuration->m_ullPollIntervalMs == 0)
                {
                    if(Configuration->m_ulPolledOnce)
                    {
                        continue;
                    }
                    SensorResult_t PolledSensorData;
                    Read_Data_Status_t eDataStatus;
                    PolledSensorData.m_eType = Configuration->m_eType;
                    PolledSensorData.m_eParam = Configuration->m_eParam;
                    if(Configuration->m_eType == PARAM_TYPE_STRING)
                    {
                        Configuration->m_ReadFn.pfnReadstr(PolledSensorData.m_Value.StringValue);
                    }
                    else
                    {
                        PolledSensorData.m_Value.lIntValue = Configuration->m_ReadFn.pfnReadInt(&ucReadStatus);
                        if(PolledSensorData.m_Value.lIntValue == DATA_ERROR)
                        {
                            printf("Failed to read Sensor due to invalid status pointer\n");
                        }
                    }
                    if(ucReadStatus == false)
                    {
                        if(PolledSensorData.m_eParam == PARAM_TEMP)
                        {
                            printf("Temperature value is not within required range\n");
                        }
                        else if(PolledSensorData.m_eParam == PARAM_PRESSURE)
                        {
                            printf("Pressure value is not within required range\n");
                        }
                        else
                        {
                            printf("Invalid Data\n");
                        }
                    }
                    eDataStatus = SetPolledValue(Configuration->m_eParam,&PolledSensorData);
                    if(eDataStatus == DATA_ERROR)
                    {
                        printf("Invalid Parameter or Read error\n");
                    }
                    else if(eDataStatus == DATA_ALLOCATION_FAILED)
                    {
                        printf("Memory Allocation failed for polling data\n");
                    }
                    Configuration->m_ulPolledOnce = 1;
                    Configuration->m_ullLastPollTime = ullCurrentTime;
                    continue;
                }

            /****************************************Periodic Poll****************************************************/

                else
                {
                    SensorResult_t PolledSensorData;
                    Read_Data_Status_t eDataStatus;
                    PolledSensorData.m_eType = Configuration->m_eType;
                    PolledSensorData.m_eParam = Configuration->m_eParam;
                    if(Configuration->m_eType == PARAM_TYPE_INT)
                    {
                        PolledSensorData.m_Value.lIntValue = Configuration->m_ReadFn.pfnReadInt(&ucReadStatus);
                        if(PolledSensorData.m_Value.lIntValue == DATA_ERROR)
                        {
                            printf("Failed to read Sensor due to invalid status pointer\n");
                        }
                        if(ucReadStatus == false)
                        {
                            if(PolledSensorData.m_eParam == PARAM_TEMP)
                            {
                                printf("Temperature value is not within required range\n");
                            }
                            else if(PolledSensorData.m_eParam == PARAM_PRESSURE)
                            {
                                printf("Pressure value is not within required range\n");
                            }
                            else
                            {
                                printf("Invalid Data\n");
                            }
                        }
                        eDataStatus = SetPolledValue(Configuration->m_eParam,&PolledSensorData);
                        if(eDataStatus == DATA_ERROR)
                        {
                            printf("Invalid Parameter or Read error\n");
                        }
                        else if(eDataStatus == DATA_ALLOCATION_FAILED)
                        {
                            printf("Memory Allocation failed for polling data\n");
                        }
                        Configuration->m_ullLastPollTime = ullCurrentTime;
                    } 
                } 
            }
        }
        usleep(10000);/* 10Ms */
    }
    return NULL;
}