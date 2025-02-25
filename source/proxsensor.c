/*
 * procsensor.h
 *
 * Created on: April 18, 2024
 *      Co-Authors: Hernan Carranza, Jonathan Amalidosan, James Hunt
 */

#include "proxsensor.h"

TaskHandle_t Prox_Task;

cyhal_i2c_t i2c_master_prox_obj;

// uint16_t PS1_THRESHOLD = 46000;

// Define the I2C monarch configuration structure
cyhal_i2c_cfg_t i2c_prox_master_config =
{
    CYHAL_I2C_MODE_MASTER,
    0, // address is not used for master mode
    I2C_MASTER_FREQUENCY
};

/** Initialize the I2C bus to the proximity sensor module site
 *
 * @param - None
 */
cy_rslt_t i2c_prox_init(void)
{
	cy_rslt_t rslt;

    // Initialize I2C monarch, set the SDA and SCL pins and assign a new clock
	rslt = cyhal_i2c_init(&i2c_master_prox_obj, PIN_PROX_SDA, PIN_PROX_SCL, NULL);

	if(rslt != CY_RSLT_SUCCESS)
	{
	    CY_ASSERT(0);

	    while(1){};
	}

    // Configure the I2C resource to be monarch
	rslt = cyhal_i2c_configure(&i2c_master_prox_obj, &i2c_prox_master_config);

	if(rslt != CY_RSLT_SUCCESS)
	{
	    CY_ASSERT(0);

	    while(1){};
	}


	return CY_RSLT_SUCCESS;
}



cy_rslt_t prox_sensor_write_reg(uint8_t reg, uint8_t val)
{

	cy_rslt_t rslt;
	uint8_t write_buffer[2];
	write_buffer[0] = reg;
	write_buffer[1] = val;

	

	rslt =    	cyhal_i2c_master_write(&i2c_master_prox_obj,
	        	(uint8) PROX_SENSOR_SUBORDINATE_ADDR, 
				write_buffer, // D300
				2,
				0, // wait indefintely
				true // send stop
				);

	return rslt;
		
}


uint8_t prox_sensor_read_reg(uint8_t reg, uint8_t *val)
{

	cy_rslt_t rslt;

	uint8_t write_buffer[1];
	write_buffer[0] = reg;

	uint8_t read_buffer[1];

	rslt = cyhal_i2c_master_write(&i2c_master_prox_obj,
	(uint8) PROX_SENSOR_SUBORDINATE_ADDR, 
	write_buffer,
	1,
	0,
	false
	);

	rslt = cyhal_i2c_master_read(&i2c_master_prox_obj,
	(uint8) PROX_SENSOR_SUBORDINATE_ADDR,
	read_buffer,
	1,
	0,
	true
	);
	
	*val = read_buffer[0];

	// print_reg_prox_val(reg, val); // for debugging print statement and read demo

	//have it so that it returns int instead for PS purposes
	return *val;
}


cy_rslt_t prox_init(void)
{
	cy_rslt_t rslt;

	//Give 250 milliseconds before any I2C operations can take place
	cyhal_system_delay_ms(250);
	uint8_t *val;

	rslt = prox_sensor_write_reg((uint8) HW_KEY, (uint8) 0x17);


	return rslt;
}

cy_rslt_t led_current(void)
{

	printf("driving LED current \n\r");

	uint8_t *val;
	cy_rslt_t rslt;

	// Give the max current to LED 1
	rslt = prox_sensor_write_reg((uint8) PS_LED21, (uint8) 0x0F);

	return rslt;
}


cy_rslt_t set_rates(void)
{
	printf("configuring measure rate and ps rate \n\r");
	cy_rslt_t rslt;

	//Device wakes up every 10 ms
	rslt = prox_sensor_write_reg((uint8)MEAS_RATE, (uint8)0x84);

	//Everytime the device wakes up, we should measure
	//(0x0001 x timeValueOf(MEAS_RATE))
	rslt = prox_sensor_write_reg((uint8)PS_RATE, (uint8)0x08);
	return rslt;
}

cy_rslt_t always_active(void)
{
	// NEED TO FIX 1B is parameter not register
	printf("setting to always active mode \n\r");
	cy_rslt_t rslt;
	uint8_t *val;

	rslt = prox_sensor_read_reg((uint8)PARAM_RD, &val);
	//value that we will set param 0x1B to
	rslt = prox_sensor_write_reg((uint8)PARAM_WR, (uint8)0x00);
	rslt = prox_sensor_write_reg((uint8)COMMAND, (uint8)NOP);
	rslt = prox_sensor_read_reg((uint8)RESPONSE, &val);
	// COMMAND Should be param set (101 -> PARAM_SET 11011 -> CHLIST)
	// 1B
	rslt = prox_sensor_write_reg((uint8)COMMAND, (uint8)0xBB);
	rslt = prox_sensor_read_reg((uint8)RESPONSE, &val);
	rslt = prox_sensor_read_reg((uint8)PARAM_RD, &val);

	return rslt;
}



static void ps1_int_enable(void)
{

	// Have interrupt set when PS1 is above threshold
	// Do we need command interrupt enabled?
	prox_sensor_write_reg((uint8)IRQ_ENABLE, (uint8)0x04);
	prox_sensor_write_reg((uint8)IRQ_MODE1, (uint8)0x30);

	prox_sensor_write_reg((uint8)INT_CFG, (uint8)0x01);
}




