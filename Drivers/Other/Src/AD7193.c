
#include "AD7193.h"


void AD7193_Init(AD7193_t *ad7193, SPI_HandleTypeDef *spi, GPIO_TypeDef* SS_GPIO,uint16_t SS_pin) {
	ad7193->AD7193_hspi = spi;
	ad7193->SS_GPIO = SS_GPIO;
	ad7193->SS_pin = SS_pin;
}

void AD7193_Reset(AD7193_t *ad7193) {
	HAL_GPIO_WritePin(ad7193->SS_GPIO, ad7193->SS_pin, GPIO_PIN_RESET);
	HAL_Delay(5);
	for (uint8_t i=0; i<4; i++) {
		HAL_SPI_Transmit(ad7193->AD7193_hspi, (uint8_t *)0xFF, 1, AD7193_TIMEOUT);
	}
	HAL_GPIO_WritePin(ad7193->SS_GPIO, ad7193->SS_pin, GPIO_PIN_SET);
	HAL_Delay(1);
}

void AD7193_WriteRegValue(AD7193_t *ad7193,uint8_t regAddr, uint8_t data_length,uint32_t val) {
	uint8_t bytes[4] = {val ,val>>8,val>>16,val>>24};
	uint8_t command= AD7193_COMM_WRITE | AD7193_COMM_ADDR(regAddr);
	HAL_GPIO_WritePin(ad7193->SS_GPIO, ad7193->SS_pin, GPIO_PIN_RESET);
	HAL_Delay(5);
	HAL_SPI_Transmit(ad7193->AD7193_hspi, &command, 1, AD7193_TIMEOUT);
	for(uint8_t i=data_length-1; i>=0; i--) {
		HAL_SPI_Transmit(ad7193->AD7193_hspi,&bytes[i], 1, AD7193_TIMEOUT);
	}
	HAL_GPIO_WritePin(ad7193->SS_GPIO, ad7193->SS_pin, GPIO_PIN_SET);
	HAL_Delay(5);
}

uint8_t AD7193_GetRegValue(AD7193_t *ad7193,uint8_t regAddr, uint8_t data_length) {
//	uint32_t val=0;
	uint8_t buffer=0;
	uint8_t command= AD7193_COMM_READ | AD7193_COMM_ADDR(regAddr);
	HAL_GPIO_WritePin(ad7193->SS_GPIO, ad7193->SS_pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(ad7193->AD7193_hspi, &command, 1, AD7193_TIMEOUT);
	HAL_SPI_Receive(ad7193->AD7193_hspi,&buffer, data_length, AD7193_TIMEOUT);
	HAL_GPIO_WritePin(ad7193->SS_GPIO, ad7193->SS_pin, GPIO_PIN_SET);
	return buffer;
}

