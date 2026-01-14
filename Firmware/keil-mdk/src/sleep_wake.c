/**
 * @file sleep_wake.c
 * @brief Sleep/Wake implementation for N32G031K8
 */

#include "sleep_wake.h"
#include "pwm.h"
#include "n32g031_rcc.h"
#include "n32g031_gpio.h"
#include "n32g031_pwr.h"

/* Sleep/Wake state machine */
static bool g_is_sleeping = false;
static uint32_t g_timeout_sec = 0;
static uint32_t g_inactivity_counter = 0;
static uint32_t g_timeout_counter = 0;

/* Saved peripheral state for restoration */
static uint8_t g_saved_backlight_brightness = 0;
static bool g_saved_tv1_state = false;
static bool g_saved_tv2_state = false;

/* Timer tick counter (assuming 100ms per call to SleepWake_CheckTimeout) */
#define TICKS_PER_SECOND 10

/**
 * @brief Initialize sleep/wake system
 */
void SleepWake_Init(uint32_t timeout_sec)
{
    g_timeout_sec = timeout_sec;
    g_inactivity_counter = 0;
    g_timeout_counter = 0;
    g_is_sleeping = false;
    g_saved_backlight_brightness = 50;  /* Default brightness */
}

/**
 * @brief Enable timeout
 */
void SleepWake_EnableTimeout(uint32_t timeout_sec)
{
    g_timeout_sec = timeout_sec;
    g_inactivity_counter = 0;
    g_timeout_counter = 0;
}

/**
 * @brief Disable timeout
 */
void SleepWake_DisableTimeout(void)
{
    g_timeout_sec = 0;
    g_inactivity_counter = 0;
    g_timeout_counter = 0;
}

/**
 * @brief Reset activity timer
 */
void SleepWake_ResetTimer(void)
{
    if (!g_is_sleeping)
    {
        g_inactivity_counter = 0;
        g_timeout_counter = 0;
    }
}

/**
 * @brief Check if timeout has expired
 * Call this periodically (e.g., every 100ms)
 */
bool SleepWake_CheckTimeout(void)
{
    if (g_timeout_sec == 0 || g_is_sleeping)
    {
        return false;  /* Timeout disabled or already sleeping */
    }

    g_inactivity_counter++;

    /* Check if timeout reached (timeout_sec * 10 ticks per second) */
    if (g_inactivity_counter >= (g_timeout_sec * TICKS_PER_SECOND))
    {
        return true;  /* Timeout expired */
    }

    return false;
}

/**
 * @brief Get remaining time before sleep
 */
uint32_t SleepWake_GetTimeRemaining(void)
{
    if (g_timeout_sec == 0 || g_is_sleeping)
    {
        return 0;
    }

    uint32_t ticks_remaining = (g_timeout_sec * TICKS_PER_SECOND) - g_inactivity_counter;
    return (ticks_remaining + TICKS_PER_SECOND - 1) / TICKS_PER_SECOND;  /* Round up */
}

/**
 * @brief Check if sleeping
 */
bool SleepWake_IsSleeping(void)
{
    return g_is_sleeping;
}

/**
 * @brief Put device to sleep
 * Turns off backlight and non-essential peripherals
 */
void SleepWake_GoToSleep(void)
{
    if (g_is_sleeping)
    {
        return;  /* Already sleeping */
    }

    /* Save current states */
    g_saved_backlight_brightness = PWM_GetDutyCycle();
    g_saved_tv1_state = GPIO_ReadOutputDataBit(GPIOA, GPIO_PIN_5);
    g_saved_tv2_state = GPIO_ReadOutputDataBit(GPIOB, GPIO_PIN_8);

    /* Turn off backlight */
    PWM_SetDutyCycle(0);

    /* Turn off non-essential output loads using GPIO library directly */
    GPIO_ResetBits(GPIOA, GPIO_PIN_5);   /* TV1 off */
    GPIO_ResetBits(GPIOB, GPIO_PIN_8);   /* TV2 off */
    GPIO_ResetBits(GPIOB, GPIO_PIN_0);   /* LP4086_ISET off */

    /* Optional: Disable non-essential clocks here if needed */
    /* RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, DISABLE); etc */

    g_is_sleeping = true;
}

/**
 * @brief Wake up from sleep
 * Restores backlight and peripherals
 */
void SleepWake_WakeUp(void)
{
    if (!g_is_sleeping)
    {
        return;  /* Not sleeping */
    }

    /* Restore backlight */
    PWM_SetDutyCycle(g_saved_backlight_brightness);

    /* Restore output loads to previous state using GPIO library directly */
    if (g_saved_tv1_state)
        GPIO_SetBits(GPIOA, GPIO_PIN_5);
    else
        GPIO_ResetBits(GPIOA, GPIO_PIN_5);

    if (g_saved_tv2_state)
        GPIO_SetBits(GPIOB, GPIO_PIN_8);
    else
        GPIO_ResetBits(GPIOB, GPIO_PIN_8);

    /* Restore charging control if it was enabled */
    GPIO_SetBits(GPIOB, GPIO_PIN_0);

    g_is_sleeping = false;

    /* Reset inactivity timer */
    g_inactivity_counter = 0;
}
