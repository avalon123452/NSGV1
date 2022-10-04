
/* An STM32 HAL library written for the DS3231 real-time clock IC. */
/* Library by @eepj www.github.com/eepj */
#ifndef M24C32_H
#define M24C32_H
#include "main.h"
/*----------------------------------------------------------------------------*/
#define M24C32_I2C_ADDR 	0xAE

#define M24C32_TIMEOUT		HAL_MAX_DELAY

void M24C32_Init(I2C_HandleTypeDef *hi2c);

void M24C32_WriteByte(uint16_t regAddr, uint8_t val);
uint8_t M24C32_ReadCurrentAddr();
uint8_t M24C32_ReadRandomAddr(uint16_t regAddr);



#endif
