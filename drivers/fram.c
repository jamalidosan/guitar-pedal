#include "fram.h"
#include "i2c.h"
#include "led_driver.h"

/*
    Reminider that memory is 1024K * 8 bits
    So if we write 8 bits 1024K times, then we know to change CS
*/

//TaskHandle_t Fram_Task;
volatile bool CS_1_DATA_FULL = false;
volatile bool CS_2_DATA_FULL = false;
volatile uint64_t count = 0;
TaskHandle_t Fram_Task;
TaskHandle_t TaskHandle_FRAM_OP;

QueueHandle_t Queue_FRAM_to_DAC;

#if (CS_1_DATA_FULL)
    #define FRAM_CS_PIN FRAM_CS_2_PIN
#else  
    #define FRAM_CS_PIN FRAM_CS_1_PIN
#endif


/*
 * Initialzw the GPIO Pin on the PSOC to use
 * for our chip select for FRAM1
 * @return CYHAL_RSLT_SUCCESS on successful
 * init, returns cyhal error code otherwise.
 */
cy_rslt_t fram_cs_1_init(void)
{
    return cyhal_gpio_init(
			FRAM_CS_1_PIN,              // Pin
			CYHAL_GPIO_DIR_OUTPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			true);		
}

/*
 * Initialzw the GPIO Pin on the PSOC to use
 * for our chip select for FRAM2
 * @return CYHAL_RSLT_SUCCESS on successful
 * init, returns cyhal error code otherwise.
 */
cy_rslt_t fram_cs_2_init(void)
{
    return cyhal_gpio_init(
			FRAM_CS_2_PIN,              // Pin
			CYHAL_GPIO_DIR_OUTPUT,      // Direction
			CYHAL_GPIO_DRIVE_STRONG,    // Drive Mode
			true);
}

/*
 * This function initializes FRAM1 and FRAM2
 * to have no protected blocks of memory and disable
 * write protects.
 * @return CY_RSLT_SUCCESS on successful transactions,
 * returns cyhal error code if an error occurs.
 */
cy_rslt_t fram_status_reg_init(void)
{
	cy_rslt_t   rslt;
	uint8_t     transmit_data[2];
	uint8_t     receive_data[2];

	/////////////////////////////////////////
	///// Set status register of FRAM 1 /////
	/////////////////////////////////////////

	// Wait for any outstanding writes to complete
	rslt = fram_wait_for_write(FRAM_CS_1_PIN);
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}

	// Enable writes to the fram 1
	rslt = fram_write_enable(FRAM_CS_1_PIN);
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}

	transmit_data[0] = FRAM_CMD_WRSR;
	transmit_data[1] = FRAM_CFG_SR;

	// Set the CS1 Low
	cyhal_gpio_write(FRAM_CS_1_PIN, 0);

	// Write configuration to status register
	rslt = cyhal_spi_transfer(
			&mSPI,
			transmit_data,
			2u,
			receive_data,
			2u,
			0xFF
		);

	// Set CS1 High
	cyhal_gpio_write(FRAM_CS_1_PIN, 1);

	// Disable writes to FRAM1
	rslt = fram_write_disable(FRAM_CS_1_PIN);
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}	

	fram_wait_for_write(FRAM_CS_1_PIN);
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}

	transmit_data[0] = FRAM_CMD_RDSR;
	
	// Set the CS1 Low
	cyhal_gpio_write(FRAM_CS_1_PIN, 0);

	// Read status register to confirm status reg value is correct
	rslt = cyhal_spi_transfer(
		&mSPI,
		transmit_data,
		2u,
		receive_data,
		2u,
		0xFF
	);

	// Set the CS1 High
	cyhal_gpio_write(FRAM_CS_1_PIN, 1);

	// check that status register set correctly
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}
	else if(receive_data[1] != 0x40)
	{
		printf("error: status register data for FRAM 1 is incorrect \r\n");
		printf("data received: %d\r\n", receive_data[0]);
		return -1;
	}

	/////////////////////////////////////////
	///// Set status register of FRAM 2 /////
	/////////////////////////////////////////

	// Wait for any outstanding writes to complete
	rslt = fram_wait_for_write(FRAM_CS_2_PIN);
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}

	// Enable writes to the fram
	rslt = fram_write_enable(FRAM_CS_2_PIN);
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}

	transmit_data[0] = FRAM_CMD_WRSR;
	transmit_data[1] = FRAM_CFG_SR;

	// Set the CS1 Low
	cyhal_gpio_write(FRAM_CS_2_PIN, 0);

	// Write to status register
	// no memory block protects
	rslt = cyhal_spi_transfer(
			&mSPI,
			transmit_data,
			2u,
			receive_data,
			2u,
			0xFF
		);

	// Set the CS2 High
	cyhal_gpio_write(FRAM_CS_2_PIN, 1);

	// Disable writes to FRAM2
	rslt = fram_write_disable(FRAM_CS_2_PIN);
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}	

	fram_wait_for_write(FRAM_CS_2_PIN);
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}

	transmit_data[0] = FRAM_CMD_RDSR;
	
	// Set the CS2 Low
	cyhal_gpio_write(FRAM_CS_2_PIN, 0);

	// read status register to confirm status reg value is correct
	rslt = cyhal_spi_transfer(
		&mSPI,
		transmit_data,
		2u,
		receive_data,
		2u,
		0xFF
	);

	// Set the CS2 High
	cyhal_gpio_write(FRAM_CS_2_PIN, 1);

	// check that status register set correctly
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}
	else if(receive_data[1] != 0x40)
	{
		printf("error: status register data for FRAM 2 is incorrect \r\n");
		printf("data received: %d\r\n", receive_data[0]);
		return -1;
	}

	return rslt;
}

