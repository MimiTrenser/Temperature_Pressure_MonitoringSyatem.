//**************************** Temparature Pressure Monitoring System ************************************** 
//  Copyright (c) 2021 Trenser 
//  All Rights Reserved 
//********************************************************************************************************** 
// 
// File    : FileName.cpp 
// Summary : Process.c file executes Process Thread.Process the Temperarture and
//           pressure values as per requirement
// Note    : 
// Author  : Mimi C.S
// Date    : 19/12/2025
// 
//**********************************************************************************************************
#include "database.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

ProcessConfig_t gProcessTable[] = {{PARAM_TEMP, 200, -10, 70, 400, 0, 0},
                                   {PARAM_PRESSURE, 400, 500, 6000, 800, 0, 0}};

#define PROCESS_TABLE_SIZE (sizeof(gProcessTable) / sizeof(ProcessConfig_t))

//******************************.SendNotificationTemperature.************************************************
//Purpose : Print function to indicate Temperature violated threshold value
//Inputs  :  int value - Violated Temperature Value.
//Notes   : Threshold for temperarture is -10 to 20
//***********************************************************************************************************
static void SendNotificationTemperature(int value)
{
    printf("Temperature : %d -> Temperature Violated Threshold Values\n", value);
}

//******************************.SendNotificationPressure.***************************************************
//Purpose : Print function to indicate Pressure violated threshold value
//Inputs  :  int value - Violated Pressure Value.
//Notes   : Threshold for pressure is 400 to 6000
//***********************************************************************************************************
static void SendNotificationPressure(int value)
{
    printf("Pressure: %d -> Pressure Violated Threshold Values\n", value);
}

//******************************.ProcessTemperatureAction.***************************************************
//Purpose : Print Temperature value
//Inputs  :  int32_t value - Temperature Value.
//***********************************************************************************************************
static void ProcessTemperatureAction(int32_t value)
{

    printf("Temperature: %d\n", value);
}

//******************************.ProcessPressureAction.******************************************************
//Purpose : Print  Pressure value
//Inputs  :  int32_t value - Pressure Value.
//***********************************************************************************************************
static void ProcessPressureAction(int32_t value)
{
    printf("Pressure: %d\n", value);
}

//******************************.ProcessingThread.************************************************************ 
//Purpose : Process the sesor data with the requirements
//Notes   : It is a Process Thread which periodically process sensor data as per 
//          requirements 
//************************************************************************************************************
void* ProcessingThread(void *arg)
{
    (void)arg;

    while (1)
    {
        for (int i = 0; i < (int)PROCESS_TABLE_SIZE; i++)
        {
            int64_t llCurrentTime = GetTimeMs();
            SensorResult_t ProcessSensorData;
            Read_Data_Status_t eDataStatus;
            ProcessConfig_t *Configuration = &gProcessTable[i];
            ProcessSensorData.m_eParam = Configuration->m_eParam;
            if((llCurrentTime - Configuration->m_ullLastProcessTime) >= (Configuration->m_ullProcessIntervalMs))
            {
                eDataStatus = GetPolledValue(Configuration->m_eParam, &ProcessSensorData);
                if(eDataStatus == DATA_ERROR)
                {
                    printf("Invalid Parameter Type or Data is not present\n");
                }
                else
                {
                if(ProcessSensorData.m_Value.lIntValue < Configuration->m_lMiniThreshold || ProcessSensorData.m_Value.lIntValue > Configuration->m_llMaxThreshold)
                {
                    Configuration->m_ulViolationTime += Configuration->m_ullProcessIntervalMs;/* Violation time increments by ProcessIntervalMs in each violation*/
                }
                else
                {
                    if(ProcessSensorData.m_eParam == PARAM_TEMP)
                    {
                        ProcessTemperatureAction(ProcessSensorData.m_Value.lIntValue);
                    }
                    else if(ProcessSensorData.m_eParam == PARAM_PRESSURE)
                    {
                        ProcessPressureAction(ProcessSensorData.m_Value.lIntValue);
                    }
                    else
                    {
                        printf("Invalid Parameter Type or Data is not present\n");
                    }
                    Configuration->m_ulViolationTime = 0;

                }
                if(Configuration->m_ulViolationTime >= Configuration->m_ulSamplingTime)/*If violation time exceeds sampling time Notification sent*/
                {
                    if(ProcessSensorData.m_eParam == PARAM_TEMP)
                    {
                        SendNotificationTemperature(ProcessSensorData.m_Value.lIntValue);
                    }
                    else if(ProcessSensorData.m_eParam == PARAM_PRESSURE)
                    {
                        SendNotificationPressure(ProcessSensorData.m_Value.lIntValue);
                    }
                    else
                    {
                        printf("Invalid Parameter Type or Data is not present\n");
                    }
                    Configuration->m_ulViolationTime = 0;
                }
                Configuration->m_ullLastProcessTime = llCurrentTime;
            }
            }
            else
            {
                /* Nothing to do */
            }
        }
        usleep(50000);/*50Ms*/
    }
    return NULL;
}