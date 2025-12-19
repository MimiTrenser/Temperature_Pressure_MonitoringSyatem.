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

/*..............Structure For Polling Configuration.................................*/
typedef struct
{
    ParamId_t Param;                        /*Parameters for Polling*/
    uint64_t PollInterval_ms;               /*Polling Interval in ms*/
    paramType_t type;                       /*Prameter Type -> Int or String*/
    union 
    {
        int32_t (*readInt_fn)(int8_t *out); /*Read Temp and Read Pressure*/
        void (*readstr_fn)(char *buf);      /*Read Config Version*/
    }read_fn;
    uint64_t lastpoll_time;                 /*Last poll time*/
    uint8_t polled_once;                    /*Config verion need to be polled once*/
} PollingConfig_t;

/*..............Structure For Process Configuration.................................*/
typedef struct
{
    ParamId_t Param;                        /*Parametrs for Processing*/
    uint64_t processInterval_ms;            /*Process interval in ms*/
    int32_t mini_threshold;                 /*Mini Threshold value*/
    int64_t max_threshold;                  /*Max Threshold Value*/
    uint32_t samplingtime;                  /*Sampling Time(After sampling time violation notification should sent)*/
    uint32_t violationtime;                 /*In every violation increment by process time*/
    uint64_t lastProcessTime;               /*Last proccess time */
}ProcessConfig_t;

/*..............Structure For storing polled Results .................................*/
typedef struct
{
    paramType_t type;                       /*Parameter Type -> Int or String*/
    uint8_t Read_Status;                    /*Read status -> Read failed or passed*/
    union 
    {
        int32_t int_val;
        char str_val[STR_LEN+1];
    }value;                                 /* Union for storing polled values*/
}SensorResult;


long long getTimeMs();
void SetPolledValue(ParamId_t id, const SensorResult *data);
void GetPolledValue(ParamId_t id,SensorResult *data);

void* PollingThread(void *arg);
void* ProcessingThread(void *arg);

#endif