/*
 * This function reads the status register of FRAM_CS
 * to provide a buffer between writes.
 * @param CS - the FRAM Chip to wait for write to complete
 * @returns CYHAL_RSLT_SUCCESS on successful read, returns
 * cyhal error code otherwise.
 */
cy_rslt_t fram_wait_for_write(uint8_t CS)
{
	uint8_t     transmit_data[2] = {FRAM_CMD_RDSR, 0xFF};
	uint8_t     receive_data[2] = {0x00, 0x00};
	cy_rslt_t   rslt;

	  // Check to see if the fram is still updating
	  // the data from the last write
	  do
	  {
		// Set the CS Low
		cyhal_gpio_write(CS, 0);

		// Starts a data transfer
		rslt = cyhal_spi_transfer(
				&mSPI,
				transmit_data,
				2u,
				receive_data,
				2u,
				0xFF
			);

		// Set the CS High
		cyhal_gpio_write(CS, 1);

		if (rslt != CY_RSLT_SUCCESS)
		{
			return rslt;
		}

	    // If the address was not ACKed, try again.
	  } while ((receive_data[1] & 0x01) != 0);

	  return CY_RSLT_SUCCESS;
}

/*
 * Sets write enable latch on FRAM_CS to allow
 * subsequent write operations to status registers
 * and memory.
 * @param CS - The FRAM Chip to enable writes to
 * @return CYHAL_RSLT_SUCCESS on successful write,
 * returns cyhal error code otherwise.
 */
cy_rslt_t fram_write_enable(uint8_t CS)
{
	uint8_t     transmit_data[1] = {FRAM_CMD_WREN};
	uint8_t     receive_data[1] = {0x00};
	cy_rslt_t   rslt;


	// Set the CS Low
	cyhal_gpio_write(CS, 0);

	// Starts a data transfer
	rslt = cyhal_spi_transfer(
				&mSPI,
				transmit_data,
				1u,
				receive_data,
				1u,
				0xFF
			);

	// Set the CS High
	cyhal_gpio_write(CS, 1);

  return rslt;
}

/*
 * Resets write enable latch on FRAM_CS to disallow
 * subsequent write operations to status registers
 * and memory.
 * @param CS - The FRAM Chip to disable writes to
 * @return CYHAL_RSLT_SUCCESS on successful write,
 * returns cyhal error code otherwise.
 */
