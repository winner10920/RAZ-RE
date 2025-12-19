#include "pa7_input.h"
#include "n32g031.h"

void pa7_input_init(void)
{
    GPIO_InitType gpio;
    GPIO_InitStruct(&gpio);

    /* Enable GPIOA clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);

    gpio.Pin = GPIO_PIN_7;
    gpio.GPIO_Mode = GPIO_MODE_INPUT;
    /* default to pull-up */
    gpio.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitPeripheral(GPIOA, &gpio);
}

uint8_t pa7_input_read(void)
{
    /* Read input pin PA7, return 0 (low) or 1 (high) */
    return (uint8_t)(GPIO_ReadInputDataBit(GPIOA, GPIO_PIN_7) ? 1 : 0);
}

void pa7_input_set_pull(uint8_t pull_up)
{
    GPIO_InitType gpio;
    GPIO_InitStruct(&gpio);

    /* Enable GPIOA clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);

    gpio.Pin = GPIO_PIN_7;
    gpio.GPIO_Mode = GPIO_MODE_INPUT;
    gpio.GPIO_Pull = pull_up ? GPIO_PULL_UP : GPIO_PULL_DOWN;
    GPIO_InitPeripheral(GPIOA, &gpio);
}
