/******************************************************************************
* File Name: ble_task.h
*
* Description: This file is the public interface of ble_task.c source file.
*
* Related Document: See README.md
*
*******************************************************************************
* (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/


/*******************************************************************************
 * Include guard 
 ******************************************************************************/
#ifndef BLE_TASK_H_
#define BLE_TASK_H_


/*******************************************************************************
 * Header files
 ******************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


/*******************************************************************************
 * Structures and enumerations 
 ******************************************************************************/
/* List of BLE commands */
typedef enum
{
    BLE_PROCESS_EVENT,
    HANDLE_GPIO_INTERRUPT
} ble_command_t;

/* Data-type of BLE commands and data */
typedef struct
{
    ble_command_t command;
    uint32_t data;
}ble_command_data_t;


/*******************************************************************************
 * Extern variables
 ******************************************************************************/
/* Handle for the queue that contains BLE commands and data */
extern QueueHandle_t ble_command_data_q;


/*******************************************************************************
 * Function prototype
 ******************************************************************************/
void task_ble(void* param);

extern volatile bool RECORDING;


#endif /* BLE_TASK_H_ */


// /* [] END OF FILE */
