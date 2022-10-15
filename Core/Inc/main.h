#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "pin_conf.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ds3231_for_stm32_hal.h"
#include "INA219.h"
#include "M24C32.h"
#include "AD7193.h"
#include "MAX6675.h"

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif
