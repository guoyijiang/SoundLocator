#include <plib.h>
#include "data.h"
#include "led.h"
#include "log.h"
#include <math.h>
#include <stdbool.h>
#include "process.h"
//#include "int_ext.h"
#include "incapInst.h"

static uint8_t process_getLedNum_byDegree(uint16_t degree)
{
    uint8_t led = 0;
    if( (int)((float)(degree)/45.0 + 0.5) > 7 )
    {
        return 0;
    }
    return (int)((float)(degree)/45.0 + 0.5);
}

//调节阈值、状态灯等
void process_reConfig(void)
{
    
}

/*
#define FACTOR_T (1.0/16000000.0) //时间系数(周期)
#define FACTOR_LENGTH (FACTOR_T/340.0*1000.0) //距离系数 distance(mm) = cnt * FACTOR_LENGTH

#define BOARD_X0_CNT (2.9368393249511608098391401276485/FACTOR_LENGTH)
#define BOARD_Y0_CNT (22.307509380910734250752544355893/FACTOR_LENGTH)
#define BOARD_X1_CNT (-20.787289481503952012884121761428/FACTOR_LENGTH)
#define BOARD_Y1_CNT (-8.610377228214519863890349640684/FACTOR_LENGTH)
#define BOARD_MK_DISTANCE_CNT (77.942286340599478208735085367764/FACTOR_LENGTH)

#define DEGRE2PI(x) ((x)*180.0/PI)
#define DEGREE_SHIFT DEGRE2PI(22.5) //degree_mk1
#define DEGREE_CHANGE0(x) DEGRE2PI(x-22.5) //弧度值
#define DEGREE_CHANGE1(x) DEGRE2PI(x+115)

//#define PI (3.14159265358979323846)
bool process_dealDataxxx(void)
{
    int i = 0;
    float cnt[3];
    float c_temp = BOARD_MK_DISTANCE_CNT /2;
    float a_temp,b_temp;
    float x,y,k1,k2,ct1,ct2;

    if((data.record[1].mk == data.record[0].mk)||(data.record[1].mk == data.record[2].mk)) return false;
    
    //mk排序
    for(i = 0; i < 3; i++)
    {
        int j =0;
        for(; j<3; j++)
        {
            if(data.record[j].mk == i)
            {
                cnt[i] = data.record[j].cntT;
            }
        }
    }

    a_temp = fabs(cnt[0] - cnt[1])/2;
    if(c_temp < a_temp) return false;
    b_temp = sqrt(c_temp*c_temp - a_temp*a_temp);
    //相对偏角
    ct1 = atan2(b_temp,a_temp); // -pi~pi
    ct1 = b_temp/a_temp;
    if(cnt[0] > cnt[1]) ct1 = -1.0*ct1;

    
    a_temp = fabs(cnt[1] - cnt[2])/2;
    if(c_temp < a_temp) return false;
    b_temp = sqrt(c_temp*c_temp - a_temp*a_temp);
    //相对偏角
    ct2 = atan2(b_temp,a_temp); // -pi~pi
    ct2 = b_temp/a_temp;
    if(cnt[0] > cnt[1]) ct2 = -1.0*ct2;


    //转换为绝对偏角
    ct1 = DEGREE_CHANGE0(ct1);
    ct2 = DEGREE_CHANGE1(ct2);

    //转换为绝对斜率
    k1 = tan(ct1);
    k2 = tan(ct2);

    x = ((BOARD_Y1_CNT - BOARD_Y0_CNT) - (ct2*BOARD_X1_CNT - ct1*BOARD_X0_CNT))/(ct1 - ct2);
    y = BOARD_Y1_CNT + ct2*(x - BOARD_X0_CNT);

    //out
    data.degree = ((int)(atan2(y, x)*180.0/PI) + 360) % 360;
    data.distance = (uint32_t)(sqrt(x*x + y*y) * FACTOR_LENGTH);
    
    return true;
}
*/

