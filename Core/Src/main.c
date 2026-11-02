#include "main.h"
#include "freertos_app.h"
#include "FreeRTOS.h"
#include "task.h"

TIM_HandleTypeDef htim2;

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM2_Init();
    MX_FREERTOS_Init();
    vTaskStartScheduler();

    while (1) {
    }
}

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef oscillator = {0};
    RCC_ClkInitTypeDef clock = {0};

    oscillator.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    oscillator.HSEState = RCC_HSE_ON;
    oscillator.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    oscillator.HSIState = RCC_HSI_ON;
    oscillator.PLL.PLLState = RCC_PLL_ON;
    oscillator.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    oscillator.PLL.PLLMUL = RCC_PLL_MUL9;
    HAL_RCC_OscConfig(&oscillator);

    clock.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                      RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clock.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clock.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clock.APB1CLKDivider = RCC_HCLK_DIV2;
    clock.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&clock, FLASH_LATENCY_2);
}

static void MX_TIM2_Init(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 7200 - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 5000 - 1;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htim2);
    HAL_NVIC_SetPriority(TIM2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef gpio = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOA, LED0_Pin | LED1_Pin | LED2_Pin, GPIO_PIN_RESET);
    gpio.Pin = LED0_Pin | LED1_Pin | LED2_Pin;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio);
}

void HAL_MspInit(void)
{
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}

