
#ifndef __ALGORITHM_H 
#define __ALGORITHM_H

#include "base.h"
#include "head_define.h"
#include "Filter.h"
#if 0
extern float Coref[10];

/*  ���Ҳ���  */
typedef struct _SinWave   //��������ṹ��
{
    float amp;      //���ֵ mv���迹ƥ��֮��
    float freq;     //Ƶ�� Hz
    float phase;     //��λ o
	float period;   //���� us
}SinWave;
extern SinWave sinwave;

/*  AD����  */
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
    

typedef struct _ArrayParam   //��������ṹ��
{
    float max;      //���ֵ
    float min;      //��Сֵ
    float tft_len;  //����һ������ռ��������
    float tft_cycle;
    float Vpp;   //���ֵ mV
    float Period;//����   us
    float Aver;  //ƽ��ֵ mv
    float Rms;   //��Чֵ mv
}ArrayParam;
extern ArrayParam AD_Params;
void Calc_array(ArrayParam *params,float* calc_addr,u32 calc_len);    //��������Ĳ���
#if 0
/*  FIR�˲���  */
#define FIR_NUM    AD_N          //FIR����
#define BLOCK_SIZE  100                   //���㵥��FIR
#define FIR_ORDER   128                  //�˲�������
#define FIR_SHIFT   FIR_ORDER/2
#define FIR_LEN     FIR_ORDER+1                //�˲���ϵ������
#define NUMBLOCKS   FIR_NUM/BLOCK_SIZE  //��Ҫ����arm_fir_f32�Ĵ���
extern u16 FIR_Shift;                  //��λ������һ��
u16 calc_FIR_W(u32 Ts,u32 Tc);
void calc_FIR(float *input,u16 w);
void FIR_calc(float *input);

/*  IIR�˲���  */
#define IIR_N 1000 //�������� 
#define IIR_Len 3 //2���˲������� 
void IIR_test(float *Input,float* Output,u32 IIR_length);

/* ���ݾ��� */
#define CHOICE_LEN 10
#define THRESHOLD  0.01f
#define ROW     20   //���֧��20·�˲�

#define AVER    0   //ƽ��ֵ
#define MANY    1   //����
#define MID     2   //��λ��
#define SMOOTH  3   //ƽ�����
#define MAX     4   //���ֵ
#define MIN     5   //��Сֵ
#define NON     6   //�޲���

float Make_Choices(float choice,u16 row,u16 mode);
float is_Smooth(float* arr,u32 len);

#endif 
#endif
#endif
