bool process_dealData(void)
{
    int i = 0;
    int maxCnt = 0;
    int minCnt = 0;
    int delayTmp[MK_FIFO_DEEP] = {0};
    int aveDelayTmp = 0;
    
    //dump cnt 
    for(i = 0 ; i<MK_ERROR; i++)
    {
        int rc = 0;
        rc = incap_dumpData( i, data.mk_record[i]);
        if(rc != MK_FIFO_DEEP)
        {
            LOG_ERROR("mk%d fifo error, real cnt:%d", i, rc);
            return false;
        }
    }

    //calc delta
    for(i = 0 ; i<MK_ERROR; i++)
    {
        int j = 0;
        for(j = 0 ; j<MK_FIFO_DEEP-1; j++)
        {
            data.mk_delta_cnt[i][j] = data.mk_record[i][j+1] -data.mk_record[i][j];
            if( (data.mk_delta_cnt[i][j] > setting.maxDeltaCnt) || (data.mk_delta_cnt[i][j] < setting.minDeltaCnt))
            {
                LOG_ERROR("mk%d_delta_cnt[%d] > %d,is %d", i, j, setting.maxDeltaCnt, data.mk_delta_cnt[i][j]);
                return false;
            }
        }
    }

    //sort
    maxCnt = MAX(data.mk_record[0][0], MAX(data.mk_record[1][0],data.mk_record[2][0]));
    minCnt = MIN(data.mk_record[0][0], MIN(data.mk_record[1][0],data.mk_record[2][0]));

    for(i = 0 ; i<MK_ERROR; i++)
    {
        if(data.mk_record[i][0] == maxCnt) data.arrive_order[2] = i;
    }
    for(i = 0 ; i<MK_ERROR; i++)
    {
        if(data.mk_record[i][0] == minCnt) data.arrive_order[0] = i;
    }
    for(i = 0 ; i<MK_ERROR; i++)
    {
        if((data.mk_record[i][0] != minCnt)&&(data.mk_record[i][0] != maxCnt)) data.arrive_order[1] = i;
    }

    //sLOG_DEBUG("arrive record: %d, %d, %d", data.arrive_order[0], data.arrive_order[1], data.arrive_order[2]);
    //ave delay
    for(i = 0; i<MK_ERROR-1; i++)
    {
        int j = 0;
        int tempDelay = 0;
        int deltaSum = 0;
        int maxDelay = 0;
        int minDelay = 0;
        
        for(j = 0; j< MK_FIFO_DEEP; j++)
        {
            tempDelay = data.mk_record[data.arrive_order[i+1]][j] - data.mk_record[data.arrive_order[i]][j];
            deltaSum += tempDelay;
            if(0 == j)
            {
                maxDelay = tempDelay;
                minDelay = tempDelay;
            }
            else
            {
                maxDelay = MAX(tempDelay, maxDelay);
                minDelay = MIN(tempDelay, minDelay);

            }
        }
        data.mk_ave_delay[i] = (int)((float)deltaSum/(float)MK_FIFO_DEEP);

        //平均延迟 <0 或 太大 则丢弃
        if(data.mk_ave_delay[i] < 0)
        {
            LOG_ERROR("mk_ave_delay[%d] < 0", i);
            return false;
        }
        if(data.mk_ave_delay[i] > setting.maxDelayCnt)
        {
            LOG_ERROR("mk_ave_delay[%d] > %d, is %d", i, setting.maxDelayCnt, data.mk_ave_delay[i]);
            return false;
        }

        //延迟抖动太大 则丢弃
        if(abs(maxDelay - minDelay)> setting.maxDelayCntShake)
        {
            LOG_ERROR("delay_shake[i] > %d, is %d", setting.maxDelayCntShake, abs(maxDelay - minDelay));
            return false;    
        }
            
    }
    
    return true;
}

