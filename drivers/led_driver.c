/**
 * @file led_driver.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2024-02-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "i2c.h"
#include "led_driver.h"
/* Packet positions */

// volatile bool REACH = false;

TaskHandle_t LED_Task;

#define PACKET_SIZE             (2)



//turn watchdog off

cy_rslt_t led_driver_reset()
{	
	uint8_t write_buffer[PACKET_SIZE];
	write_buffer[0] = (uint8) KTD2052_REG_IRED1;
	write_buffer[1] = (uint8) 0;
	cyhal_i2c_master_write(&i2c_master_obj, LED_DRIVER_SUBORDINATE_ADDR , write_buffer, PACKET_SIZE, 0, true);

	write_buffer[0] = (uint8) KTD2052_REG_IRED2;
	cyhal_i2c_master_write(&i2c_master_obj, LED_DRIVER_SUBORDINATE_ADDR , write_buffer, PACKET_SIZE, 0, true);
	
	write_buffer[0] = (uint8) KTD2052_REG_IRED3;
	cyhal_i2c_master_write(&i2c_master_obj, LED_DRIVER_SUBORDINATE_ADDR , write_buffer, PACKET_SIZE, 0, true);

	write_buffer[0] = (uint8) KTD2052_REG_IGRN1;
	cyhal_i2c_master_write(&i2c_master_obj, LED_DRIVER_SUBORDINATE_ADDR , write_buffer, PACKET_SIZE, 0, true);

	write_buffer[0] = (uint8) KTD2052_REG_IGRN2;
	cyhal_i2c_master_write(&i2c_master_obj, LED_DRIVER_SUBORDINATE_ADDR , write_buffer, PACKET_SIZE, 0, true);

	write_buffer[0] = (uint8) KTD2052_REG_IGRN3;
	cyhal_i2c_master_write(&i2c_master_obj, LED_DRIVER_SUBORDINATE_ADDR , write_buffer, PACKET_SIZE, 0, true);
	
	write_buffer[0] = (uint8) KTD2052_REG_IBLU1;
	cyhal_i2c_master_write(&i2c_master_obj, LED_DRIVER_SUBORDINATE_ADDR , write_buffer, PACKET_SIZE, 0, true);

	write_buffer[0] = (uint8) KTD2052_REG_IBLU2;
	cyhal_i2c_master_write(&i2c_master_obj, LED_DRIVER_SUBORDINATE_ADDR , write_buffer, PACKET_SIZE, 0, true);

	write_buffer[0] = (uint8) KTD2052_REG_IBLU3;
	cyhal_i2c_master_write(&i2c_master_obj, LED_DRIVER_SUBORDINATE_ADDR , write_buffer, PACKET_SIZE, 0, true);

	write_buffer[0] = (uint8) KTD2052_REG_IRED4;
	cyhal_i2c_master_write(&i2c_master_obj, LED_DRIVER_SUBORDINATE_ADDR , write_buffer, PACKET_SIZE, 0, true);
	write_buffer[0] = (uint8) KTD2052_REG_IBLU4;
	cyhal_i2c_master_write(&i2c_master_obj, LED_DRIVER_SUBORDINATE_ADDR , write_buffer, PACKET_SIZE, 0, true);
	write_buffer[0] = (uint8) KTD2052_REG_IGRN3;
	cyhal_i2c_master_write(&i2c_master_obj, LED_DRIVER_SUBORDINATE_ADDR , write_buffer, PACKET_SIZE, 0, true);

	// write_buffer[0] = (uint8) KTD2052_REG_PG_FADE;
	// cyhal_i2c_master_write(&i2c_master_obj, LED_DRIVER_SUBORDINATE_ADDR , write_buffer, PACKET_SIZE, 0, true);
	write_buffer[0] = (uint8) KTD2052_REG_WD;
	write_buffer[1] = (uint8) 255;
	cyhal_i2c_master_write(&i2c_master_obj, LED_DRIVER_SUBORDINATE_ADDR , write_buffer, PACKET_SIZE, 0, true);




	return NULL;	
}



cy_rslt_t led_driver_write_reg(uint8_t reg, uint8_t val)
{
	uint8_t write_buffer[PACKET_SIZE];
	write_buffer[0] = reg;
	write_buffer[1] = val; 

	cyhal_i2c_master_write(&i2c_master_obj,
	LED_DRIVER_SUBORDINATE_ADDR , 
	write_buffer, // D300
	PACKET_SIZE,
	0, // wait indefintely
	true // send stop
	);
	return NULL;
		
}

cy_rslt_t led_driver_read_reg(uint8_t reg, uint8_t *val)
{
	uint8_t write_buffer[1];
	write_buffer[0] = reg;

	uint8_t read_buffer[1];

	cyhal_i2c_master_write(&i2c_master_obj,
	LED_DRIVER_SUBORDINATE_ADDR, 
	write_buffer,
	1,
	0,
	false
	);

	cyhal_i2c_master_read(&i2c_master_obj,
	LED_DRIVER_SUBORDINATE_ADDR,
	read_buffer,
	1,
	0,
	true
	);
	*val = read_buffer[0];
	print_reg_val(reg, val); // for debugging print statement and read demo
	return NULL;
}

void print_reg_val(uint8_t reg, uint8_t *val) {
	printf("LED value in reg %i is %i\n\r", reg, *val);
}

/*
	gets task notification from fram notifying when capacity taken passes threshold
	
	checks if/case statement to see what percentage it is at
		25% = half of cs one 524288
		50% = chip select one 1048576
		75% = half of cs two
		100% = both chip selects 2097152
	lights up corresponding light
	stays on
	keeps on going until it reaches full capacity
*/
// void task_fram_status(void *param)
// {
// 	(void)param;
// 	BaseType_t status;
// 	// uint32_t addr;
// 	// bool full;
// 	uint8_t light = 0;

	
// 	status = xQueueReceive(LED_On, &light, portMAX_DELAY);
// 	// printf("entered task \r\n");

// 	if(light == 0x01)
// 	{
// 		led_driver_write_reg((uint8) KTD2052_REG_IRED1, (uint8) 192);
// 		// printf("light one \r\n");
		
// 	}
// 	else if(light == 0x02)
// 	{
// 		led_driver_write_reg((uint8) KTD2052_REG_IRED2, (uint8) 192);
// 		// printf("light two \r\n");	
// 	}
// 	else if(light == 0x03)
// 	{
// 		led_driver_write_reg((uint8) KTD2052_REG_IRED3, (uint8) 192);
// 		// printf("light three \r\n");	
// 	}
// 	else if(light == 0x04)
// 	{
// 		led_driver_write_reg((uint8) KTD2052_REG_IRED4, (uint8) 192);
// 		// printf("light four \r\n");	
// 	}
	


// }
