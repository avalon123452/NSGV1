#ifndef MAX6675_H
#define MAX6675_H
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TEMP 1024
#define MAX6675_TIMEOUT		HAL_MAX_DELAY

typedef struct
{
	SPI_HandleTypeDef *MAX6675_hspi;
	GPIO_TypeDef* SS_GPIO;
	uint16_t SS_pin;
} MAX6675_t;

void MAX6675_Init(MAX6675_t *dev, SPI_HandleTypeDef *spi, GPIO_TypeDef* SS_GPIO,uint16_t SS_pin);
uint16_t MAX6675_getReg(MAX6675_t *dev);
float MAX6675_getTemp(MAX6675_t *dev);
uint8_t MAX6675_isOpen(MAX6675_t *dev);

#ifdef __cplusplus
}
#endif


#endif /* MAX6675_H_ */
