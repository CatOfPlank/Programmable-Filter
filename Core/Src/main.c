/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dac.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "arm_math.h"
#include "Filter.h"
#include "ADS8688.H"
#include "head_define.h"
#include "hmi_driver.h"
#include "hmi_user_uart.h"
#include "cmd_queue.h"
#include "cmd_process.h"
#include "stdio.h"
#include "base.h"
#include "Wifi.h" 
#include "MAX262.h"
#include "AD9959.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float sweep_points[21] = {0,};
float
AD_arr[2][AD_N],
*pAD_arr,*pAD_arr_end,

FFT_result[2][AD_N],
FFT_INPUT[AD_N],
AD_arr_c[2][AD_N*2]={0,},
*pAD_arr_c,*pAD_arr_c_end,
FFT_result_c[2][AD_N];
u16 ad_result[8];
u8 dataGraph[AD_N],
*pDataGraph;

extern my_Filter my_Filter1;

enum Sample_Rate{  //采样率
K200 = 200000,K400 = 400000,K800 = 800000,K1000 = 1000000

};

enum Sample_State{ //采样状态
OK=1,NO = !OK
};

u32 ctrl_dac[7] = {265,400,548,690,845,1041,1900};	//控制放大器增益

float hp_arr[50]=
{
0.088,0.176,0.267,0.348,0.440,0.530,0.620,0.710,0.800,0.880,0.990,1.07,1.15,1.25,1.35,1.45,1.55,1.58,1.68,1.80,1.91,1.98,2.03,2.11,2.21,2.30,\
2.45,2.50,2.65,2.75,2.85,2.95,3.05,3.15,3.25,3.35,3.45,3.55,3.56,3.75,3.85,3.95,4.05,4.15,4.23,4.45,4.55,4.65,4.78,4.88	};  //高通数组
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

	
void getAD_allch(float*result){
	Get_AUTO_RST_Mode_Data((uint16_t*)ad_result,8);  //两个通道
	*result=(ad_result[0]-ADS8688_ZERO)*AD_mV_Scale;
	result+=AD_N;
	*result=(ad_result[1]-ADS8688_ZERO)*AD_mV_Scale;
//	result+=AD_N;
}

void AD_arr_Init(){
	pAD_arr=AD_arr[0];pAD_arr_end=AD_arr[0]+AD_N;
	memset(AD_arr[0],0,AD_N);
	memset(AD_arr[1],0,AD_N);
}

float get_AD_result(){
	float rms;
	AD_arr_Init();
	getAD_allch(pAD_arr);
	HAL_TIM_Base_Start_IT(&htim4);
	while(pAD_arr!=pAD_arr_end){}
	HAL_TIM_Base_Stop_IT(&htim4);
	arm_mean_f32(AD_arr[0],21,&rms);
	return rms;
}

void Sweep_Freq(void)
{
	u16 i,times;
	times=(sweepfreq.end-sweepfreq.start)/sweepfreq.step+1;  //扫频点数
//	Out_freq(0,fre,1000);
	for(i=0;i<times;i++)  
	{
        Out_freq(0,sweepfreq.start+(sweepfreq.step*i),800);  //输出最大530vpp
		sweep_points[i] = get_AD_result(); //采样
	//	Wifi_Send(sweep_points[i]);
	}
}


void ctrl_amplifier() //程控放大器	
{	
	HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
	HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,ctrl_dac[my_Filter1.amp_dB]);

}

void calSysPara()  //计算电路参数
{
	float max_temp,resolu,freq;
	float funWave,har_2,har_3,har_4,har_5;
	u32	max_idex;
	calc_FFT(AD_arr[0],FFT_result[0]);
	my_Filter1.resolution_fre = my_Filter1.sample_rate / my_Filter1.show_points;  	//计算分辨率
	arm_max_f32(FFT_result[0],AD_N,&max_temp,&max_idex);  	//找最大值
	freq = max_idex *resolu;
	my_Filter1.cur_fre = freq;		//当前频率
	funWave = FFT_result[0][max_idex];		//基波
	har_2 = pow((double)FFT_result[0][max_idex * 2],2); 	//二次谐波
	har_3 = pow((double)FFT_result[0][max_idex * 3],2); 	//三次谐波
	har_4 = pow((double)FFT_result[0][max_idex * 4],2); 	//四次谐波
	har_5 = pow((double)FFT_result[0][max_idex * 5],2); 	//五次谐波
	my_Filter1.THD = sqrt(har_2+har_3+har_4+har_5) / funWave;
	
}
void Filter_Init(void)
{
	my_Filter1.Mode = Normal;
	my_Filter1.sample_rate = K200;  	//初始采样率200k
	my_Filter1.cur_fre = 0; 		//获取到的当前频率
	my_Filter1.show_points = AD_N;  	//初始点数
	my_Filter1.cutoff_fre = 1000;  	//初始截止频率1k
	my_Filter1.filterKind = LP;  	//滤波器类型默认低通
	my_Filter1.THD = 0; 	//总谐波失真初始为0
	sweepfreq.start = 100;  	//扫频开始为1000Hz
	sweepfreq.end = 200100;  	//扫频结束200100
	sweepfreq.step = 10000;		//步进10k
	sweepfreq.time = 100;		//每个点持续时间
	GraphChannelDataClear(1,3,0);
}



