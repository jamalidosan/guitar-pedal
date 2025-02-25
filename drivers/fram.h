#include "main.h"

#ifndef FRAM_H_
#define FRAM_H_

#define FRAM_CS_1_PIN					P10_3
#define FRAM_CS_2_PIN					P10_4
#define FRAM_SCLK                       P10_2
#define FRAM_MISO                       P10_1
#define FRAM_MOSI                       P10_0

#define FRAM_CMD_WREN					0x06    // write enable
#define FRAM_CMD_WRDI					0x04    // reset write enable latch
#define FRAM_CMD_RDSR					0x05    // read status reg
#define FRAM_CMD_WRSR					0x01    // write status reg
#define FRAM_CMD_READ					0x03    // read memory data
#define FRAM_CMD_WRITE				    0x02    // write memory data
#define FRAM_CMD_RDID					0x9f    // read device ID   

#define FRAM_CFG_SR                     0x02    // disables block protect and write protect enable

/** Initializes the IO pins used to control the CS of the
 *  EEPROM
 *
 * @param
 *
 */
cy_rslt_t fram_cs_1_init(void);

/** Initializes the IO pins used to control the CS of the
 *  EEPROM
 *
 * @param
 *
 */
cy_rslt_t fram_cs_2_init(void);

/** Initializes the status register of the
 *  FRAM for Chip Select 1
 *
 * @param
 *
 */
cy_rslt_t fram_status_reg_init(void);

/** Determine if the EEPROM is busy writing the last
 *  transaction to non-volatile storage
 *
 * @param
 *
 */
cy_rslt_t fram_wait_for_write(uint8_t CS);

/** Enables Writes to the FRAM
 *
 * @param CS - The FRAM Chip to enable
 *
 */
cy_rslt_t fram_write_enable(uint8_t CS);

/** Disable Writes to the EEPROM
 *
 * @param
 *
 */
cy_rslt_t fram_write_disable(uint8_t CS);


/*
 * This is our main function to write ADC values to FRAM
 * when recording. We automatically choose to write to FRAM1
 * first and switch to FRAM2 when memory is full in FRAM1. 
 * We write 2 packets to FRAM since ADC values are 16-bits and
 * memory locations are only 8-bits wide.
 * @param address - the 20-bit address (0x00000 - 0xFFFFF) to write to in memory,
 * only the lowest 20-bits are used and the remaining upper
 * bits are treated as don't-cares.
 * @param data - the data to write in the passed address
 * 
 * @return CY_RSLT_SUCCESS on a successful write, any other return
 * value is an error.
 */
cy_rslt_t fram_memory_write(uint32_t address, uint16_t data);

/*
 * This is our main function to read ADC values to FRAM
 * when using playback. We read from FRAM1 first and then switch
 * to FRAM2 if necessary. 
 * We read 2 packets from FRAM since ADC values are 16-bits and
 * memory locations are only 8-bits wide.
 * @param address - the 20-bit address (0x00000 - 0xFFFFF) to read from memory,
 * only the lowest 20-bits are used and the remaining upper
 * bits are treated as don't-cares.
 * @param data - the address of the variable to store read data from address
 * 
 * @return CY_RSLT_SUCCESS on a successful write, any other return
 * value is an error.
 */
cy_rslt_t fram_memory_read(uint32_t address, uint16_t *data, uint8_t CS);

/** Tests Writing and Reading the EEPROM
 *
 * @param
 *
 */
cy_rslt_t fram_full_test(void);

extern QueueHandle_t Queue_FRAM_to_DAC;
extern TaskHandle_t TaskHandle_FRAM_OP;
void task_fram_operation(void *param);

void task_fram_testing(void *param);

#endif /* FRAM_H_ */