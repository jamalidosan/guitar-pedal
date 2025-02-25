/*
 * opt3001.h
 *
 *  Created on: Oct 20, 2020
 *      Author: Joe Krachey
 */

#ifndef __LED_DRIVER_H__ 
#define __LED_DRIVER_H__ 

#include <stdint.h>
#include <stdbool.h>
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include <stdio.h>
#include <string.h>
#include "console.h"
#include "main.h"

#define KTD2052A_SUBORDINATE_ADDR                 0x75
#define LED_DRIVER_SUBORDINATE_ADDR               KTD2052A_SUBORDINATE_ADDR 

#define KTD2052_REG_ID                            0x00
#define KTD2052_REG_MONITOR                       0x01
#define KTD2052_REG_CONTROL                       0x02
#define KTD2052_REG_IRED1                         0x04 // good
#define KTD2052_REG_IGRN1                         0x03 // good
#define KTD2052_REG_IBLU1                         0x05 // good
#define KTD2052_REG_IRED2                         0x07 // good
#define KTD2052_REG_IGRN2                         0x06 // good
#define KTD2052_REG_IBLU2                         0x08 // good
#define KTD2052_REG_IRED3                         0x0A // good
#define KTD2052_REG_IGRN3                         0x09 // good 
#define KTD2052_REG_IBLU3                         0x0B // good
#define KTD2052_REG_IRED4                         0x0D // good
#define KTD2052_REG_IGRN4                         0x0C // good
#define KTD2052_REG_IBLU4                         0x0E // good
#define KTD2052_REG_PG_CNTL                       0x0F
#define KTD2052_REG_PG_FADE                       0x10
#define KTD2052_REG_PG_RGB1                       0x11
#define KTD2052_REG_PG_RGB2                       0x12
#define KTD2052_REG_PG_RGB3                       0x13
#define KTD2052_REG_PG_RGB4                       0x14
#define KTD2052_REG_WD                            0x15

cy_rslt_t led_driver_write_reg(uint8_t reg, uint8_t val);
cy_rslt_t led_driver_read_reg(uint8_t reg, uint8_t *val);
cy_rslt_t led_driver_reset(void);
// void task_fram_status(void *param);


extern char led_commands[4][100];



#endif /* IO_EXPANDER_H_ */