void process_calc(void)
{
    int first, second;
    double interval, theta, angle, real_angle;
    int number;
    
    first = data.arrive_order[0];
    second = data.arrive_order[1];
    interval = data.mk_ave_delay[0];

    LOG_DEBUG("********>first:%d, second:%d, deltaCnt:%.4f", first, second, interval);
    theta = acos(interval/POINT_DISTANCE)*180/PI;
    angle = theta - 30;
    //printf("theta is %lf angle is %lf\n", theta, angle);
    if(second-first==1 || second-first==-2)
    {
        real_angle = 120*first + angle + 22.5;
    }
    else
    {
        real_angle = 120*first - angle + 22.5;
    }
    number = floor((real_angle+22.5)/45);
    //printf("真实方向约为%lf\n", real_angle);
    //printf("亮灯编号为%d\n\n", number);

    data.degree = ((int)real_angle+360)%360;
}

#define RESULT_FIFO_DEEP 2

void process_resultOut(void)
{
    static uint8_t resultFifo[RESULT_FIFO_DEEP] = {0};
    static uint8_t resultIndex = 0;
    uint8_t ledIndex = 0;
    int i = 0;
    ledIndex = process_getLedNum_byDegree(data.degree);

    resultFifo[resultIndex] = ledIndex;
    resultIndex = (++resultIndex)%RESULT_FIFO_DEEP;

    for( ; i < RESULT_FIFO_DEEP; i++)
    {
        if(resultFifo[i] != resultFifo[0]) break;
    }

    if(i == RESULT_FIFO_DEEP)
    {
        int j = 3553500;
        
        //方向指示灯
        pwm_led_run();
        led_flash_biu(ledIndex);
        while(--j);
        pwm_led_stop();

        LOG_DEBUG("4.out, degree:%d, led:%d", data.degree, ledIndex);
        //print(">>led%d<<\r\n", ledIndex);
    }
    else
    {
        int j = 655350;
        while(--j);
    }
}

void process_run(void)
{
    static unsigned int j = 0;
    bool firstArrive = false;   
    unsigned int startTime = 0;
    unsigned int waitTime = 0;
    while(1)
    {
        switch(data.processState)
        {
            case STATE_IDLE:
                //清除标志位 开始任务
                firstArrive = false; 
                startTime = 0;
                waitTime = 0;
                LOG_DEBUG(">>>>>>>>>>>>>>>>>>>>>");
                //process_clear();
                incap_inst_init();
                data.processState = STATE_WAIT_INPUT;
                break;
            case STATE_WAIT_INPUT:
                if((false == firstArrive) && (MK0_BUFFER_FULL()||MK1_BUFFER_FULL()||MK2_BUFFER_FULL()))
                {
                    firstArrive = true;
                    startTime = ReadTimer23();
                }
                else if(true == firstArrive)
                {
                    waitTime = ReadTimer23() - startTime;
                    if (waitTime > setting.maxWaitCnt)
                    {
                        uint32_t buffer[MK_FIFO_DEEP];
                        //LOG_DEBUG("i can not wait anymore");
                        data.processState = STATE_IDLE;
                        break;
                    }
                }

                if( MK0_BUFFER_FULL()&&MK1_BUFFER_FULL()&&MK2_BUFFER_FULL())
                {
                    data.processState = STATE_OVER;
                }

                break;
            case STATE_OVER:
                //1.整理数据
                if(false == process_dealData())
                {
                    LOG_DEBUG("1.data is not perfect");
                    data.processState = STATE_IDLE;
                    break;
                }

                LOG_DEBUG("1.deal data over");
                
                //2.处理数据
                process_calc();
                LOG_DEBUG("2.calc data over");
                
                //3.检测配置
                process_reConfig();
                LOG_DEBUG("3.reconfig  over");
                
                //4.输出
                process_resultOut();     
/*
                //延时
                j = 12000000;
                while(--j);
*/                
                data.processState = STATE_IDLE;
                break;
            default:
                data.processState = STATE_IDLE;
                break;
        }    
    }
}
