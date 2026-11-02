#include "main.h"
#include "stm32f1xx_it.h"
#include "stm32f1xx_hal_tim.h"

extern TIM_HandleTypeDef htim2;

void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
}
