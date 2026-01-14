/**
 * @file pwm.h
 * @brief PWM (Pulse Width Modulation) library for N32G031K8
 * 
 * Provides easy-to-use PWM control for GPIO pins using hardware timers.
 * Default configuration: 1000 Hz frequency, 50% duty cycle on PA6 (LCD_BACKLIGHT)
 */

#ifndef __PWM_H__
#define __PWM_H__

#include <stdint.h>
#include "n32g031.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize PWM on LCD backlight pin (PA6 - TIM3_CH1)
 * @param frequency Desired PWM frequency in Hz (default 1000 Hz)
 * @note Must be called before any other PWM functions
 */
void PWM_Init(uint32_t frequency);

/**
 * @brief Set PWM duty cycle
 * @param duty_cycle Duty cycle as percentage (0-100)
 *        0 = always off, 100 = always on
 */
void PWM_SetDutyCycle(uint8_t duty_cycle);

/**
 * @brief Change PWM frequency at runtime
 * @param frequency New frequency in Hz
 */
void PWM_SetFrequency(uint32_t frequency);

/**
 * @brief Get current duty cycle
 * @return Current duty cycle (0-100)
 */
uint8_t PWM_GetDutyCycle(void);

/**
 * @brief Get current frequency
 * @return Current frequency in Hz
 */
uint32_t PWM_GetFrequency(void);

/**
 * @brief Enable/disable PWM output
 * @param enable 1 to enable, 0 to disable
 */
void PWM_Enable(uint8_t enable);

#ifdef __cplusplus
}
#endif

#endif /* __PWM_H__ */
