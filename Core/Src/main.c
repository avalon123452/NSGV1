#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ds3231_for_stm32_hal.h"
#include "INA219.h"
#include "M24C32.h"

ADC_HandleTypeDef hadc1;
CAN_HandleTypeDef hcan1;
I2C_HandleTypeDef hi2c2;
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim9;

uint32_t tick=0, tim_capture[3][2]={0};
uint8_t freq_update[3]={0},beat=0, adc_update=0;
UART_HandleTypeDef uart={0};
CAN_RxHeaderTypeDef RxHeader;
uint8_t can_msg[8]={0};

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM9_Init(void);
void dmsg(char *msg);
void CAN_Filter_Config(void);
void HAL_SYSTICK_Callback(void);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);


int main(void)
{
	uint16_t vbus=0, vshunt=0, current=0;
	INA219_t ina1, ina2;
	uint8_t mem=10;
	char msg[100];
	CAN_TxHeaderTypeDef TxHeader={0};
	TxHeader.DLC = 5;
	TxHeader.StdId = 0x6A4;
	TxHeader.IDE   = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	uint32_t TxMailbox;
	uint16_t PIN_LED = PINC_RLED|PINC_GLED|PINC_BLED|PINC_YLED, year=0;
	uint32_t vmon=0;
	float freq[3]={0}, duty_cycle[3]={0};
   /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_CAN1_Init();
	MX_SPI1_Init();
	MX_ADC1_Init();
	MX_I2C2_Init();
	MX_USART1_UART_Init();
	MX_USART3_UART_Init();
	MX_TIM2_Init();
	MX_TIM5_Init();
	MX_TIM9_Init();
	DS3231_Init(&hi2c2);
	DS3231_SetFullTime(15, 25, 30);
	DS3231_SetFullDate(29, 9, 4, 2022);
	CAN_Filter_Config();
	while(!INA219_Init(&ina1, &hi2c2, INA219_ADDRESS));
	while(!INA219_Init(&ina2, &hi2c2, INA219_ADDRESS+1));
	M24C32_Init(&hi2c2);

	HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim5,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim5,TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim9,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim9,TIM_CHANNEL_2);

	HAL_ADC_Start_IT(&hadc1);

	uart= huart1;

	M24C32_WriteByte(0x0001, 69);
	HAL_Delay(1000);
	mem=M24C32_ReadRandomAddr(0x0001);

//	if(HAL_CAN_ActivateNotification(&hcan1,CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING |CAN_IT_BUSOFF) != HAL_OK)
//	{
//		Error_Handler();
//	}
//
//	if( HAL_CAN_Start(&hcan1) != HAL_OK)
//	{
//		Error_Handler();
//	}



	while (1)
	{
//		LED Toggle
		if (beat==1)
		{
			beat=0;
			HAL_GPIO_TogglePin(GPIOC,PINC_HEART|PIN_LED);
			HAL_GPIO_TogglePin(GPIOB, PINB_EN);
		}

//		GPIO Input
		memset(msg,0,sizeof(msg));
		sprintf(msg,"Switch Input: %d",HAL_GPIO_ReadPin(GPIOC, PINC_SWITCH));
		dmsg(msg);
		memset(msg,0,sizeof(msg));
		sprintf(msg,"State Input: %d",HAL_GPIO_ReadPin(GPIOB, PINB_STATE));
		dmsg(msg);

//		Pulse Input
		for (int i=0; i<3; i++)
		{
			if (freq_update[i]==1)
			{
				freq[i]= 1000000/tim_capture[i][0];
				duty_cycle[i]= (tim_capture[i][1]/tim_capture[i][0])*100;
				freq_update[i]=0;
			}
			memset(msg,0,sizeof(msg));
			sprintf(msg,"Frequency3: %.2f   Duty Cycle: %.2f", freq[i],duty_cycle[i]);
			dmsg(msg);
		}

//		Voltage Monitor
		if (adc_update==1)
		{
			vmon=HAL_ADC_GetValue(&hadc1);
			memset(msg,0,sizeof(msg));
			sprintf(msg,"Voltage Monitor: %ld", vmon);
			dmsg(msg);
		}
//		RTC
		year= DS3231_GetYear();
		memset(msg,0,sizeof(msg));
		sprintf(msg,"RTC Year: %d", year);
		dmsg(msg);
////		CAN TX
//		if( HAL_CAN_AddTxMessage(&hcan1,&TxHeader,(uint8_t*)"HELLO",&TxMailbox) != HAL_OK)
//		{
//			Error_Handler();
//		}
////		CAN RX
//		memset(msg,0,sizeof(msg));
//		sprintf(msg,"Message Received : #%x",can_msg[0]);
//		dmsg(msg);
//		Current Monitor
		vbus = INA219_ReadBusVoltage(&ina1);
		vshunt = INA219_ReadShuntVolage(&ina1);
		current = INA219_ReadCurrent(&ina1);
		memset(msg,0,sizeof(msg));
		sprintf(msg,"Bus Voltage1: %d  Voltage1: %d  Current1: %d", vbus, vshunt, current);
		dmsg(msg);
		vbus = INA219_ReadBusVoltage(&ina2);
		vshunt = INA219_ReadShuntVolage(&ina2);
		current = INA219_ReadCurrent(&ina2);
		memset(msg,0,sizeof(msg));
		sprintf(msg,"Bus Voltage2: %d  Voltage2: %d  Current2: %d", vbus, vshunt, current);
		dmsg(msg);
//		EEPROM
		memset(msg,0,sizeof(msg));
		sprintf(msg,"Memory 0x1:  %d  ", mem);
		dmsg(msg);

		HAL_Delay(1000);
	}
	return 0;
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	dmsg("CAN Transmitted Mailbox 0");

}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
	dmsg("CAN Transmitted Mailbox 1");
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	dmsg("CAN Transmitted Mailbox 2");
}

 void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&RxHeader,can_msg) != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	adc_update=1;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM2)
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
		{
			tim_capture[2][0]= HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			if (tim_capture[2][0]!= 0)
			{
				tim_capture[2][1]= HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
				freq_update[2]=1;
			}
		}
	}
	else if(htim->Instance==TIM5)
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
		{
			tim_capture[1][0]= HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			if (tim_capture[1][0]!= 0)
			{
				tim_capture[1][1]= HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
				freq_update[1]=1;
			}
		}
	}
	else if(htim->Instance==TIM9)
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
		{
			tim_capture[0][0]= HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			if (tim_capture[0][0]!= 0)
			{
				tim_capture[0][1]= HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
				freq_update[0]=1;
			}
		}
	}
}

