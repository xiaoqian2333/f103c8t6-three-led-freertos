# 电控组第二次培训作业

- 姓名：**崔羡源**
- 学号：***20253002044***

设置500ms的定时中断，中断回调触发后在回调函数里通过信号量唤醒任务，任务按照状态位（状态位我这里采用数组控制）切换led，然后清空信号量，当信号量清空时任务协程等待，此为rtos的思路，另外也有在中断回调中修改触发led状态位，然后在main主函数中套一个大循环遍历led灯状态和其他代码，或者是设置三个相同优先级的任务，分别按照500,1000,1500ms阻塞置高低电平。但另外两个没有在工程代码中写出

```c
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *timer)
{
    if (timer->Instance == TIM2) {
        BaseType_t higherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(ledTickSemaphore, &higherPriorityTaskWoken);
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }
}
```

演示视频位于 `videos/` 目录。
