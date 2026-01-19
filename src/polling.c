//****************** Temparature Pressure Monitoring System ********************
// Copyright (c) 2026 Trenser Technology Solutions (P) Ltd
// All Rights Reserved
//******************************************************************************
//
// File     : Polling.c
// Summary  : Polling.c file includes execution of Polling Thread, Read
//            Temperature and Pressure sensors
// Note     : None
// Author   : Mimi C.S
// Date     : 19/12/2025
//
//******************************************************************************

//***************************** Include Files **********************************
#include "database.h"

//***************************** Local Constants ********************************
#define TEMPERATURE_LOWERBOUND              (-20)
#define TEMPERATURE_UPPERBOUND              (100)
#define PRESSURE_LOWERBOUND                 (0)
#define PRESSURE_UPPERBOUND                 (10000)
#define ZERO                                (0)
#define RAND_RANGE_TEMPERATURE              (141)
#define RAND_RANGE_PRESSURE                 (11001)
#define BUFFER_SIZE                         (4)
#define POLLING_INTERVAL_TEMPERATURE        (50)
#define POLLING_INTERVAL_PRESSURE           (200)
#define POLLING_INTERVAL_CONFIG_VERSION     (0)
#define LAST_POLLTIME_TEMPERATURE           (0)
#define LAST_POLLTIME_PRESSURE              (0)
#define LAST_POLLTIME_CONFIG_VERSION        (0)
#define POLLED_ONCE_TEMPERATURE             (0)
#define POLLED_ONCE_PRESSURE                (0)
#define POLLED_ONCE_CONFIG_VERSION          (0)


//***************************** Local Types ************************************

//***************************** Local Variables ********************************

//***************************** gPollingTable **********************************
//Purpose   : Parses the Configuration parameters of Temperature,Pressure &
//            Configversion.
//Notes     : None
//******************************************************************************
POLLING_CONFIG_t gPollingTable[] = {
                                        {
                                            PARAM_TEMP,
                                            POLLING_INTERVAL_TEMPERATURE,
                                            PARAM_TYPE_INT,
                                            .m_READFN.pfnReadInt = readTemperature,
                                            LAST_POLLTIME_TEMPERATURE,
                                            POLLED_ONCE_TEMPERATURE
                                        },
                                        {
                                            PARAM_PRESSURE,
                                            POLLING_INTERVAL_PRESSURE,
                                            PARAM_TYPE_INT,
                                            .m_READFN.pfnReadInt = readPressure,
                                            LAST_POLLTIME_PRESSURE,
                                            POLLED_ONCE_PRESSURE
                                        },
                                        {
                                            PARAM_CONFIG_VERSION,
                                            POLLING_INTERVAL_CONFIG_VERSION,
                                            PARAM_TYPE_STRING,
                                            .m_READFN.pfnReadstr = readConfigVersion,
                                            LAST_POLLTIME_CONFIG_VERSION,
                                            POLLED_ONCE_CONFIG_VERSION
                                        }
                                    };

#define POLL_TABLE_SIZE (sizeof(gPollingTable) / sizeof(POLLING_CONFIG_t))

//****************************** Local Functions *******************************

//****************************** ReadTemperature *******************************
//Purpose   : Generates a simulated Temperature reading and validates it against
//            bounds.
//Inputs    : pblReadStatus - Pointer to a boolean to store the success/failure
//            status.
//Outputs   : Read Temperature value and validate the value
//Return    : The generated temperature value (-20 to 120).
//Notes     : Simulated via rand(); depends on TemperatureLowerBound and 
//            TemperatureUpperBound
//******************************************************************************
int32_t readTemperature(bool *pblReadStatus)
{
    if(pblReadStatus == NULL)
    {
        return DATA_ERROR;
    }

    static int32_t slTemperature = 0;

    /* Genereate random values from 0 to 120 */
    slTemperature = (TEMPERATURE_LOWERBOUND + rand() % RAND_RANGE_TEMPERATURE);

    if(slTemperature < TEMPERATURE_LOWERBOUND ||
        slTemperature > TEMPERATURE_UPPERBOUND)
    {
        *pblReadStatus = false;
    }
    else
    {
        *pblReadStatus = true;
    }
    return slTemperature;
}