void HAL_SYSTICK_Callback(void)
{
	if (HAL_GetTick()- tick >=1000)
	{
		tick = HAL_GetTick();
		beat=1;
	}
}

void dmsg(char *msg)
{
	char *str = malloc(strlen(msg) + 3);
	strcpy(str, msg);
	strcat(str, "\r\n");
	HAL_UART_Transmit(&uart, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

void CAN_Filter_Config(void)
{
	CAN_FilterTypeDef can1_filter_init;

	can1_filter_init.FilterActivation = ENABLE;
	can1_filter_init.FilterBank  = 0;
	can1_filter_init.FilterFIFOAssignment = CAN_RX_FIFO0;
	can1_filter_init.FilterIdHigh = 0x0000;
	can1_filter_init.FilterIdLow = 0x0000;
	can1_filter_init.FilterMaskIdHigh = 0X0000;
	can1_filter_init.FilterMaskIdLow = 0x0000;
	can1_filter_init.FilterMode = CAN_FILTERMODE_IDMASK;
	can1_filter_init.FilterScale = CAN_FILTERSCALE_32BIT;

	if( HAL_CAN_ConfigFilter(&hcan1,&can1_filter_init) != HAL_OK)
	{
		Error_Handler();
	}

}

static void SystemClock_Config(void)
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
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 320;
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
	__HAL_RCC_HSI_DISABLE();
}

static void MX_TIM2_Init(void)
{
	TIM_SlaveConfigTypeDef sSlaveConfig = {0};
	TIM_IC_InitTypeDef sConfigIC = {0};

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 80;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 0xFFFFFFFF;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
	sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
	sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1;
	sSlaveConfig.TriggerFilter = 0;
	if (HAL_TIM_SlaveConfigSynchro(&htim2, &sSlaveConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
	if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
	sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
	if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_TIM5_Init(void)
{
	TIM_SlaveConfigTypeDef sSlaveConfig = {0};
	TIM_IC_InitTypeDef sConfigIC = {0};

	htim5.Instance = TIM5;
	htim5.Init.Prescaler = 80;
	htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim5.Init.Period = 0xFFFFFFFF;
	htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_IC_Init(&htim5) != HAL_OK)
	{
		Error_Handler();
	}
	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
	sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
	sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1;
	sSlaveConfig.TriggerFilter = 0;
	if (HAL_TIM_SlaveConfigSynchro(&htim5, &sSlaveConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
	sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
	if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_TIM9_Init(void)
{
	TIM_SlaveConfigTypeDef sSlaveConfig = {0};
	TIM_IC_InitTypeDef sConfigIC = {0};

	htim9.Instance = TIM9;
	htim9.Init.Prescaler = 160;
	htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim9.Init.Period = 0xFFFF;
	htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_IC_Init(&htim9) != HAL_OK)
	{
		Error_Handler();
	}
	sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
	sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
	sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1;
	sSlaveConfig.TriggerFilter = 0;
	if (HAL_TIM_SlaveConfigSynchro(&htim9, &sSlaveConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
	if (HAL_TIM_IC_ConfigChannel(&htim9, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
	sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
	if (HAL_TIM_IC_ConfigChannel(&htim9, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_ADC1_Init(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	/** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	*/
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = ENABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	*/
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_CAN1_Init(void)
{
	hcan1.Instance = CAN1;
	hcan1.Init.Prescaler = 10;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = ENABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.AutoRetransmission = ENABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;
	if (HAL_CAN_Init(&hcan1) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_I2C2_Init(void)
{
	hi2c2.Instance = I2C2;
	hi2c2.Init.ClockSpeed = 100000;
	hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c2.Init.OwnAddress1 = 0;
	hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c2.Init.OwnAddress2 = 0;
	hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c2) != HAL_OK)
	{
		Error_Handler();
	}

}

static void MX_SPI1_Init(void)
{
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}

}

static void MX_USART1_UART_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}

}

static void MX_USART3_UART_Init(void)
{
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart3) != HAL_OK)
	{
		Error_Handler();
	}

}

static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	HAL_GPIO_WritePin(GPIOC, PINC_RLED|PINC_GLED|PINC_BLED|PINC_YLED|PINC_CAN_STB|PINC_HEART, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, PINC_SPI_SSA|PINC_SPI_SSB|PINC_SPI_SSV|PINC_SPI_SST, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = PINC_RLED|PINC_GLED|PINC_BLED|PINC_YLED|PINC_CAN_STB|
					  PINC_HEART|PINC_SPI_SSA|PINC_SPI_SSB|PINC_SPI_SSV|PINC_SPI_SST;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PINC_SWITCH;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PINA_IC_FREQ3|PINA_IC_FREQ2|PINA_IC_FREQ1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PINB_EN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PINB_STATE;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void Error_Handler(void)
{
	__disable_irq();
	while (1)
	{
	}
}

