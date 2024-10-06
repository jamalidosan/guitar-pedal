#include "console_task.h"

TaskHandle_t CLI_Task;


void task_console_input(void *param)
{
    (void)param; // Silence IDE
    char* input; // used for parsing console input
    BaseType_t xReturned;

    printf("\x1b[2J\x1b[;H");
    printf("******************\n\r");
    printf("* ECE453 Dev Platform\n\r");
    printf("******************\n\r");
    printf("Type 'record' to start a recording \n\r");
    printf("Type 'stop' to end a recording \n\r");
    printf("Type 'help' for any assistance \n\r");

    /* Repeatedly running part of the task */
    // command line interface
    for (;;)
    {
        if (ALERT_CONSOLE_RX)
        {
            input = strtok(pcInputString, " \n\r");
            if (strcmp(input, "record") == 0)
            {
                if(RECORDING)
                {
                    printf("\x1b[2J\x1b[;H");
                    printf("******************\n\r");
                    printf("* Now Recording\n\r");
                    printf("******************\n\r");
                    printf("Recording in progress: Please type 'stop' \n\r");
                    // break;
                }
                else
                {
                    RECORDING = true;
                    printf("\x1b[2J\x1b[;H");
                    printf("******************\n\r");
                    printf("* Now Recording\n\r");
                    printf("******************\n\r");
                    printf("RECORDING: %d \n\r", RECORDING);
                    // break;
                }
            }
            else if (strcmp(input, "stop") == 0)
            {
                if(!RECORDING)
                {
                    printf("\x1b[2J\x1b[;H");
                    printf("Recording has not started: Please type 'record' \n\r");
                    // break;
                }
                else
                {
                    RECORDING = false;
                    printf("\x1b[2J\x1b[;H");
                    printf("******************\n\r");
                    printf("* Recording Finished\n\r");
                    printf("******************\n\r");
                    printf("RECORDING: %d \n\r", RECORDING);
                    // break;
                }
            }
            else if (strcmp(input, "help") == 0)
            {
                printf("\x1b[2J\x1b[;H");
                printf("Type 'record' to start a recording \n\r");
                printf("Type 'stop' to end a recording \n\r");
                // break;
            }
            console_clear_buffer();
        }
    }
}