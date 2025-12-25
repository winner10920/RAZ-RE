#include "input.h"
#include "n32g031.h"



uint8_t input_read(void)
{
    /* Read input pin PA7, return 0 (low) or 1 (high) */
    return (uint8_t)(GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN) ? 1 : 0);
}

