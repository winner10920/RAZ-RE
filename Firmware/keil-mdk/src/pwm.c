/**
 * @file pwm.c
 * @brief PWM implementation for N32G031K8 using TIM3 on PA6
 */

#include "pwm.h"
#include "n32g031_tim.h"
#include "n32g031_rcc.h"
#include "n32g031_gpio.h"

/* PWM Configuration */
#define PWM_TIMER              TIM3
#define PWM_TIMER_CLK          RCC_APB1_PERIPH_TIM3
#define PWM_GPIO_PORT          GPIOA
#define PWM_GPIO_PIN           GPIO_PIN_6
#define PWM_GPIO_AF            GPIO_AF2_TIM3
#define PWM_TIMER_CHANNEL      TIM_CH_1

#define PWM_DEFAULT_FREQUENCY  1000  /* Hz */
#define PWM_DEFAULT_DUTY_CYCLE 50    /* % */

/* Static variables to track state */
static uint32_t g_pwm_frequency = PWM_DEFAULT_FREQUENCY;
static uint8_t g_pwm_duty_cycle = PWM_DEFAULT_DUTY_CYCLE;
static uint16_t g_pwm_period = 0;
static uint16_t g_pwm_pulse = 0;

/**
 * @brief Calculate timer prescaler and period for desired frequency
 * @param frequency Desired frequency in Hz
 * @param[out] prescaler Calculated prescaler value
 * @param[out] period Calculated period value
 */
static void PWM_CalculateTimerValues(uint32_t frequency, uint16_t *prescaler, uint16_t *period)
{
    /* N32G031 APB1 clock is typically 32MHz (after RCC configuration)
     * Timer frequency = APB1_CLOCK / ((prescaler + 1) * (period + 1))
     * We want: frequency = 32000000 / ((prescaler + 1) * (period + 1))
     * Rearranging: (prescaler + 1) * (period + 1) = 32000000 / frequency
     * 
     * Strategy: Use prescaler to get into reasonable range, then set period
     */
    
    uint32_t timer_clock = 32000000;  /* Default APB1 clock */
    uint32_t ticks_needed = timer_clock / frequency;
    
    /* Start with prescaler = 31 (divides by 32) for 1MHz base */
    *prescaler = 31;
    *period = (ticks_needed / 32) - 1;
    
    /* Ensure period fits in 16-bit value */
    if (*period > 65535)
    {
        *prescaler = 319;  /* Divide by 320 for 100kHz base */
        *period = (ticks_needed / 320) - 1;
    }
    
    /* Cap period at maximum */
    if (*period > 65535)
        *period = 65535;
}

/**
 * @brief Initialize PWM on LCD backlight
 */
void PWM_Init(uint32_t frequency)
{
    GPIO_InitType GPIO_InitStructure;
    uint16_t prescaler, period;
    
    /* Store frequency for later use */
    g_pwm_frequency = frequency;
    
    /* Enable TIM3 and GPIOA clocks */
    RCC_EnableAPB1PeriphClk(PWM_TIMER_CLK, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    
    /* Configure GPIO PA6 as alternate function for TIM3_CH1 */
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = PWM_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = PWM_GPIO_AF;
    //GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    //GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitPeripheral(PWM_GPIO_PORT, &GPIO_InitStructure);
    
    /* Calculate prescaler and period based on desired frequency */
    PWM_CalculateTimerValues(frequency, &prescaler, &period);
    g_pwm_period = period;
    
    /* Configure TIM3 using direct register access for clarity */
    /* Disable timer first */
    PWM_TIMER->CTRL1 &= ~0x0001;  /* Disable bit 0 (CEN) */
    
    /* Set prescaler and period */
    PWM_TIMER->PSC = prescaler;
    PWM_TIMER->AR = period;
    
    /* Configure output compare mode on CCMOD1 register */
    /* Bits [7:4] = OC1M = 0111 (PWM2 mode) */
    /* Bits [1:0] = CC1SEL = 00 (output) */
    PWM_TIMER->CCMOD1 &= ~0x00F0;  /* Clear OC1M bits */
    PWM_TIMER->CCMOD1 |= 0x0070;   /* Set PWM2 mode (0111) */
    
    /* Set pulse value (comparison register) */
    PWM_TIMER->CCDAT1 = (period * PWM_DEFAULT_DUTY_CYCLE) / 100;
    
    /* Enable output compare channel 1 on CCEN register */
    /* Set CC1E bit (bit 0) */
    PWM_TIMER->CCEN |= 0x0001;
    
    /* Set output polarity: CC1P = 0 (active high) */
    PWM_TIMER->CCEN &= ~0x0002;
    
    /* Enable the timer counter */
    PWM_TIMER->CTRL1 |= 0x0001;  /* Set CEN bit */
    
    /* Save pulse value */
    g_pwm_pulse = PWM_TIMER->CCDAT1;
    g_pwm_duty_cycle = PWM_DEFAULT_DUTY_CYCLE;
}

/**
 * @brief Set PWM duty cycle (0-100%)
 */
void PWM_SetDutyCycle(uint8_t duty_cycle)
{
    uint16_t pulse;
    
    /* Clamp duty cycle to valid range */
    if (duty_cycle > 100)
        duty_cycle = 100;
    
    g_pwm_duty_cycle = duty_cycle;
    
    /* Calculate pulse width for PWM2 mode */
    pulse = (g_pwm_period * duty_cycle) / 100;
    g_pwm_pulse = pulse;
    
    /* Update the compare register - directly access CCDAT1 register for channel 1 */
    PWM_TIMER->CCDAT1 = pulse;
}

/**
 * @brief Change PWM frequency at runtime
 */
void PWM_SetFrequency(uint32_t frequency)
{
    uint16_t prescaler, period;
    
    g_pwm_frequency = frequency;
    
    /* Disable timer */
    TIM_Enable(PWM_TIMER, DISABLE);
    
    /* Calculate new prescaler and period */
    PWM_CalculateTimerValues(frequency, &prescaler, &period);
    g_pwm_period = period;
    
    /* Update prescaler and period */
    PWM_TIMER->PSC = prescaler;
    PWM_TIMER->AR = period;
    
    /* Recalculate pulse to maintain duty cycle percentage for PWM2 mode */
    uint16_t pulse = (period * g_pwm_duty_cycle) / 100;
    g_pwm_pulse = pulse;
    PWM_TIMER->CCDAT1 = pulse;
    
    /* Re-enable timer */
    TIM_Enable(PWM_TIMER, ENABLE);
}

/**
 * @brief Get current duty cycle
 */
uint8_t PWM_GetDutyCycle(void)
{
    return g_pwm_duty_cycle;
}

/**
 * @brief Get current frequency
 */
uint32_t PWM_GetFrequency(void)
{
    return g_pwm_frequency;
}

/**
 * @brief Enable or disable PWM
 */
void PWM_Enable(uint8_t enable)
{
    if (enable)
    {
        TIM_Enable(PWM_TIMER, ENABLE);
    }
    else
    {
        TIM_Enable(PWM_TIMER, DISABLE);
    }
}
