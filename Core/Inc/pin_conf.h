#ifndef __PIN_CONF_H
#define __PIN_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

#define PINA_IC_FREQ3	GPIO_PIN_0
#define PINA_IC_FREQ2	GPIO_PIN_1
#define PINA_IC_FREQ1	GPIO_PIN_2
#define PINA_ADC_VMON	GPIO_PIN_3
#define PINA_SPI_SCK	GPIO_PIN_5
#define PINA_SPI_MISO	GPIO_PIN_6
#define PINA_SPI_MOSI	GPIO_PIN_7
#define PINB_EN			GPIO_PIN_0
#define PINB_STATE    	GPIO_PIN_1
#define PINB_UART1_TX 	GPIO_PIN_6
#define PINB_UART1_RX 	GPIO_PIN_7
#define PINB_CAN_RX   	GPIO_PIN_8
#define PINB_CAN_TX   	GPIO_PIN_9
#define PINB_I2C_SCL  	GPIO_PIN_10
#define PINB_12C_SDA  	GPIO_PIN_11
#define PINC_SWITCH   	GPIO_PIN_0
#define PINC_RLED     	GPIO_PIN_1
#define PINC_GLED     	GPIO_PIN_2
#define PINC_BLED     	GPIO_PIN_3
#define PINC_YLED     	GPIO_PIN_4
#define PINC_SPI_SSA  	GPIO_PIN_5
#define PINC_SPI_SSB  	GPIO_PIN_6
#define PINC_SPI_SSV  	GPIO_PIN_7
#define PINC_SPI_SST  	GPIO_PIN_8
#define PINC_CAN_STB  	GPIO_PIN_9
#define PINC_UART2_RX 	GPIO_PIN_10
#define PINC_UART2_TX 	GPIO_PIN_11
#define PINC_HEART    	GPIO_PIN_13


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_IT_H */
