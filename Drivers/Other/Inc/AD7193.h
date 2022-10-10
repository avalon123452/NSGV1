#include "main.h"

#ifndef AD7193_H
#define AD7193_H

#ifdef __cplusplus
extern "C" {
#endif

/* AD7193 Register Map */
#define AD7193_REG_COMM         0 // Communications Register (WO, 8-bit)
#define AD7193_REG_STAT         0 // Status Register         (RO, 8-bit)
#define AD7193_REG_MODE         1 // Mode Register           (RW, 24-bit
#define AD7193_REG_CONF         2 // Configuration Register  (RW, 24-bit)
#define AD7193_REG_DATA         3 // Data Register           (RO, 24/32-bit)
#define AD7193_REG_ID           4 // ID Register             (RO, 8-bit)
#define AD7193_REG_GPOCON       5 // GPOCON Register         (RW, 8-bit)
#define AD7193_REG_OFFSET       6 // Offset Register         (RW, 24-bit
#define AD7193_REG_FULLSCALE    7 // Full-Scale Register     (RW, 24-bit)

/* Communications Register Bit Designations (AD7193_REG_COMM) */
#define AD7193_COMM_WEN         (1 << 7)           // Write Enable.
#define AD7193_COMM_WRITE       (0 << 6)           // Write Operation.
#define AD7193_COMM_READ        (1 << 6)           // Read Operation.
#define AD7193_COMM_ADDR(x)     (((x) & 0x7) << 3) // Register Address.
#define AD7193_COMM_CREAD       (1 << 2)           // Continuous Read of Data Register.

#define AD7193_TIMEOUT		HAL_MAX_DELAY

typedef struct
{
	SPI_HandleTypeDef *AD7193_hspi;
	GPIO_TypeDef* SS_GPIO;
	uint16_t SS_pin;
} AD7193_t;

void AD7193_Init(AD7193_t *ad7193, SPI_HandleTypeDef *spi, GPIO_TypeDef* SS_GPIO,uint16_t SS_pin);
void AD7193_Reset(AD7193_t *ad7193);
void AD7193_WriteRegValue(AD7193_t *ad7193,uint8_t regAddr, uint32_t val, uint8_t data_length);
uint32_t AD7193_GetRegValue(AD7193_t *ad7193,uint8_t regAddr, uint8_t data_length);


#ifdef __cplusplus
}
#endif

#endif /* OTHER_INC_AD7193_H_ */
