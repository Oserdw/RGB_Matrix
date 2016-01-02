/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V2.0
  * @date    2013-xx-xx
  * @brief   STM32 RGB������ʾ����
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "Systick.h"
#include "stm32_LED.H"
#include "stm32f10x_uart.h"
#include "dht11.h"
#include "spi_flash.h"
#include "Init.h"
#include "rtc.h"
#include "i2c_ee.h"

/****************************  SPI  **************/
/* ��ȡ�������ĳ��� */
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize (countof(Tx_Buffer)-1)  //��ȡ�����С
#define  Read_BufferSize 128

#define  FLASH_WriteAddress     0x0000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
#define  sFLASH_ID              0xEF3015
#define  SPI_FLASH_NUM          3             //����  Ҫ��4
/**************** I2C ****************************/
#define EEP_Firstpage      0x00   //i2c ��ַ
#define EEP_NUMBER 2              //
u8 I2c_Buf_Write[EEP_NUMBER];     //д����
u8 I2c_Buf_Read[EEP_NUMBER];      //������
/*************************************************/
#define System_Error   0xa3;      //ϵͳ����
/***********ϵͳ��ʱ��**********/
extern __IO u32 Timing;           //ϵͳsystick��ʱ������
/***********��ʾ����************/
extern u8 String_DisCache_R[];    //��ʾ����
extern u8 String_DisCache_G[];
extern u8 String_DisCache_B[];
extern u8 String_number[];
extern __IO u8 hour,minute,second;//Сʱ�����ӣ��룬ȫ�ֱ���

extern char g_DatRev[Max_UART_String]; //���ڻ������
extern u8 g_DatRev_num;           //���ڼ�������
extern u8 UART_FLAG;              //���ڱ�־λ
//extern STR_num;
u32 String_num=0;                 //��ʾ��ȡ��ַ����
DHT11_Data_TypeDef DHT11_Data;    //�¶�ʪ�Ȼ���
__IO u32 time;                    // ms �����ٶȼ�ʱ����
__IO u8 Display_flag=0x01;        //��ʾģʽ��־ ģʽ����

uint8_t Tx_Buffer[] = {
0

};

void I2C_Test(void)
{
	printf("д�������\n\r");
  I2c_Buf_Write[0] = 0x01;      //��ʾģʽ����
  I2c_Buf_Write[1] = 0x03;      //�ַ�����������
  //��I2c_Buf_Write��˳�����������д��EERPOM�� 
	I2C_EE_BufferWrite( I2c_Buf_Write, EEP_Firstpage, EEP_NUMBER);	 
  
  printf("\n\r����������\n\r");
  //��EEPROM��������˳�򱣳ֵ�I2c_Buf_Read�� 
	I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, EEP_NUMBER); 

  //��I2c_Buf_Read�е�����ͨ�����ڴ�ӡ
}

int main(void)
{
	  //SystemInit();
    SysTick_Init();
    /* USART1 config 115200 8-N-1 */
	  USART1_Config();
	  printf("\n\n********This is Maxire Display System**********");
		/* 2M����flash W25X16��ʼ�� */
		SPI_FLASH_Init();
	  EXTI_Config();
	  TIM2_Configuration();
		/*��ʼ��DTT11������*/
		DHT11_GPIO_Config();
	  /* I2C �����(AT24C02)ʼ�� */
  	I2C_EE_Init();
	  NVIC_Configuration();
	  /* led CONFIG  */
		LED_Init_io();
    Set_time();

		Time_Show();
		/*����Read_DHT11��ȡ��ʪ�ȣ����ɹ����������Ϣ*/
//	  Delay_us(1000);
		if( Read_DHT11(&DHT11_Data)==SUCCESS)
		{
			printf("[OK] ʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\n",\
			DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
			//printf("\r\n ʪ��:%d,�¶�:%d \r\n" ,DHT11_Data.humi_int,DHT11_Data.temp_int);
		}
		else
		{
			printf("[ERROR] Read DHT11 ERROR!\r\n");
		}
//    I2C_Test();
		
/*********************** RTC  *****************/
	  while(1)   //ѭ��
		{
        if(UART_FLAG==0x01)   //�����־λ��һλ��һ˵���������յ�'\n'
				{
						UART_FLAG &= 0xFE;
						printf("String is:%s\n",g_DatRev);
					  //�����ܵ����ݺ󣬽��������������������
					  
					  if(strcmp1(g_DatRev,"AT+RST\r\n"))
						{
							printf("OK");
            }

						if(strcmp1(g_DatRev,"AT+SPI_FLASH_BulkErase\r\n"))
						{
								SPI_FLASH_BulkErase();
                printf("[OK] SPI_FLASH_BulkErase OK");
            }
						
						if(strcmp1(g_DatRev,"AT+A"))
						{
                /* �����ͻ�����������д��flash�� */
//								SPI_FLASH_BulkErase();
                printf("[OK] SPI_FLASH_BulkErase OK");
//                SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
							  printf("OK");
            }
						while(g_DatRev_num)  //��ս��ܻ�����
						{
                 g_DatRev[g_DatRev_num]=0;
							   g_DatRev_num--;
						}
        }
				else if(UART_FLAG==0x02)  //������������ 
				{
            UART_FLAG &= 0xFD;  //�����־λ
						printf("\r\n Data overflow \r\n");
						while(g_DatRev_num)    //��ս��ܻ���
						{
                 g_DatRev[g_DatRev_num]=0;
							   g_DatRev_num--;
						}
        }
				
				if(Display_flag==0)  //�ж��Ƿ�Ϊ1 Ϊ1����ʾʱ��
				{
					  if(time>=1000)
						{
                if( Read_DHT11(&DHT11_Data)==SUCCESS){}
						    Show_Time(RTC_GetCounter());
							  time=0;
            }
						Display_num(DHT11_Data.humi_int,DHT11_Data.temp_int,hour,minute);
						//Delay_us(25);
				}
				else         //����Ϊ1 ����ʾ����
				{
					  //if(time>100)
						display_HZ();
						if ( time >= 1000) /* 1s ʱ�䵽 */
						{
							time = 0;
							LED_BLUE_OFF;
							/* LED1 �ر� */
							LED_GREEN_ON;
							
							if((Display_flag&0x01)==0x01)
							    SPI_FLASH_BufferRead(String_DisCache_R, (String_num*128), Read_BufferSize);  //��ȡflash������
							if((Display_flag&0x02)==0x02)
							    SPI_FLASH_BufferRead(String_DisCache_G, (String_num*128), Read_BufferSize);  //��ȡflash������
							if((Display_flag&0x04)==0x04)
							    SPI_FLASH_BufferRead(String_DisCache_B, (String_num*128), Read_BufferSize);  //��ȡflash������

							String_num=String_num+1;
							if(String_num>=SPI_FLASH_NUM)       //�ж�������ڵ���spi flash num ����������4
									String_num=FLASH_WriteAddress;  //
						}
//					  Delay_us(1);
				}
    }
}

/*********************************************END OF FILE**********************/

