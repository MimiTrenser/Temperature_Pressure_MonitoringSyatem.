#include "sensor.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>


static int32_t readTemperature(int8_t *out)
{
    static int32_t Temp; 
    Temp = (-20 + rand() % 141);
    if(Temp < -20 || Temp > 100)
    {
        *out = 1;
    }
    else
    {
        *out = 0;
    }
    return Temp; 
}

static int32_t readPressure(int8_t *out)
{
    static int32_t Pressure;
    Pressure = (rand() % 11001);
    if(Pressure < 0 || Pressure > 10000)
    {
        *out = 1;
    }
    else
    {
        *out = 0;
    }
    return Pressure;
}

static void readConfigVersion(char *buf)
{
    strcpy(buf,"1234");
    buf[4] = '\0';
    printf("Config Ver : %s\n",buf);
}

PollingConfig_t polling_table[] = {{PARAM_TEMP,50,PARAM_TYPE_INT,.read_fn.readInt_fn = readTemperature,0,0},
                                    {PARAM_PRESSURE,200,PARAM_TYPE_INT,.read_fn.readInt_fn = readPressure,0,0},
                                    {PARAM_CONFIG_VERSION,0,PARAM_TYPE_STRING,.read_fn.readstr_fn = readConfigVersion,0,0}};

#define SizeofPollTable (sizeof(polling_table)/sizeof(PollingConfig_t))

void* PollingThread(void *arg)
{
    (void)arg;
    while (1)
    {
        int64_t now = getTimeMs();
        static int8_t out = 0;
        for(int i = 0;i < (int)SizeofPollTable;i++)
        {
            PollingConfig_t *config = &polling_table[i];

            /*............................Poll Once.......................................*/
            if((now - config->lastpoll_time) >= config->PollInterval_ms)
            {
                if(config->PollInterval_ms == 0)
                {
                    if(config->polled_once)
                    {
                        continue;
                    }
                    SensorResult Result;
                    Result.type = config->type;
                    if(config->type == PARAM_TYPE_STRING)
                    {
                        config->read_fn.readstr_fn(Result.value.str_val);
                    }
                    else
                    {
                        Result.value.int_val = config->read_fn.readInt_fn(&out);
                    }
                    SetPolledValue(config->Param,&Result);
                    config->polled_once = 1;
                    config->lastpoll_time = now;
                    continue;
                }

            /*............................Periodic Poll.......................................*/
                else
                {
                    SensorResult Result;
                    Result.type = config->type;
                    if(config->type == PARAM_TYPE_INT)
                    {
                        Result.value.int_val = config->read_fn.readInt_fn(&out);
                        Result.Read_Status = out;
                        SetPolledValue(config->Param,&Result);
                        config->lastpoll_time = now;
                    } 
                } 
            }
        }
        usleep(10000);//10ms
    }
    return NULL;
}