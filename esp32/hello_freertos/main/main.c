#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define QUEUE_SIZE 3

QueueHandle_t xQueue;

typedef enum
{
    eSender0,
    eSender1
} DataSource_t;


typedef struct
{
    DataSource_t eSource;
    uint8_t ucValue;
} Data_t;


static const Data_t xStructsToSend[] =
{
    {eSender0, 100},
    {eSender1, 200}
};


static void vSenderTask(void *pvParameters)
{
    BaseType_t xStatus;
    static const TickType_t xTicksToWait = pdMS_TO_TICKS(100);

    for (;;)
    {
        xStatus = xQueueSendToBack(xQueue, pvParameters, xTicksToWait);
        if (xStatus != pdPASS)
        {
            printf("Error: Could not send to the queue. Queue is full.\r\n");
        }
        // After receiver continously reads all and the queue is empty: the reciever blocks, the sender blocks, so idle
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}


static void vReceiverTask(void *pvParameters)
{
    Data_t xReceivedStructure;
    BaseType_t xStatus;
    static const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
    for (;;)
    {
        xStatus = xQueueReceive(xQueue, &xReceivedStructure, xTicksToWait);
        if (xStatus == pdPASS)
        {
            if (xReceivedStructure.eSource == eSender0)
            {
                printf("From Sender0: %hhu\r\n", xReceivedStructure.ucValue);
            }
            else if (xReceivedStructure.eSource == eSender1)
            {
                printf("From Sender1: %hhu\r\n", xReceivedStructure.ucValue);
            }
        }
        else
        {
            printf("ERROR: Could not receive from the queue. Queue is empty.\r\n");
        }
    }
}


void app_main(void)
{
    xQueue = xQueueCreate(QUEUE_SIZE, sizeof(Data_t));

    if (xQueue != NULL)
    {
        void *const sender0Data = &xStructsToSend[0];
        void *const sender1Data = &xStructsToSend[1];

        xTaskCreate(vSenderTask, "Sender0", 2048, sender0Data, 2, NULL);
        xTaskCreate(vSenderTask, "Sender0", 2048, sender1Data, 2, NULL);
        xTaskCreate(vReceiverTask, "Receiver0", 2048, NULL, 1, NULL);
    }
    else
    {
        printf("ERROR: Queue cannot be created.\r\n");
    }
}