cy_rslt_t fram_write_disable(uint8_t CS)
{
	uint8_t     transmit_data[1] = {FRAM_CMD_WRDI};
	uint8_t     receive_data[1] = {0x00};
	cy_rslt_t   rslt;


	// Set the CS Low
	cyhal_gpio_write(CS, 0);

	// Starts a data transfer
	rslt = cyhal_spi_transfer(
				&mSPI,
				transmit_data,
				1u,
				receive_data,
				1u,
				0xFF
			);

	// Set the CS High
	cyhal_gpio_write(CS, 1);

  return rslt;
}

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
cy_rslt_t fram_memory_write(uint32_t address, uint16_t data)
{
	// bound check the address
    if(address > 0xFFFFF)
    {
        address = 0xFFFFF;
    }

	uint8_t     transmit_data[6];
	uint8_t     receive_data[6];
	cy_rslt_t   rslt;

	// Wait for any outstanding writes to complete
	rslt = fram_wait_for_write(FRAM_CS_PIN);
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}

	// Enable writes to the fram
	rslt = fram_write_enable(FRAM_CS_PIN);
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}

	//transmit data 
	// index 0: write command
	// index 1: msb of address (19 to 16): top 4 bits don't care about
	// index 2: bits 15 to 8
	// index 3: bits 7 to 0
	// index 4: MSB's of data to be transmitted
	// index 5: LSB's of data to be transmitted
	transmit_data[0] = FRAM_CMD_WRITE;
	transmit_data[1] = (uint8_t)((address & 0x00ff0000U) >> 16);
	transmit_data[2] = (uint8_t)((address & 0x0000ff00U) >> 8);
	transmit_data[3] = (uint8_t)(address & 0x000000ffU);
	transmit_data[4] = (uint8_t)(data >> 8);
	transmit_data[5] = (uint8_t)(data);

	// Set the CS Low
	cyhal_gpio_write(FRAM_CS_PIN, 0);

	// Starts a data transfer
	rslt = cyhal_spi_transfer(
			&mSPI,
			transmit_data,
			6u,
			receive_data,
			6u,
			0xFF
		);

	// Set the CS High
	cyhal_gpio_write(FRAM_CS_PIN, 1);


	// Disable writes to the fram
	rslt = fram_write_disable(FRAM_CS_PIN);
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}

	return CY_RSLT_SUCCESS;
}

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
cy_rslt_t fram_memory_read(uint32_t address, uint16_t *data, uint8_t CS)
{

    if(address > 0xFFFFF)
    {
        address = 0xFFFFF;
    }

	uint8_t     transmit_data[6];
	uint8_t     receive_data[6];
	cy_rslt_t   rslt;

	// Wait for any outstanding writes to complete
	rslt = fram_wait_for_write(FRAM_CS_PIN);
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}

	// Enable writes to the fram
	rslt = fram_write_enable(CS);
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}

	transmit_data[0] = FRAM_CMD_READ;
	transmit_data[1] = (uint8_t)((address & 0x00ff0000U) >> 16);
	transmit_data[2] = (uint8_t)((address & 0x0000ff00U) >> 8);
	transmit_data[3] = (uint8_t)(address & 0x000000ffU);
	transmit_data[4] = 0x00u;
	transmit_data[5] = 0x00u;

	// Set the CS Low
	cyhal_gpio_write(CS, 0);

	// Starts a data transfer
	rslt = cyhal_spi_transfer(
			&mSPI,
			transmit_data,
			6u,
			receive_data,
			6u,
			0xFF
		);

	// Set the CS High
	cyhal_gpio_write(CS, 1);

	// Return the value from the fram to the user
	*data |= (uint16_t)(receive_data[4] << 8);
	*data |= (uint16_t)(receive_data[5]);

	// Disable writes to the fram
	rslt = fram_write_disable(FRAM_CS_PIN);
	if(rslt != CY_RSLT_SUCCESS)
	{
		return rslt;
	}

	return CY_RSLT_SUCCESS;
}

/*
 * Our main task for operating FRAM reads and writes.
 * @return none
 */
void task_fram_operation(void *param)
{
	(void)param;	// ignore param
	uint32_t addr = 0x00000u;
	cy_rslt_t rslt;
	BaseType_t rtos_rslt;
	uint16_t data = 0x0000u;

	// looping task code
	while(1)
	{
		// store ADC values while we are recording audio
		while(RECORDING == true)
		{
			rtos_rslt = xQueueReceive(Queue_ADC_Results, &data, portMAX_DELAY);
			if (rtos_rslt != pdTRUE) 
			{
				printf("ERROR in receiving Queue_ADC_Results!\r\n");
			}
			rslt = fram_memory_write(addr, data);
			if (rslt != CY_RSLT_SUCCESS)
			{
				printf("ERROR writing to FRAM!\r\n");
			}
			addr += 2;
		}
		// on playback, return and loop recored audio.
		while((RECORDING == false) /* && (PLAYBACK == TRUE)*/)
		{
			addr = 0x00000u;
			// grab data from FRAM
			fram_memory_read(addr, &data, FRAM_CS_1_PIN);
			// send data to DAC
			rtos_rslt = xQueueSendToBack(Queue_FRAM_to_DAC, &data, portMAX_DELAY);
			if (rtos_rslt != pdTRUE)
			{
				printf("ERROR sending read FRAM data to FRAM_to_DAC queue!)\r\n");
			}

		}
	}
}

