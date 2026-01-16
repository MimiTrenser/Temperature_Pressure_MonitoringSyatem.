//******************** Temparature Pressure Monitoring System ******************
// Copyright (c) 2026 Trenser Technology Solutions (P) Ltd
// All Rights Reserved
//******************************************************************************
//
// File    : process.c
// Summary : Process.c file executes Process Thread.Process the Temperarture and
//           pressure values as per requirement
// Note    : None
// Author  : Mimi C.S
// Date    : 19/12/2025
//
//******************************************************************************

//******************************* Include Files ********************************
#include "database.h"

//***************************** Local Types ************************************ 
 
//***************************** Local Constants ******************************** 
#define PROCESS_INTERVAL_TEMPERATURE        (200)
#define PROCESS_INTERVAL_PRESSURE           (400)
#define MINIMUM_THRESHOLD_TEMPERATURE       (-10)
#define MINIMUM_THRESHOLD_PRESSURE          (500)
#define MAXIMUM_THRESHOLD_TEMPERATURE       (70)
#define MAXIMUM_THRESHOLD_PRESSURE          (6000)
#define SAMPLING_TIME_TEMPERATURE           (400)
#define SAMPLING_TIME_PRESSURE              (800)
#define VIOLATION_TIME_TEMPERATURE          (0)
#define VIOLATION_TIME_PRESSURE             (0)
#define LAST_PROCESS_TIME_TEMPERATURE       (0)
#define LAST_PROCESS_TIME_PRESSURE          (0)

//***************************** Local Variables ********************************

PROCESS_CONFIG_t gProcessTable[] = {{PARAM_TEMP, PROCESS_INTERVAL_TEMPERATURE, MINIMUM_THRESHOLD_TEMPERATURE, MAXIMUM_THRESHOLD_TEMPERATURE, SAMPLING_TIME_TEMPERATURE, VIOLATION_TIME_TEMPERATURE, LAST_PROCESS_TIME_TEMPERATURE},
                                   {PARAM_PRESSURE, PROCESS_INTERVAL_PRESSURE, MINIMUM_THRESHOLD_PRESSURE, MAXIMUM_THRESHOLD_PRESSURE, SAMPLING_TIME_PRESSURE, VIOLATION_TIME_PRESSURE, LAST_PROCESS_TIME_PRESSURE}};

#define PROCESS_TABLE_SIZE (sizeof(gProcessTable) / sizeof(PROCESS_CONFIG_t))

//**************************** Local Functions *********************************
static void sendNotificationTemperature(int value);
static void sendNotificationPressure(int value);
static void processTemperatureAction(int32_t value);
static void processPressureAction(int32_t value);

//**************************** sendNotificationTemperature *********************
//Purpose : Print function to indicate Temperature violated threshold value
//Inputs  : int value - Violated Temperature Value.
//Outputs : Log temperature violation
//Return  : Void return
//Notes   : Threshold for temperarture is -10 to 20
//******************************************************************************
static void sendNotificationTemperature(int value)
{
    printf("Temperature: %d -> Temperature Violated Threshold Values\n", value);
}

//****************************** SendNotificationPressure **********************
//Purpose : Print function to indicate Pressure violated threshold value
//Inputs  : int value - Violated Pressure Value.
//Outputs : Log pressure violation
//Return  : Void return
//Notes   : Threshold for pressure is 400 to 6000
//******************************************************************************
static void sendNotificationPressure(int value)
{
    printf("Pressure: %d -> Pressure Violated Threshold Values\n", value);
}

//****************************** ProcessTemperatureAction **********************
//Purpose : Print Temperature value
//Inputs  : int32_t value - Temperature Value.
//Outputs : Log Temperature
//Return  : Void return
//******************************************************************************
static void processTemperatureAction(int32_t value)
{

    printf("Temperature: %d\n", value);
}

//****************************** ProcessPressureAction *************************
//Purpose : Print  Pressure value
//Inputs  : int32_t value - Pressure Value.
//Outputs : Log pressure 
//Return  : Void return
//******************************************************************************
static void processPressureAction(int32_t value)
{
    printf("Pressure: %d\n", value);
}

//****************************** ProcessingThread ******************************
//Purpose : Process the sesor data with the requirements
//Notes   : It is a Process Thread which periodically process sensor data as per
//          requirements
//Return  : Void return
//******************************************************************************
void* processingThread(void *arg)
{
    (void)arg;

    while (1)
    {
        for (int i = 0; i < (int)PROCESS_TABLE_SIZE; i++)
        {
            int64_t llCurrentTime = GetTimeMs();
            SENSOR_RESULT_t ProcessSensorData = {0};
            READ_DATA_STATUS_t eDataStatus;
            PROCESS_CONFIG_t *Configuration = &gProcessTable[i];
            ProcessSensorData.m_eParam = Configuration->m_eParam;

            if((llCurrentTime - Configuration->m_ullLastProcessTime) >= (Configuration->m_ullProcessIntervalMs))
            {
                eDataStatus = sensorPolledValueGet(Configuration->m_eParam, &ProcessSensorData);
                if(eDataStatus == DATA_ERROR)
                {
                    printf("Invalid Parameter Type or Data is not present\n");
                }
                else
                {
                if(ProcessSensorData.m_VALUE.lIntValue < Configuration->m_lMiniThreshold ||
                   ProcessSensorData.m_VALUE.lIntValue > Configuration->m_llMaxThreshold)
                {
                    /* Violation time increments by ProcessIntervalMs in each violation*/
                    Configuration->m_ulViolationTime += Configuration->m_ullProcessIntervalMs;
                }
                else
                {
                    if(ProcessSensorData.m_eParam == PARAM_TEMP)
                    {
                        processTemperatureAction(ProcessSensorData.m_VALUE.lIntValue);
                    }
                    else if(ProcessSensorData.m_eParam == PARAM_PRESSURE)
                    {
                        processPressureAction(ProcessSensorData.m_VALUE.lIntValue);
                    }
                    else
                    {
                        printf("Invalid Parameter Type or Data is not present\n");
                    }
                    Configuration->m_ulViolationTime = 0;

                }
                /*If violation time exceeds sampling time Notification sent*/
                if(Configuration->m_ulViolationTime >= Configuration->m_ulSamplingTime)
                {
                    if(ProcessSensorData.m_eParam == PARAM_TEMP)
                    {
                        sendNotificationTemperature(ProcessSensorData.m_VALUE.lIntValue);
                    }
                    else if(ProcessSensorData.m_eParam == PARAM_PRESSURE)
                    {
                        sendNotificationPressure(ProcessSensorData.m_VALUE.lIntValue);
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