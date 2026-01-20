/**
 * @file sleep_wake.c
 * @brief Sleep/Wake implementation for N32G031K8
 */

#include "sleep_wake.h"
#include "pwm.h"
#include "n32g031_rcc.h"
#include "n32g031_gpio.h"
#include "n32g031_exti.h"

/* Sleep/Wake state machine */
static bool g_is_sleeping = false;
static uint32_t g_timeout_sec = 0;
static uint32_t g_inactivity_counter = 0;
static uint32_t g_timeout_counter = 0;
static volatile bool g_wake_interrupt_triggered = false;

/* Saved peripheral state for restoration */
static uint8_t g_saved_backlight_brightness = 0;
static bool g_saved_tv1_state = false;
static bool g_saved_tv2_state = false;

/* GPIO Pin definitions */
#define BUTTON_PORT GPIOA
#define BUTTON_PIN GPIO_PIN_7
#define BUTTON_EXTI_LINE EXTI_LINE7

#define MIC_PORT GPIOA
#define MIC_PIN GPIO_PIN_3
#define MIC_EXTI_LINE EXTI_LINE3

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
    g_wake_interrupt_triggered = false;
    g_saved_backlight_brightness = 50;  /* Default brightness */
    
    /* Initialize external interrupts for wake */
    SleepWake_InitExternalInterrupts();
}

/**
 * @brief Initialize external interrupts for BUTTON and MIC pins
 * Configured for falling edge (button press, activity detection)
 */
void SleepWake_InitExternalInterrupts(void)
{
    GPIO_InitType gpio_init;
    EXTI_InitType exti_init;


    /* Configure EXTI for BUTTON_PIN (PA7 -> EXTI7) */
    EXTI_InitStruct(&exti_init);
    exti_init.EXTI_Line = BUTTON_EXTI_LINE;
    exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Falling;  /* Button press (low) */
    exti_init.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&exti_init);

    /* Configure EXTI for MIC_PIN (PA3 -> EXTI3) */
    exti_init.EXTI_Line = MIC_EXTI_LINE;
    exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Rising;  /* Activity detection */
    exti_init.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&exti_init);

    /* Enable EXTI interrupt handlers in NVIC */
    NVIC_EnableIRQ(EXTI2_3_IRQn);  /* EXTI3 is handled by EXTI2_3_IRQn */
    NVIC_EnableIRQ(EXTI4_15_IRQn); /* EXTI7 is handled by EXTI4_15_IRQn */
    NVIC_SetPriority(EXTI2_3_IRQn, 2);
    NVIC_SetPriority(EXTI4_15_IRQn, 2);
}

/**
 * @brief Disable external interrupts
 */
void SleepWake_DisableExternalInterrupts(void)
{
    EXTI_InitType exti_init;
    exti_init.EXTI_LineCmd = DISABLE;
    exti_init.EXTI_Line = BUTTON_EXTI_LINE;
    exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitPeripheral(&exti_init);
    
    exti_init.EXTI_Line = MIC_EXTI_LINE;
    EXTI_InitPeripheral(&exti_init);

    NVIC_DisableIRQ(EXTI2_3_IRQn);
    NVIC_DisableIRQ(EXTI4_15_IRQn);
}

/**
 * @brief Enable external interrupts
 */
void SleepWake_EnableExternalInterrupts(void)
{
    EXTI_InitType exti_init;
    exti_init.EXTI_LineCmd = ENABLE;
    exti_init.EXTI_Line = BUTTON_EXTI_LINE;
    exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitPeripheral(&exti_init);
    
    exti_init.EXTI_Line = MIC_EXTI_LINE;
    EXTI_InitPeripheral(&exti_init);

    NVIC_EnableIRQ(EXTI2_3_IRQn);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

/**
 * @brief Check if wake interrupt was triggered
 */
bool SleepWake_IsWakeInterruptTriggered(void)
{
    return g_wake_interrupt_triggered;
}

/**
 * @brief Clear wake interrupt flag
 */
void SleepWake_ClearWakeInterrupt(void)
{
    g_wake_interrupt_triggered = false;
}
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
    //GPIO_ResetBits(GPIOB, GPIO_PIN_0);   /* LP4086_ISET off */

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
    //GPIO_SetBits(GPIOB, GPIO_PIN_0);

    g_is_sleeping = false;

    /* Reset inactivity timer */
    g_inactivity_counter = 0;
}

/**
 * @brief EXTI2_3 Interrupt handler (MIC_PIN on EXTI3)
 */
void EXTI2_3_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_LINE3) != RESET)
    {
        g_wake_interrupt_triggered = true;
        EXTI_ClrITPendBit(EXTI_LINE3);
    }
}

/**
 * @brief EXTI4_15 Interrupt handler (BUTTON_PIN on EXTI7)
 */
void EXTI4_15_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_LINE7) != RESET)
    {
        g_wake_interrupt_triggered = true;
        EXTI_ClrITPendBit(EXTI_LINE7);
    }
}
