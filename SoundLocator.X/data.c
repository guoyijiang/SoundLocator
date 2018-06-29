
#include "data.h"
ST_DATA data = 
{
    .processState = STATE_IDLE,

    .mk_record = {0},
    .mk_delta_cnt = {0},
 
    .arrive_order = {MK_ERROR},
    .mk_ave_delay = {0},
    
    .degree = 0,
    .distance = 0
};

ST_SETTING setting =
{
    .maxWaitCnt = 100000,
    .maxDelayCnt = 6000,
    .maxDelayCntShake = 100,
    .minDeltaCnt = 0,
    .maxDeltaCnt = 16000,
};

ST_SETTING *pSetting = &setting;
ST_DATA *pData = &data;