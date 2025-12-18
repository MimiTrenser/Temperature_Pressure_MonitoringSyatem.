#include "sensor.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

static void sendNotification_Temp(int value)
{
    printf("Temperature : %d -> Temperature Violated Threshold Values\n",value);
}

static void sendNotification_Pressure(int value)
{
    printf("Pressure : %d -> Pressure Violated Threshold Values\n",value);
}

static void processTempAction(int32_t value)
{

    printf("Temperature: %d\n", value);
}

static void processPressureAction(int32_t value)
{
    printf("Pressure: %d\n", value);
}


ProcessConfig_t process_table[] = {{PARAM_TEMP,200,-10,70,400,0,0},
                                    {PARAM_PRESSURE,400,500,6000,800,0,0}};

#define SizeofProcessTable (sizeof(process_table)/sizeof(ProcessConfig_t))

void* ProcessingThread(void *arg)
{
    (void)arg;

    while (1)
    {
        int64_t now = getTimeMs();
        for(int i = 0;i < (int)SizeofProcessTable ;i++)
        {
            SensorResult data;
            ProcessConfig_t *config = &process_table[i];
            if((now - config->lastProcessTime) >= (config->processInterval_ms))
            {
                GetPolledValue(config->Param,&data);
                if(data.Read_Status == 1)
                {
                    if(config->Param == PARAM_TEMP)
                    {
                        printf("Temperature value is not within required range\n");
                    }
                    if(config->Param == PARAM_PRESSURE)
                    {
                        printf("Pressure value is not within required range\n");
                    }
                }
                if(data.value.int_val < config->mini_threshold || data.value.int_val > config->max_threshold)
                {
                    config->violationtime += config->processInterval_ms;
                }
                else
                {
                    if(config->Param == PARAM_TEMP)
                    {
                        processTempAction(data.value.int_val);
                    }
                    if(config->Param == PARAM_PRESSURE)
                    {
                        processPressureAction(data.value.int_val);
                    }
                    config->lastProcessTime = now;
                    config->violationtime = 0;

                }
                if(config->violationtime >= config->samplingtime)
                {
                    if(config->Param == PARAM_TEMP)
                    {
                        sendNotification_Temp(data.value.int_val);
                    }
                    if(config->Param == PARAM_PRESSURE)
                    {
                        sendNotification_Pressure(data.value.int_val);
                    }
                    config->lastProcessTime = now;
                    config->violationtime = 0;
                }
            }
        }
        usleep(50000);//50ms
    }
    return NULL;
}