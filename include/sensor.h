#ifndef SENSOR_SYSTEM_H
#define SENSOR_SYSTEM_H

#include <stdint.h>
#include <sys/time.h>

#define STR_LEN 5

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
}paramType_t;

typedef struct
{
    ParamId_t Param;
    uint64_t PollInterval_ms;
    paramType_t type;
    union 
    {
        int32_t (*readInt_fn)(int8_t *out);
        void (*readstr_fn)(char *buf);
    }read_fn;
    uint64_t lastpoll_time;
    uint8_t polled_once;
} PollingConfig_t;

typedef struct
{
    ParamId_t Param;
    uint64_t processInterval_ms;
    int32_t mini_threshold;
    int64_t max_threshold;
    uint32_t samplingtime;
    uint32_t violationtime;
    uint64_t lastProcessTime;
}ProcessConfig_t;

typedef struct
{
    paramType_t type;
    uint8_t Read_Status;
    union 
    {
        int32_t int_val;
        char str_val[STR_LEN+1];
    }value;
}SensorResult;


long long getTimeMs();
void SetPolledValue(ParamId_t id, const SensorResult *data);
void GetPolledValue(ParamId_t id,SensorResult *data);

void* PollingThread(void *arg);
void* ProcessingThread(void *arg);

#endif