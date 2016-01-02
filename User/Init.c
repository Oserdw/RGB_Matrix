#include "Init.h"


void LED_Init_io(void)
{
	//LED��ʼ�� led OE 
	GPIO_InitTypeDef GPIO_InitStructureA;
	GPIO_InitTypeDef GPIO_InitStructureB;
	GPIO_InitTypeDef GPIO_InitStructureC;
	GPIO_InitTypeDef GPIO_InitStructureX; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_InitStructureA.GPIO_Pin =GPIO_Pin_2;
  GPIO_InitStructureA.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructureA.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructureA);

  GPIO_InitStructureB.GPIO_Pin =GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;	
  GPIO_InitStructureB.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructureB.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_Init(GPIOB, &GPIO_InitStructureB);

  GPIO_InitStructureC.GPIO_Pin =GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5  | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_12;	
  GPIO_InitStructureC.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructureC.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_Init(GPIOC, &GPIO_InitStructureC);
	
	/* EXTI line gpio config(PC13) */	
  GPIO_InitStructureX.GPIO_Pin = GPIO_Pin_13;       
  GPIO_InitStructureX.GPIO_Mode = GPIO_Mode_IPU;	 // ��������
  GPIO_Init(GPIOC, &GPIO_InitStructureX);

/********************************************************************/
	GPIO_SetBits(GPIOC, GPIO_Pin_7);	  // OE��ʼ��Ϊ�ߵ�ƽ ����ʾ
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);  // SCKʱ�ӵ���0
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);  // LE��ʼ��Ϊ�͵�ƽ
  GPIO_ResetBits(GPIOC, GPIO_Pin_4);  // A
	GPIO_ResetBits(GPIOB, GPIO_Pin_10); // B
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);  // C
	GPIO_ResetBits(GPIOC, GPIO_Pin_0);  //R1
  GPIO_ResetBits(GPIOC, GPIO_Pin_2);  //R2
  GPIO_ResetBits(GPIOB, GPIO_Pin_11);  //G1
  GPIO_ResetBits(GPIOB, GPIO_Pin_12);  //G2
  GPIO_ResetBits(GPIOC, GPIO_Pin_1);  //B1
  GPIO_ResetBits(GPIOC, GPIO_Pin_3);  //B2
/*****************  LED  ***********************/
  GPIO_SetBits(GPIOA, GPIO_Pin_2);  //LED
  GPIO_SetBits(GPIOC, GPIO_Pin_8);  //LED_GREEN
  GPIO_SetBits(GPIOC, GPIO_Pin_9);  //LED_BLUE
  GPIO_SetBits(GPIOC, GPIO_Pin_12); //LED_RED
}

/*TIM_Period--1000   TIM_Prescaler--71 -->�ж�����Ϊ1ms*/
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period=1000;		 								/* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    TIM_TimeBaseStructure.TIM_Prescaler= (72 - 1);				    /* ʱ��Ԥ��Ƶ�� 72M/72 */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		/* ������Ƶ */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; /* ���ϼ���ģʽ */
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);							    		/* �������жϱ�־ */
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);																		/* ����ʱ�� */
    
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);		/*�ȹرյȴ�ʹ��*/    
}

/*
 * ��������EXTI_PB0_Config
 * ����  ������ PB0 Ϊ���жϿڣ��������ж����ȼ�
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void EXTI_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline(PC13) clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//RCC_APB2Periph_GPIOB | 

//	/* config the NVIC(PC13) */
//	NVIC_Configuration();


	/* EXTI line(PB0) mode config */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13); 
  EXTI_InitStructure.EXTI_Line = EXTI_Line13;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½����ж�
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 
}


/*
 * ��������NVIC_Configuration
 * ����  ������Ƕ�������жϿ�����NVIC
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* ����P[A|B|C|D|E]0Ϊ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
  NVIC_Init(&NVIC_InitStructure);  
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	
}