//****************************** sReadPressure *********************************
//Purpose : Generates a simulated pressure reading and validates it against
//          bounds.
//Inputs  : pblReadStatus - Pointer to a boolean to store the success/failure
//          status.
//Outputs : Read Pressure value and validate the value 
//Return  : The generated pressure value (0 to 11000).
//Notes   : Simulated via rand(); depends on PressureLowerBound and
//          PressureUpperBound
//******************************************************************************
int32_t readPressure(bool *pblReadStatus)
{
    if(pblReadStatus == NULL)
    {
        return DATA_ERROR;
    }

    static int32_t sPressure = 0;

    /* Generate Random values from 0 to 11000 */
    sPressure = (rand() % RAND_RANGE_PRESSURE);
                                           
    if(sPressure < PRESSURE_LOWERBOUND || sPressure > PRESSURE_UPPERBOUND)
    {
        *pblReadStatus = false;
    }
    else
    {
        *pblReadStatus = true;
    }

    return sPressure;
}

//****************************** sReadConfigVersion ****************************
//Purpose   : To read configuration version one time
//Inputs    : pBuffer : Pointer buffer which stores the Configuration Version
//Outputs   : Read Configuration version
//Return    : None
//Notes     : Configuration version read only once
//******************************************************************************
void readConfigVersion(char *pBuffer)
{
    if(pBuffer == NULL)
    {
        printf("Failed to access Buffer\n");
    }
    else
    {
        strcpy(pBuffer,"1234");
        pBuffer[BUFFER_SIZE] = '\0';
        printf("Configuration Version : %s\n", pBuffer);
    }
}

//******************************.Polling Thread.********************************
//Purpose : Periodically poll sensor data as per requirements
//Inputs  : None 
//Outputs : Periodic eading of temperature and pressure sensor
//Return  : None
//Notes   : Temperature and pressure have different polling intervals
//******************************************************************************
void* pollingThread(void *arg)
{
    (void)arg;

    while (1)
    {
        uint64_t ullCurrentTime = getTimeMs();
        static bool blReadStatus = true;
        volatile int32_t lReadValue = ZERO;

        for (int i = ZERO; i < (int)POLL_TABLE_SIZE; i++)
        {
            POLLING_CONFIG_t *Configuration = &gPollingTable[i];

            /*************************** Poll Once*****************************/

            if((ullCurrentTime - Configuration->m_ullLastPollTime) >= Configuration->m_ullPollIntervalMs)
            {
                if(Configuration->m_ullPollIntervalMs == 0)
                {
                    if(Configuration->m_ulPolledOnce)
                    {
                        continue;
                    }
                    SENSOR_RESULT_t PolledSensorData = {0};
                    READ_DATA_STATUS_t eDataStatus;
                    PolledSensorData.m_eType = Configuration->m_eType;
                    PolledSensorData.m_eParam = Configuration->m_eParam;

                    if(Configuration->m_eType == PARAM_TYPE_STRING)
                    {
                        Configuration->m_READFN.pfnReadstr(PolledSensorData.m_VALUE.cStringValue);
                    }
                    else
                    {
                        lReadValue = Configuration->m_READFN.pfnReadInt(&blReadStatus);
                    }

                    if(blReadStatus == false)
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
                    else
                    {
                        PolledSensorData.m_VALUE.lIntValue = lReadValue;
                    }

                    eDataStatus = sensorPolledValueSet(Configuration->m_eParam, &PolledSensorData);

                    if(eDataStatus == DATA_ERROR)
                    {
                        printf("Invalid Parameter or Read error\n");
                    }
                    else if(eDataStatus == DATA_ALLOCATION_FAILED)
                    {
                        printf("Memory Allocation failed for polling data\n");
                    }
                    else
                    {
                        /* Nothing to do */
                    }

                    Configuration->m_ulPolledOnce = 1;
                    Configuration->m_ullLastPollTime = ullCurrentTime;
                    continue;
                }

            /*************************** Periodic Poll*************************/

                else
                {
                    SENSOR_RESULT_t PolledSensorData = {0};
                    READ_DATA_STATUS_t eDataStatus;
                    PolledSensorData.m_eType = Configuration->m_eType;
                    PolledSensorData.m_eParam = Configuration->m_eParam;

                    if(Configuration->m_eType == PARAM_TYPE_INT)
                    {
                        lReadValue = Configuration->m_READFN.pfnReadInt(&blReadStatus);

                        if(blReadStatus == false)
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
                        else
                        {
                            PolledSensorData.m_VALUE.lIntValue = lReadValue;
                        }
                        eDataStatus = sensorPolledValueSet(Configuration->m_eParam, &PolledSensorData);

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
                    else
                    {
                        /* Do Nothing */
                    }
                } 
            }
        }
        usleep(10000);/* 10Ms */
    }

    return NULL;
}