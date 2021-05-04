/*
// Example x...

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "freertos/timers.h"


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
*/













/*
//// Example 13
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#define mainONE_SHOT_TIMER_PERIOD (pdMS_TO_TICKS(3300))
#define mainAUTO_RELOAD_TIMER_PERIOD (pdMS_TO_TICKS(500))

volatile uint32_t ulCallCount = 0; 

typedef struct
{
    TimerHandle_t xHandle;
    BaseType_t xStarted;
} Timer_t;


static void prvOneShotTimerCallback(TimerHandle_t xTimer)
{
    TickType_t xTimeNow;
    
    xTimeNow = xTaskGetTickCount();
    printf("One-shot timer callback executing at tick: %u.\r\n", xTimeNow);
    ulCallCount++;
}


static void prvAutoReloadTimerCallback(TimerHandle_t xTimer)
{
    TickType_t xTimeNow;
    xTimeNow = xTaskGetTickCount();
    printf("Auto-reload timer callback executing at tick: %u.\r\n", xTimeNow);
    ulCallCount++;
}


void app_main(void)
{
    Timer_t xOneShotTimer = 
    {
        .xHandle = xTimerCreate(
            "OneShot",
            mainONE_SHOT_TIMER_PERIOD,
            pdFALSE,
            0,
            prvOneShotTimerCallback
        ) 
    };

    Timer_t xAutoReloadTimer =
    {
        .xHandle = xTimerCreate(
            "AutoReload",
            mainAUTO_RELOAD_TIMER_PERIOD,
            pdTRUE,
            0,
            prvAutoReloadTimerCallback
        )
    };

    if ((xOneShotTimer.xHandle != NULL) && (xAutoReloadTimer.xHandle != NULL))
    {
        xOneShotTimer.xStarted = xTimerStart(xOneShotTimer.xHandle, 0);
        xAutoReloadTimer.xStarted = xTimerStart(xAutoReloadTimer.xHandle, 0);
    
        if ((xOneShotTimer.xStarted == pdPASS) && (xAutoReloadTimer.xStarted == pdPASS))
        {
            printf("Successful start.\r\n");
        }
    }
}
*/

//// Example 14

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#define mainONE_SHOT_TIMER_PERIOD (pdMS_TO_TICKS(3333))
#define mainAUTO_RELOAD_TIMER_PERIOD (pdMS_TO_TICKS(500))

typedef struct
{
    TimerHandle_t xHandle;
    BaseType_t xStarted;
} Software_Timer_t;


Software_Timer_t xOneShotTimer;
Software_Timer_t xAutoReloadTimer;

static void prvTimerCallback(TimerHandle_t xTimer)
{
    TickType_t xTimeNow;
    uint32_t ulExecutionCount;
    
    xTimeNow = xTaskGetTickCount();

    ulExecutionCount = (uint32_t)pvTimerGetTimerID(xTimer);
    ulExecutionCount++;
    vTimerSetTimerID(xTimer, (void *)ulExecutionCount);

    if (xTimer == xOneShotTimer.xHandle)
    {
        printf("One-shot timer callback executed %u times at tick %u\r\n", ulExecutionCount, xTimeNow);
    }
    else if (xTimer == xAutoReloadTimer.xHandle)
    {
        printf("Auto-reload timer callback executed %u times at tick %u\r\n", ulExecutionCount, xTimeNow);

        if (ulExecutionCount >= 5)
        {
            xTimerStop(xTimer, 0);
            // if (xTimerStop variable pass/not pass...)
        }
    }
}


void app_main(void)
{
    printf("Initiate app_main.\r\n");

    xOneShotTimer.xHandle = xTimerCreate("One Shot", mainONE_SHOT_TIMER_PERIOD, pdFALSE, 0, prvTimerCallback);
    xAutoReloadTimer.xHandle = xTimerCreate("Auto Reload", mainAUTO_RELOAD_TIMER_PERIOD, pdTRUE, 0, prvTimerCallback);

    if ((xOneShotTimer.xHandle != NULL) && (xAutoReloadTimer.xHandle != NULL))
    {
        xOneShotTimer.xStarted = xTimerStart(xOneShotTimer.xHandle, 0);
        xAutoReloadTimer.xStarted = xTimerStart(xAutoReloadTimer.xHandle, 0);

        if ((xOneShotTimer.xStarted == pdPASS) && (xAutoReloadTimer.xStarted == pdPASS))
        {
            printf("Successful start.\r\n");
        }
    }
}

