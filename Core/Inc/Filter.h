#ifndef __Filter_H__
#define __Filter_H__

#include "base.h"
#define LPFIR_ORDER 129
/*  AD����  */
#define AD_N  21
#define ADS8688_ZERO    32765
#define AD_mV_Scale 0.3120317334945769f
#define AD_V_Scale 0.0003023317334945769f


extern const float LowPassFIR[20][LPFIR_ORDER];
/*  ���Ҳ���  */
typedef struct _SinWave   //��������ṹ��
{
    float amp;      //���ֵ mv���迹ƥ��֮��
    float freq;     //Ƶ�� Hz
    float phase;     //��λ o
	float period;   //���� us
}SinWave;
extern SinWave sinwave;

#define LENGTH_SAMPLES    AD_N  //��������
/*FIR�˲�������*/
        //
#define BLOCK_SIZE_FIR     1             //����һ��FIR,����1����˲�
#define FIR_ORDER   128	              //�˲�������
#define FIR_SHIFT  	 FIR_ORDER/2
#define FIR_LEN     FIR_ORDER+1                //�˲���ϵ�����������ڽ�����1
#define NUMBLOCKS_FIR   LENGTH_SAMPLES/BLOCK_SIZE_FIR  //��Ҫ����arm_fir_f32�Ĵ���

/*IIR�˲���*/
#define BLOCK_SIZE_IIR   1                   // ����һ��arm_biquad_cascade_df1_f32����Ĳ��������
#define IIR_ORDER   3                  //2���˲������� = �˲�������/2
#define NUMBLOCKS_IIR   LENGTH_SAMPLES/BLOCK_SIZE_IIR  //��Ҫ����arm_iir_f32�Ĵ���


u16 calc_FIR_W(u32 Ts,u32 Tc);	//�����ֹƵ��
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