cy_rslt_t set_autonomous(void)
{

	cy_rslt_t rslt;

	uint8_t *val;
	// bool zero;

	rslt = prox_sensor_write_reg((uint8)COMMAND, (uint8)NOP);
	// while(!zero)
	// {
	rslt = prox_sensor_read_reg((uint8)RESPONSE, &val);
		// if(&val == 0)
		// {
		// 	zero = true;
		// }
	// }
	rslt = prox_sensor_write_reg((uint8)COMMAND, (uint8)PS_AUTO);
	rslt = prox_sensor_read_reg((uint8)RESPONSE, &val);

	return rslt;
}

uint16_t set_threshold(void)
{

	// TODO: Assert INT

	uint8_t *val;
	uint8_t lsb;
	uint8_t msb;
	uint16_t threshold;
	
	prox_sensor_write_reg((uint8)PS1_TH1, 0x80);
	prox_sensor_write_reg((uint8)PS1_TH0, 0xE8);

	msb = prox_sensor_read_reg((uint8)PS1_TH1, &val);
	lsb = prox_sensor_read_reg((uint8)PS1_TH0, &val);

	// PS1 = ReadFromRegister(REG_PS1_DATA0) + 256 * ReadFromRegister(REG_PS1_DATA1); 
	threshold = (uint16_t) lsb + (256 * ((uint16_t) msb));

	// printf("Proximity Value: %d \n\r", PS1);

	return threshold;

}

uint16_t read_measurement(void)
{

	// TODO: Assert INT

	uint8_t *val;
	uint8_t lsb;
	uint8_t msb;
	uint16_t PS1;
	
	// make sure it's 0x00
	msb = prox_sensor_read_reg((uint8)PS1_DATA1, &val);
	lsb = prox_sensor_read_reg((uint8)PS1_DATA0, &val);

	// PS1 = ReadFromRegister(REG_PS1_DATA0) + 256 * ReadFromRegister(REG_PS1_DATA1); 
	PS1 = (uint16_t) lsb + (256 * ((uint16_t) msb));

	// printf("Proximity Value: %d \n\r", PS1);

	return PS1;

}

cy_rslt_t enable_ps1(void)
{
	// printf("writing to CHLIST \n\r");
	cy_rslt_t rslt;
	uint8_t *val;
	rslt = prox_sensor_read_reg((uint8)PARAM_RD, &val);
	rslt = prox_sensor_write_reg((uint8)PARAM_WR, (uint8)0x01);
	rslt = prox_sensor_write_reg((uint8)COMMAND, NOP);
	rslt = prox_sensor_read_reg((uint8)RESPONSE, &val);
	// COMMAND Should be param set (101 -> PARAM_SET 00001 -> CHLIST)
	// 0xA1
	rslt = prox_sensor_write_reg((uint8)COMMAND, (uint8)0xA1);
	rslt = prox_sensor_read_reg((uint8)RESPONSE, &val);
	rslt = prox_sensor_read_reg((uint8)PARAM_RD, &val);

}

cy_rslt_t ps1_pause(void)
{
	uint8_t *val;
	cy_rslt_t rslt;

	rslt = prox_sensor_write_reg((uint8)COMMAND, (uint8)NOP);
	rslt = prox_sensor_read_reg((uint8)RESPONSE, &val);
	// COMMAND Should be param set (101 -> PARAM_SET 00001 -> CHLIST)
	// 0xA1
	rslt = prox_sensor_write_reg((uint8)COMMAND, (uint8)PS_PAUSE);
	rslt = prox_sensor_read_reg((uint8)RESPONSE, &val);

	rslt = prox_sensor_write_reg((uint8) INT_CFG, 0x0);

	return rslt;

}

void print_reg_prox_val(uint8_t reg, uint8_t *val) 
{
	printf("value in reg %i is %i\n\r", reg, *val);
}

void task_read_motion(void)
{


	uint16_t PS1_THRESHOLD;
	prox_init();
	uint8_t *val;
	led_current();
	enable_ps1();
	

	// prox_sensor_read_reg((uint8_t)PARAM_RD, &val);
	set_rates();
	
	// always_active();
	PS1_THRESHOLD = set_threshold();
	set_autonomous();
	// assert INT before reading measurements
	ps1_int_enable();


	// TO IMPLEMENT:
	
	uint8_t rsp;

	while(1)
	{
		// prox_init();	
		// ps1_int_enable();
		
		uint16_t sense = read_measurement();

		// Temporary Solution while I try to get this to work
		// cyhal_system_delay_ms(250);
		// ps1_pause();
		

		
		if(sense > PS1_THRESHOLD)
		{
			RECORDING = !RECORDING;
			printf("RECORDING: %d \n\r", RECORDING);
		}
		
		vTaskDelay(pdMS_TO_TICKS(2000));
	}

}



/*
 * For every write to the command register, the following sequence is required:
 * 1. Write 0x00 to Command register to clear the Response register
 * 2. Read Response register and verify contents are 0x00
 * 3. Write Command value into Command register
 * 4. Read the Response register and verify contents are now non-zero.
 *    If contents are still 0x00, repeat this step.
 */