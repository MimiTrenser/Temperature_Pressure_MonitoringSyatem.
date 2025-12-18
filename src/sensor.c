#include "sensor.h"
#include <pthread.h>
#include <string.h>
#include <time.h>
#include<unistd.h>
#include <sys/time.h>

static SensorResult GetSensorResult[PARAM_MAX];
static pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;

long long getTimeMs() 
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long milliseconds = (long long)tv.tv_sec * 1000 + (long long)tv.tv_usec / 1000;
    return milliseconds;
}

void SetPolledValue(ParamId_t id,const SensorResult *data)
{
    if(id>=PARAM_MAX)
    {
        return;
    }
    pthread_mutex_lock(&gMutex);
    GetSensorResult[id] = *data;
    pthread_mutex_unlock(&gMutex);
}

void GetPolledValue(ParamId_t id,SensorResult *data)
{
    if(id>=PARAM_MAX)
    {
        return;
    }
    pthread_mutex_lock(&gMutex);
    *data = GetSensorResult[id];
    pthread_mutex_unlock(&gMutex);
    return ;
}