/*
 * Tester function to validate that adc values are properly
 * sent to and recieved from FRAM
 * @param none
 */
void test_fram_adc()
{
	BaseType_t rtos_rslt;
	cy_rslt_t rslt;
	uint32_t addr = 0x00000u;
	uint16_t data = 0x0000u;
	printf("***FRAM R/W WITH ADC TEST ***\r\n");

	/////////////////////////
	///// FRAM 1 Test	/////
	/////////////////////////

	// pull items from the queue
	rtos_rslt = xQueueReceive(Queue_ADC_Results, &data, portMAX_DELAY);
	printf("ADC Data Recieved: %d \n\r", data);
	if (rtos_rslt != pdTRUE) 
	{
		printf("ERROR in receiving Queue_ADC_Results!\r\n");
	}
	// send them to fram1
	rslt = fram_memory_write(addr, &data);
	if(rslt != CY_RSLT_SUCCESS) 
	{
		printf("ERROR sending ADC data to FRAM_1!\n\r");
	}
	// read them back
	rslt = fram_memory_read(addr, &data, FRAM_CS_1_PIN);
	if(rslt != CY_RSLT_SUCCESS) 
	{
		printf("ERROR sending ADC data to FRAM_1!\n\r");
	}
	printf("Value read from FRAM1: %d\n\r", data);

	/////////////////////////
	///// FRAM 2 Test	/////
	/////////////////////////


}

void task_fram_testing(void *param)
{
	(void)param;	// shut up compiler
	
	fram_status_reg_init();

	//////////////////////////
	///// test_fram here /////
	test_fram_adc();

	// BaseType_t status;
	// uint32_t addr;
	// cy_rslt_t rslt;
	// uint8_t data;
	// // bool full;
	// // uint8_t light;

	// // Write the data to the fram.
	// addr = 0x00000u;
	// data = 0x01u;

	// // printf("starting task \r\n");

	// // Looping task code
	// while(1)
	// {
	// 	while(!CS_2_DATA_FULL)
	// 	{
			
	// 		// light = 0x00;
	// 		rslt = fram_memory_write(addr, data);

	// 		if(addr == 0x80000)
	// 		{
	// 			if(!CS_1_DATA_FULL)
	// 			{
	// 				// printf("light one \n\r");
	// 				led_driver_write_reg((uint8) KTD2052_REG_IRED1, (uint8) 192);
	// 			}
	// 			else
	// 			{
	// 				// printf("light three \n\r");
	// 				led_driver_write_reg((uint8) KTD2052_REG_IRED3, (uint8) 192);
	// 			}
	// 		}
	// 		else if(addr == 0xfffff)
	// 		{
	// 			if(!CS_1_DATA_FULL)
	// 			{
	// 				// printf("light two \n\r");
	// 				led_driver_write_reg((uint8) KTD2052_REG_IRED2, (uint8) 192);
	// 				CS_1_DATA_FULL = true;
	// 			}
	// 			else
	// 			{
	// 				// printf("light four \n\r");
	// 				led_driver_write_reg((uint8) KTD2052_REG_IRED4, (uint8) 192);
	// 				CS_2_DATA_FULL = true;
	// 			}
	// 		}
			
	// 		// if(addr == 0xfffff && (!CS_1_DATA_FULL))
	// 		// {
	// 		// 	CS_1_DATA_FULL = true;
	// 		// }
	// 		// else if(addr == 0xfffff && (CS_1_DATA_FULL))
	// 		// {
	// 		// 	CS_2_DATA_FULL = true;
	// 		// }

	// 		addr++;

	// 		if (addr > 0xFFFFF)
	// 		{
	// 			addr = 0x00000;
	// 		}


	// 		// if(light != 0x00)
	// 		// {
	// 		// 	status = xQueueSendToBack(LED_On, &light, portMAX_DELAY);

	// 		// 	if(status != pdTRUE)
	// 		// 	{
	// 		// 		while(1){}
					
	// 		// 	}
	// 		// 	vTaskDelay(pdMS_TO_TICKS(1));
	// 		// }

			
			
	// 	}
	// }
}
