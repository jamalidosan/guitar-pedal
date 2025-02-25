/*
 * procsensor.h
 *
 * Created on: April 18, 2024
 *      Co-Authors: Hernan Carranza, Jonathan Amalidosan, James Hunt
 */

/******************************************************************************
 * Include guard
 *****************************************************************************/
#ifndef PROCSENSOR_H_
#define PROCSENSOR_H_

#include "main.h"
// #include "i2c.h"

/*******************************************************************************
* Macros
********************************************************************************/

#define PROX_SENSOR_SUBORDINATE_ADDR                 0x5A //refer to page 14

/// I2C Registers   ///
#define PART_ID         0x00 
#define INT_CFG         0x03 
#define IRQ_ENABLE      0x04 
#define IRQ_MODE1       0x05
#define IRQ_MODE2       0x06
#define HW_KEY          0x07
#define MEAS_RATE       0x08
#define ALS_RATE        0x09
#define PS_RATE         0x0A 
#define ALS_LOW_TH0     0x0B 
#define ALS_LOW_TH1     0x0C
#define ALS_HI_TH0      0x0D 
#define ALS_HI_TH1      0x0E 
#define PS_LED21        0x0F  //Note: 4 LSB used for LED 1... Use only this?
#define PS1_TH0         0x11
#define PS1_TH1         0x12
#define PARAM_WR        0x17
#define COMMAND         0x18
#define RESPONSE        0x20
#define IRQ_STATUS      0x21
#define ALS_IR_DATA0    0x24
#define ALS_IR_DATA1    0x25
#define PS1_DATA0       0x26
#define PS1_DATA1       0x27
#define PARAM_RD        0x2E 

#define PIN_PROX_SCL			P5_0
#define PIN_PROX_SDA			P5_1
#define PIN_PROX_INT	        P5_3	


/// COMMAND register commands ///

/*These don't cares represent the actual offset of the parameter*/

// #define PARAM_QUERY    //0b100xxxxx
// #define PARAM_SET      //0b101xxxxx
// #define PARAM_AND      //0b110xxxxx
// #define PARAM_OR       //0b111xxxxx
#define NOP            0x00
#define RESET          0x01
#define BUSADDR        0x02
#define PS_FORCE       0x05
#define ALS_FORCE      0x06
#define PSALS_FORCE    0x07
#define PS_PAUSE       0x09
#define ALS_PAUSE      0x0A
#define PSALS_PAUSE    0x0B
#define PS_AUTO        0x0D
#define ALS_AUTO       0x0E
#define PSALS_AUTO     0x0F

//Response Error Codes//
#define NO_ERROR               0x00 //0x0x
#define INVALID_SETTING        0x80
#define PS1_ADC_OVERFLOW       0x88
// #define PS2_ADC_OVERFLOW       0x89 don't need these only one PS
// #define PS3_ADC_OVERFLOW       0x8A
#define ALS_VIS_ADC_OVERFLOW   0x8C
#define ALS_IR_ADC_OVERFLOW    0x8D
#define AUX_ADC_OVERFLOW       0x8E

// Parameters Offsets (Not Directly Addressable Over I2C)

// #define 0x00
// #define 0x01 
// #define 0x02
// #define 0x03
// #define 0x04
// #define 0x05
// #define 0x06
// #define 0x07
// #define 0x08
// #define 0x09
// #define 0x0A
// 0x0B
// 0x0C
// 0x0E

/*******************************************************************************
* Public API
********************************************************************************/

/** Initialize the I2C bus to the proximity sensor module site
 *
 * @param - None
 */
cy_rslt_t i2c_prox_init(void);

cy_rslt_t int_prox_init(void);

cy_rslt_t prox_init(void);

cy_rslt_t prox_open_drain(void);

cy_rslt_t set_autonomous(void);

cy_rslt_t set_rates(void);

cy_rslt_t always_active(void);

cy_rslt_t led_current(void);

cy_rslt_t enable_ps1(void);

cy_rslt_t ps1_pause(void);

cy_rslt_t prox_sensor_write_reg(uint8_t reg, uint8_t val);

uint8_t prox_sensor_read_reg(uint8_t reg, uint8_t *val);

uint16_t read_measurement(void);

void print_reg_prox_val(uint8_t reg, uint8_t *val);

void task_read_motion(void);

#endif
/* END OF FILE [] */