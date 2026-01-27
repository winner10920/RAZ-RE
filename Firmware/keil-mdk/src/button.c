/**
 * @file button.c
 * @brief Button timing implementation using TIM1 and EXTI interrupts
 */

#include "button.h"
#include "n32g031_tim.h"
#include "n32g031_rcc.h"
#include "n32g031_gpio.h"
#include "n32g031_exti.h"

/* Button GPIO configuration */
#define BUTTON_PORT     GPIOA
#define BUTTON_PIN      GPIO_PIN_7
#define BUTTON_EXTI_LINE EXTI_LINE7

/* TIM1 configuration for button timing */
#define BUTTON_TIMER           TIM1
#define BUTTON_TIMER_CLK       RCC_APB2_PERIPH_TIM1

/* Global button handler */
static ButtonHandler_t g_button_handler = {0};

/**
 * @brief Initialize TIM1 for button timing
 * Configures TIM1 to count at 1kHz (1ms per tick)
 */
void Button_Init(void)
{
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    
    /* Enable TIM1 clock */
    RCC_EnableAPB2PeriphClk(BUTTON_TIMER_CLK, ENABLE);
    
    /* Configure TIM1 for 1ms tick (1kHz) */
    /* APB2 clock for N32G031 is 48MHz */
    /* For basic timers: Timer_clock = APB_clock when prescaler = 1 */
    /* Prescaler = 47999 gives 48MHz / 48000 = 1kHz = 1ms tick */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Period = 0xFFFF;        /* 16-bit counter, max value */
    TIM_TimeBaseStructure.Prescaler = 47999;      /* 48MHz / 48000 = 1kHz */
    TIM_TimeBaseStructure.ClkDiv = 0;
    TIM_TimeBaseStructure.CntMode = TIM_CNT_MODE_UP;
    TIM_InitTimeBase(BUTTON_TIMER, &TIM_TimeBaseStructure);
    
    /* Enable timer */
    TIM_Enable(BUTTON_TIMER, ENABLE);
    
    /* Initialize button handler structure */
    g_button_handler.press_start_time = 0;
    g_button_handler.press_duration = 0;
    g_button_handler.last_action = BUTTON_STATE_NONE;
    g_button_handler.is_pressed = false;
    g_button_handler.pressed_on_wakeup = false;
    g_button_handler.debounce_active = false;
    g_button_handler.debounce_time = 0;
    
    /* Note: EXTI7 interrupt for button is configured in sleep_wake.c */
    /* We just need to handle the timing in the existing EXTI handler */
}

/**
 * @brief Get current timer tick count
 */
uint32_t Button_GetTicks(void)
{
    return TIM_GetCnt(BUTTON_TIMER);
}

/**
 * @brief Calculate elapsed time accounting for timer overflow
 */
static uint32_t Button_GetElapsedTime(uint32_t start_time)
{
    uint32_t current_time = Button_GetTicks();
    
    /* Handle overflow (16-bit timer) */
    if (current_time >= start_time)
    {
        return current_time - start_time;
    }
    else
    {
        /* Timer wrapped around */
        return (0xFFFF - start_time) + current_time + 1;
    }
}

/**
 * @brief Update button state machine
 */
void Button_Update(void)
{
    /* Timing is now handled by interrupts, this function just checks
     * if button is currently pressed for real-time duration updates */
    
    /* Nothing to do here - all timing handled in interrupt handlers */
}

/**
 * @brief Get last button action
 */
ButtonState_t Button_GetLastAction(void)
{
    return g_button_handler.last_action;
}

/**
 * @brief Clear last action
 */
void Button_ClearAction(void)
{
    g_button_handler.last_action = BUTTON_STATE_NONE;
}

/**
 * @brief Get current press duration
 */
uint32_t Button_GetPressDuration(void)
{
    if (g_button_handler.is_pressed)
    {
        return Button_GetElapsedTime(g_button_handler.press_start_time);
    }
    else
    {
        return g_button_handler.press_duration;
    }
}

/**
 * @brief Check if button is pressed
 */
bool Button_IsPressed(void)
{
    return g_button_handler.is_pressed;
}

/**
 * @brief Check if button was pressed on wakeup
 */
bool Button_WasPressedOnWakeup(void)
{
    return g_button_handler.pressed_on_wakeup;
}

/**
 * @brief Set pressed on wakeup flag
 */
void Button_SetPressedOnWakeup(void)
{
    g_button_handler.pressed_on_wakeup = true;
}

/**
 * @brief Clear pressed on wakeup flag
 */
void Button_ClearPressedOnWakeup(void)
{
    g_button_handler.pressed_on_wakeup = false;
}

/**
 * @brief Button press interrupt handler (called from EXTI7 falling edge)
 * Captures the timer value when button is pressed
 */
void Button_HandlePressInterrupt(void)
{
    /* Capture timestamp on button press */
    g_button_handler.press_start_time = Button_GetTicks();
    g_button_handler.is_pressed = true;
    g_button_handler.last_action = BUTTON_STATE_PRESSED;
    g_button_handler.debounce_active = true;
    g_button_handler.debounce_time = g_button_handler.press_start_time;
}

/**
 * @brief Button release interrupt handler (called from EXTI7 rising edge)
 * Calculates press duration and classifies press type
 */
void Button_HandleReleaseInterrupt(void)
{
    /* Check if we were actually pressed (debounce protection) */
    if (!g_button_handler.is_pressed)
    {
        return;
    }
    
    /* Capture timestamp and calculate duration */
    uint32_t release_time = Button_GetTicks();
    g_button_handler.press_duration = Button_GetElapsedTime(g_button_handler.press_start_time);
    g_button_handler.is_pressed = false;
    
    /* Simple debounce check - ignore very short presses */
    if (g_button_handler.press_duration < BUTTON_DEBOUNCE_MS)
    {
        g_button_handler.last_action = BUTTON_STATE_NONE;
        return;
    }
    
    /* Classify press type based on duration */
    if (g_button_handler.press_duration < BUTTON_SHORT_PRESS_MS)
    {
        g_button_handler.last_action = BUTTON_STATE_SHORT_PRESS;
    }
    else if (g_button_handler.press_duration < BUTTON_LONG_PRESS_MS)
    {
        g_button_handler.last_action = BUTTON_STATE_LONG_PRESS;
    }
    else if (g_button_handler.press_duration >= BUTTON_VERY_LONG_PRESS_MS)
    {
        g_button_handler.last_action = BUTTON_STATE_VERY_LONG_PRESS;
    }
    else
    {
        /* Between long and very long (3-10 seconds) - treat as long */
        g_button_handler.last_action = BUTTON_STATE_LONG_PRESS;
    }
}
