#include <string.h>
#include <stdio.h>
#include "log.h"
#include "debug.h"
#include "protocol.h"

#include "led.h"
#include "data.h"

#define MAX_CMD_LENGTH (16)
#define MAX_CMD_NUMBER  (32)

typedef struct
{
    unsigned char cmd[MAX_CMD_LENGTH];
    CMD_ACTION  action;
}CMD_MAP;

static int cmd_pwm(const unsigned char* cmdString, unsigned short length)
{
    int i = 0;
    
    sscanf((const char *)cmdString, "pwm:%d", &i);
    if(i > 1024)
    {
        i = 1024;
    }
    else if(i<0)
    {
        i =0;
    }
    LOG_DEBUG("set pwm to %d", i);
    led_state_setLevel(i);
    return 0;
}

extern int indexXX;
static int cmd_test(const unsigned char* cmdString, unsigned short length)
{

    sscanf((const char *)cmdString, "test:%d", &indexXX);
    LOG_DEBUG("change indexXX:%d", indexXX);
}

static int cmd_log_setting(const unsigned char* cmdString, unsigned short length)
{
    print("hese is setting:\r\n");   
    print("maxWaitCnt:%d\r\n", setting.maxWaitCnt);
    print("maxDelayCnt:%d\r\n", setting.maxDelayCnt);
    print("maxDelayCntShake:%d\r\n", setting.maxDelayCntShake);
    print("minDeltaCnt:%d\r\n", setting.minDeltaCnt);
    print("maxDeltaCnt:%d\r\n", setting.maxDeltaCnt);

    return 0;
}

static int cmd_set_maxWaitCnt(const unsigned char* cmdString, unsigned short length)
{
    int rc = 0;
    rc = sscanf((const char *)cmdString, "set maxWaitCnt %d", &setting.maxWaitCnt);
    if(1 == rc)
    {
        LOG_DEBUG("SUCCESS:set maxWaitCnt to %d", setting.maxWaitCnt);
    }
    else
    {
        LOG_ERROR("FAIELD:maxWaitCnt is %d", setting.maxWaitCnt);
    }

    return 0;
}
static int cmd_set_maxDelayCnt(const unsigned char* cmdString, unsigned short length)
{
    int rc = 0;
    rc = sscanf((const char *)cmdString, "set maxDelayCnt %d", &setting.maxDelayCnt);
    if(1 == rc)
    {
        LOG_DEBUG("SUCCESS:set maxDelayCnt to %d", setting.maxDelayCnt);
    }
    else
    {
        LOG_ERROR("FAIELD:maxDelayCnt is %d", setting.maxDelayCnt);
    }

    return 0;
}

static int cmd_set_maxDelayCntShake(const unsigned char* cmdString, unsigned short length)
{
    int rc = 0;
    rc = sscanf((const char *)cmdString, "set maxShake %d", &setting.maxDelayCntShake);
    if(1 == rc)
    {
        LOG_DEBUG("SUCCESS:set maxShake to %d", setting.maxDelayCntShake);
    }
    else
    {
        LOG_ERROR("FAIELD:set maxShake is %d", setting.maxDelayCntShake);
    }

    return 0;
}

static int cmd_set_minDeltaCnt(const unsigned char* cmdString, unsigned short length)
{
    int rc = 0;
    rc = sscanf((const char *)cmdString, "set minDeltaCnt %d", &setting.minDeltaCnt);
    if(1 == rc)
    {
        LOG_DEBUG("SUCCESS:set minDeltaCnt to %d", setting.minDeltaCnt);
    }
    else
    {
        LOG_ERROR("FAIELD:minDeltaCnt is %d", setting.minDeltaCnt);
    }

    return 0;
}

static int cmd_set_maxDeltaCnt(const unsigned char* cmdString, unsigned short length)
{
    int rc = 0;
    rc = sscanf((const char *)cmdString, "set maxDeltaCnt %d", &setting.maxDeltaCnt);
    if(1 == rc)
    {
        LOG_DEBUG("SUCCESS:set maxDeltaCnt to %d", setting.maxDeltaCnt);
    }
    else
    {
        LOG_ERROR("FAIELD:maxDeltaCnt is %d", setting.maxDeltaCnt);
    }

    return 0;
}

static CMD_MAP cmd_map[MAX_CMD_NUMBER] =
{
        {"pwm",                     cmd_pwm},
        {"test",                    cmd_test},

        {"log setting",             cmd_log_setting},
        {"set maxWaitCnt",           cmd_set_maxWaitCnt},
        {"set maxDelayCnt",         cmd_set_maxDelayCnt},
        {"set maxShake",       cmd_set_maxDelayCntShake},
        {"set minDeltaCnt",         cmd_set_minDeltaCnt},
        {"set maxDeltaCnt",         cmd_set_maxDeltaCnt},            
};

int debug_proc(const unsigned char* cmdString, unsigned short length)
{
    int i = 0;
    const unsigned char* cmd = string_trimLeft(cmdString);

    for (i = 0; i < MAX_CMD_NUMBER && cmd_map[i].action; i++)
    {
        if (strncmp((const char *)cmd, (const char *)cmd_map[i].cmd, strlen((const char *)cmd_map[i].cmd)) == 0)
        {
            return cmd_map[i].action(cmdString, length);
        }
    }

    LOG_INFO("CMD not processed");
    return -1;
}