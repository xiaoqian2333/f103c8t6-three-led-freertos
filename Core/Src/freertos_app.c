#include "main.h"
#include "freertos_app.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32f1xx_hal_tim.h"

extern TIM_HandleTypeDef htim2;

static SemaphoreHandle_t ledTickSemaphore;

static void StartLedTask(void *argument)
{
    GPIO_PinState ledState[3] = {
        GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET
    };

    HAL_TIM_Base_Start_IT(&htim2);
    for (;;) {
        xSemaphoreTake(ledTickSemaphore, portMAX_DELAY);
        HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, ledState[0]);
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, ledState[1]);
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, ledState[2]);

        GPIO_PinState lastState = ledState[2];
        ledState[2] = ledState[1];
        ledState[1] = ledState[0];
        ledState[0] = lastState;
    }
}

void MX_FREERTOS_Init(void)
{
    ledTickSemaphore = xSemaphoreCreateBinary();
    xTaskCreate(StartLedTask, "LedTask", 128, NULL, 2, NULL);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *timer)
{
    if (timer->Instance == TIM2) {
        BaseType_t higherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(ledTickSemaphore, &higherPriorityTaskWoken);
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }
}
