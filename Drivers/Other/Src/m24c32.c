
#include "M24C32.h"


I2C_HandleTypeDef *_M24C32_ui2c;

void M24C32_Init(I2C_HandleTypeDef *hi2c) {
	_M24C32_ui2c = hi2c;
}

void M24C32_WriteByte(uint16_t regAddr, uint8_t val) {
	uint8_t bytes[3] = { regAddr>>8, regAddr&0xFF, val };
	HAL_I2C_Master_Transmit(_M24C32_ui2c, M24C32_I2C_ADDR, bytes, 3, M24C32_TIMEOUT);
}

uint8_t M24C32_ReadCurrentAddr() {
	uint8_t val;
	HAL_I2C_Master_Receive(_M24C32_ui2c, M24C32_I2C_ADDR, &val, 1, M24C32_TIMEOUT);
	return val;
}

uint8_t M24C32_ReadRandomAddr(uint16_t regAddr) {
	uint8_t val;
	uint8_t bytes[2] = { regAddr>>8, regAddr&0xFF};
	HAL_I2C_Master_Transmit(_M24C32_ui2c, M24C32_I2C_ADDR, bytes, 2, M24C32_TIMEOUT);
	HAL_I2C_Master_Receive(_M24C32_ui2c, M24C32_I2C_ADDR, &val, 1, M24C32_TIMEOUT);
	return val;
}


