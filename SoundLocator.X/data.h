#ifndef __DATA_H__
#define	__DATA_H__

#include <stdint.h>

/*
typedef enum  {
    false,
    true,
} bool;
*/

#define MK_FIFO_DEEP (2)

#define PI (3.1415926535897932384626433832795)
#define POINT_DISTANCE 5868.0


typedef enum 
{
    MK0 = 0x00,
    MK1,
    MK2,
    MK_ERROR,
}ENUM_MK;

typedef enum 
{
    STATE_IDLE,
    STATE_WAIT_INPUT, 
    STATE_OVER,
}ENUM_PROCESS_STATE;

typedef struct
{
    ENUM_MK mk;
    uint16_t cntT;
}ST_PULSE_RECORD;
typedef struct
{
    //state
    ENUM_PROCESS_STATE processState;

    //record
    uint32_t mk_record[3][MK_FIFO_DEEP];//原始记录
    uint16_t mk_delta_cnt[3][MK_FIFO_DEEP-1];//每个mk四次记录的delta_t

    //sort
    uint8_t  arrive_order[3];//到达顺序
    int mk_ave_delay[2];//按照到达顺序的mk之间的平均延迟
    
    //result
    uint16_t degree;//°
    uint32_t distance; //mm
    
}ST_DATA;

typedef struct
{
    int maxWaitCnt;
    int maxDelayCnt;        //mk之间的最大延迟
    int maxDelayCntShake;   //mk沿最大抖动时间 832 = 52us
    int minDeltaCnt;        //同一mk两个沿之间的最小时间差
    int maxDeltaCnt;        //同一mk两个沿之间的最大时间差
    
}ST_SETTING;

extern ST_DATA data;
extern ST_SETTING setting;

#endif	/* __DATA_H__ */

