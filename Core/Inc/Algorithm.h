
#ifndef __ALGORITHM_H 
#define __ALGORITHM_H

#include "base.h"
#include "head_define.h"
#include "Filter.h"
#if 0
extern float Coref[10];

/*  正弦参数  */
typedef struct _SinWave   //数组参数结构体
{
    float amp;      //峰峰值 mv（阻抗匹配之后）
    float freq;     //频率 Hz
    float phase;     //相位 o
	float period;   //周期 us
}SinWave;
extern SinWave sinwave;

/*  AD采样  */
extern float 
AD_arr[AD_N],
*pAD_arr,*pAD_arr_end,

AD_sum_arr[AD_N],
*pAD_sum_arr,*pAD_sum_arr_end
;

extern u8 
TFT_arr[AD_N],
*pTFT_arr,*pTFT_arr_end
;

void ADarr_Init(void);
    

typedef struct _ArrayParam   //数组参数结构体
{
    float max;      //最大值
    float min;      //最小值
    float tft_len;  //波形一个周期占多少像素
    float tft_cycle;
    float Vpp;   //峰峰值 mV
    float Period;//周期   us
    float Aver;  //平均值 mv
    float Rms;   //有效值 mv
}ArrayParam;
extern ArrayParam AD_Params;
void Calc_array(ArrayParam *params,float* calc_addr,u32 calc_len);    //计算数组的参数
#if 0
/*  FIR滤波器  */
#define FIR_NUM    AD_N          //FIR点数
#define BLOCK_SIZE  100                   //计算单次FIR
#define FIR_ORDER   128                  //滤波器阶数
#define FIR_SHIFT   FIR_ORDER/2
#define FIR_LEN     FIR_ORDER+1                //滤波器系数个数
#define NUMBLOCKS   FIR_NUM/BLOCK_SIZE  //需要调用arm_fir_f32的次数
extern u16 FIR_Shift;                  //移位阶数的一半
u16 calc_FIR_W(u32 Ts,u32 Tc);
void calc_FIR(float *input,u16 w);
void FIR_calc(float *input);

/*  IIR滤波器  */
#define IIR_N 1000 //采样点数 
#define IIR_Len 3 //2阶滤波器个数 
void IIR_test(float *Input,float* Output,u32 IIR_length);

/* 数据决策 */
#define CHOICE_LEN 10
#define THRESHOLD  0.01f
#define ROW     20   //最大支持20路滤波

#define AVER    0   //平均值
#define MANY    1   //众数
#define MID     2   //中位数
#define SMOOTH  3   //平滑输出
#define MAX     4   //最大值
#define MIN     5   //最小值
#define NON     6   //无操作

float Make_Choices(float choice,u16 row,u16 mode);
float is_Smooth(float* arr,u32 len);

#endif 
#endif
#endif
















