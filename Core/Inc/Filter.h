#ifndef __Filter_H__
#define __Filter_H__

#include "base.h"
#define LPFIR_ORDER 129
/*  AD采样  */
#define AD_N  21
#define ADS8688_ZERO    32765
#define AD_mV_Scale 0.3120317334945769f
#define AD_V_Scale 0.0003023317334945769f


extern const float LowPassFIR[20][LPFIR_ORDER];
/*  正弦参数  */
typedef struct _SinWave   //数组参数结构体
{
    float amp;      //峰峰值 mv（阻抗匹配之后）
    float freq;     //频率 Hz
    float phase;     //相位 o
	float period;   //周期 us
}SinWave;
extern SinWave sinwave;

#define LENGTH_SAMPLES    AD_N  //采样点数
/*FIR滤波器参数*/
        //
#define BLOCK_SIZE_FIR     1             //计算一次FIR,等于1逐点滤波
#define FIR_ORDER   128	              //滤波器阶数
#define FIR_SHIFT  	 FIR_ORDER/2
#define FIR_LEN     FIR_ORDER+1                //滤波器系数个数，等于阶数加1
#define NUMBLOCKS_FIR   LENGTH_SAMPLES/BLOCK_SIZE_FIR  //需要调用arm_fir_f32的次数

/*IIR滤波器*/
#define BLOCK_SIZE_IIR   1                   // 调用一次arm_biquad_cascade_df1_f32处理的采样点个数
#define IIR_ORDER   3                  //2阶滤波器个数 = 滤波器阶数/2
#define NUMBLOCKS_IIR   LENGTH_SAMPLES/BLOCK_SIZE_IIR  //需要调用arm_iir_f32的次数


u16 calc_FIR_W(u32 Ts,u32 Tc);	//计算截止频率
u16 calc_IIR_W(u32 Ts,u32 Tc);
void calc_FIR_noW(float*,float*para);
void calc_FIR(float *input,u16 w);
void calc_IIR(float *Input,float* Output);
void calc_FFT(float*Input,float*Output);
void hanning_window(float * pDst,u32 blockSize);
void hamming_window(float * pDst,u32 blockSize);
void blackman_window(float * pDst,u32 blockSize);
void PowerPhaseRadians_f32(float* Input_f32,float Phase_f32,uint16_t _usFFTPoints, float _uiCmpValue);
#endif 
