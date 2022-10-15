#include "MAX6675.h"

void MAX6675_Init(MAX6675_t *dev, SPI_HandleTypeDef *spi, GPIO_TypeDef* SS_GPIO,uint16_t SS_pin)
{
	dev->MAX6675_hspi = spi;
	dev->SS_GPIO = SS_GPIO;
	dev->SS_pin = SS_pin;
}

uint16_t MAX6675_getReg(MAX6675_t *dev)
{
	uint16_t val =0;
	uint8_t buffer[2]={0};
	HAL_GPIO_WritePin(dev->SS_GPIO, dev->SS_pin, GPIO_PIN_RESET);
	HAL_SPI_Receive(dev->MAX6675_hspi, buffer, 2, MAX6675_TIMEOUT);
	HAL_GPIO_WritePin(dev->SS_GPIO, dev->SS_pin, GPIO_PIN_SET);

	for (uint8_t i = 0; i < 2; i++) {
		val = val <<8;
		val += buffer[i];
	}

	return val;
}

float MAX6675_getTemp(MAX6675_t *dev)
{
	float temp=0;
	uint16_t val=0;
	val = MAX6675_getReg(dev) >> 3;
	temp = val * 1024 / 4095;

	return temp;
}

uint8_t MAX6675_isOpen(MAX6675_t *dev)
{
	uint16_t val=0;
	val = MAX6675_getReg(dev) >> 2;
	val = (uint8_t) val & 0x1;
	return val;
}
















