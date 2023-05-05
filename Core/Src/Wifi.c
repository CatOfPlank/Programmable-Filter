#include "Wifi.h"
#include "usart.h"
#include "stdio.h"
#include "gpio.h"
unsigned char echo1[50];
unsigned char echo2[5000];
unsigned char echo3[250];
unsigned char echo4[350];
unsigned char line[10] = "\r\n";
void Wifi_Connect(){
	unsigned char buf2[80];
	unsigned char buf3[80];
	unsigned char buf4[80];
	unsigned char buf5[80];
//	sprintf((char*)buf1,"AT+RESTORE\r\n");	//重启
//  	HAL_UART_Transmit(&huart2,buf1,12,500);
	
//	HAL_UART_Receive(&huart1,echo,50,2000);
//	HAL_UART_Transmit(&huart1,echo,strlen(echo),2000);	//测试
	
		/*设置模式*/
	sprintf((char*)buf2,"AT+CWMODE=1\r\n");	//STA模式
	HAL_UART_Transmit(&huart3,buf2,13,1500);
	
	HAL_UART_Receive(&huart3,echo1,50,1500);
//	HAL_UART_Transmit(&huart1,echo1,strlen(echo1)+1,1500);	//测试
	
	sprintf((char*)buf3,"AT+RST\r\n");	//重启生效
	HAL_UART_Transmit(&huart3,buf3,8,1500);
	HAL_UART_Receive(&huart3,echo2,5000,2000);
//	HAL_UART_Transmit(&huart1,echo2,strlen(echo2)+1,2000);	//测试
	
	sprintf((char*)buf4,"AT+CWJAP=\"%s\",\"%s\"\r\n","dxxy16-402-1","dxxy16402");	//连接402wifi
	HAL_UART_Transmit(&huart3,buf4,37,20000);
//	HAL_UART_Transmit(&huart2,line,4,10000);
	HAL_UART_Receive(&huart3,echo3,50,20000);
	HAL_Delay(5000);
//	HAL_UART_Transmit(&huart1,echo3,strlen(echo3) + 1,150000);	//测试
	HAL_Delay(5000);
	
	/* 与PC机建立连接 */
	sprintf((char*)buf5,"AT+CIPSTART=\"%s\",\"%s\",%s\r\n", "TCP","192.168.0.171","8080");	
	HAL_UART_Transmit(&huart3,buf5,strlen(buf5),30000);
	HAL_UART_Receive(&huart3,echo4,50,30000);
	//HAL_UART_Transmit(&huart1,echo4,strlen(echo4) + 1,35000);	//测试
	
}
	
void Wifi_Send(float data)
{
	unsigned char data_buf[9];
	unsigned char send_buf[50];
	sprintf((char*)send_buf,"AT+CIPSEND=9\r\n");	//9个字节
	HAL_UART_Transmit(&huart3,send_buf,14,30000);
	HAL_Delay(5);
	
	sprintf((char*)data_buf,"%6.2f\r\n",data);	//保留两位小数
	HAL_UART_Transmit(&huart3,data_buf, 9 ,2000);
	HAL_Delay(150);
	
}
	
void bee()	//蜂鸣器提示连接成功
{
	__HAL_RCC_GPIOG_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_SET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_RESET);
//	__HAL_RCC_GPIOG_CLK_DISABLE();
	
}