void setCutoff() 		//切换截止频率
{	
	if(my_Filter1.filterKind == LP)
		Out_freq(1, my_Filter1.cutoff_fre * 115,500);		//输出时钟
	else if(my_Filter1.filterKind == HP &&my_Filter1.cutoff_fre/1000 <= 50)
		Out_freq(1, hp_arr[my_Filter1.cutoff_fre/1000 - 1]* 1000000,500);//hp_arr[my_Filter1.cutoff_fre/1000 - 1]
} 
void showGraph()
{

	float32_t max,sweep_N;
	sweep_N = AD_N; 	//
	u32 useless;
	arm_max_f32(sweep_points,sweep_N,&max,&useless);
	if(max<1)max = 1;
	arm_scale_f32(sweep_points, 255.0f/max,sweep_points,sweep_N);//归一化
	pDataGraph = dataGraph; pAD_arr = sweep_points;
	
	for(int i=0;i<sweep_N;i++)
		*pDataGraph++ = *pAD_arr++;  //float->u8
//	if(my_Filter1.filterKind == LP)
//		dataGraph[0] = dataGraph[1] * 0.96;
//	else if(my_Filter1.filterKind == HP)
//		dataGraph[0] = 0;
	GraphChannelDataClear(1,3,0);
	GraphChannelDataAdd(1,3,0,dataGraph,sweep_N);


}

void Signal_or_Sweep()  //开关1切换信号源和扫频，低电平有效
{
	if(my_Filter1.Mode == Sweep)
		HAL_GPIO_WritePin(Switch_1_GPIO_Port,Switch_1_Pin,GPIO_PIN_SET) ; //高电平切换9959，扫频
	else 
		HAL_GPIO_WritePin(Switch_1_GPIO_Port,Switch_1_Pin,GPIO_PIN_RESET) ; 
}

void Test_or_Next() //开关2默认测试，高电平接到后级电路
{
	if(my_Filter1.filterKind != Amp)
		HAL_GPIO_WritePin(Switch_2_GPIO_Port,Switch_2_Pin,GPIO_PIN_SET) ; //高电平切换到后级
	else
		HAL_GPIO_WritePin(Switch_2_GPIO_Port,Switch_2_Pin,GPIO_PIN_RESET) ; 
}

void setFilterMode() //开关3切换滤波类型，椭圆or高低通，高电平触发
{
	if(my_Filter1.filterKind == HP || my_Filter1.filterKind == LP)
		HAL_GPIO_WritePin(Switch_3_GPIO_Port,Switch_3_Pin,GPIO_PIN_SET) ; //低电平换到高低通模式
	else if(my_Filter1.filterKind ==ELL) //椭圆
		HAL_GPIO_WritePin(Switch_3_GPIO_Port,Switch_3_Pin,GPIO_PIN_RESET);
		
}

void Lp_or_Hp()		//开关4切换高通或低通，默认低电平高通
{
	if(my_Filter1.filterKind == LP)
		HAL_GPIO_WritePin(Switch_4_GPIO_Port,Switch_4_Pin,GPIO_PIN_SET) ; //高电平切换到低通
	else if(my_Filter1.filterKind == HP)
		HAL_GPIO_WritePin(Switch_4_GPIO_Port,Switch_4_Pin,GPIO_PIN_RESET) ; //高电平切换到后级

}

void Out_LpHp_or_epl() //开关5切换输出类型，高电平为椭圆滤波器输出
{
	if(my_Filter1.filterKind == ELL)
		HAL_GPIO_WritePin(Switch_5_GPIO_Port,Switch_5_Pin,GPIO_PIN_SET) ;
	else 
		HAL_GPIO_WritePin(Switch_5_GPIO_Port,Switch_5_Pin,GPIO_PIN_RESET) ;

}
void softReset()  //软件复位
{
		HAL_NVIC_SystemReset();
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM4_Init();
  MX_DAC_Init();
  /* USER CODE BEGIN 2 */
  delay_init(168);
  Init_AD9959();		//9959初始化
  ADS8688_Init_Mult();
  Filter_Init();    //参数初始化
  TFT_Init();		//大彩屏初始化
//	Wifi_Connect();	//wifi初始化
	bee();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  switch(my_Filter1.Mode)
	  {  
		  case Normal:  	//放大/滤波模式
		  {
			  if(my_Filter1.filterKind == Amp) 		//放大模式
			  { 
				    ctrl_amplifier();
					Signal_or_Sweep();		//开关1
					Test_or_Next();  		//开关2
			  }
			  else  			//滤波
			  {
					setCutoff(); 
					Signal_or_Sweep();		//开关1
					Test_or_Next();  		//开关2
					setFilterMode();  		//开关3
					Lp_or_Hp(); 			//开关4
					Out_LpHp_or_epl();		//开关5
				  }
			  
		   }break;
		  case Sweep:		//扫频
		  {
					Signal_or_Sweep();		//开关1
					Test_or_Next();  		//开关2
					setFilterMode();  		//开关3
					Lp_or_Hp(); 			//开关4
					Out_LpHp_or_epl();		//开关5
			  		Sweep_Freq();			//扫频输出
					showGraph();			//绘制曲线
			//GraphChannelDataAdd(2,3,0,test,21);
			}break; 	 
		  case sysReset:
			  softReset();break;
		  
	  }

	//  delay_ms(100);

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &htim4){
		if(pAD_arr < pAD_arr_end){
         getAD_allch(pAD_arr++);
    }
	}
}
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
//	if(htim == &htim4)
//		{

//				
//		{
//    }
//}
//}
		//}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
