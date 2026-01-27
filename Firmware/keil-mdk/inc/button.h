/**
 * @file button.h
 * @brief Button timing and press detection using TIM1
 * 
 * Provides precise button press duration measurement and detection of:
 * - Short press (< 500ms)
 * - Long press (500ms - 3000ms)
 * - Very long press (> 10000ms)
 * - Continuous press since wakeup
 */

#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Button press states
 */
typedef enum {
    BUTTON_STATE_NONE = 0,           /* No action */
    BUTTON_STATE_RELEASED,           /* Button released */
    BUTTON_STATE_PRESSED,            /* Button currently pressed */
    BUTTON_STATE_SHORT_PRESS,        /* Short press detected (< 500ms) */
    BUTTON_STATE_LONG_PRESS,         /* Long press detected (500ms - 3000ms) */
    BUTTON_STATE_VERY_LONG_PRESS     /* Very long press detected (> 10000ms) */
} ButtonState_t;

/**
 * @brief Button timing thresholds (milliseconds)
 */
#define BUTTON_DEBOUNCE_MS          20      /* Debounce time */
#define BUTTON_SHORT_PRESS_MS       500     /* < 500ms = short */
#define BUTTON_LONG_PRESS_MS        3000    /* 500-3000ms = long */
#define BUTTON_VERY_LONG_PRESS_MS   10000   /* > 10000ms = very long */

/**
 * @brief Button handler structure
 */
typedef struct {
    uint32_t press_start_time;      /* Timestamp when button was pressed */
    uint32_t press_duration;        /* Duration of last press (ms) */
    ButtonState_t last_action;      /* Last detected button action */
    bool is_pressed;                /* Current button state */
    bool pressed_on_wakeup;         /* Was button held during wakeup */
    bool debounce_active;           /* Debouncing in progress */
    uint32_t debounce_time;         /* Debounce timer */
} ButtonHandler_t;

/**
 * @brief Initialize button timing system with TIM1
 * Sets up TIM1 as a 1ms tick counter for button timing
 */
void Button_Init(void);

/**
 * @brief Update button state (call in main loop)
 * Handles debouncing and press duration calculation
 */
void Button_Update(void);

/**
 * @brief Get the last detected button action
 * @return ButtonState_t representing the last button action
 */
ButtonState_t Button_GetLastAction(void);

/**
 * @brief Clear the last action flag
 * Call after handling a button action
 */
void Button_ClearAction(void);

/**
 * @brief Get current button press duration
 * @return Duration in milliseconds (0 if not pressed)
 */
uint32_t Button_GetPressDuration(void);

/**
 * @brief Check if button is currently pressed
 * @return true if button is currently pressed
 */
bool Button_IsPressed(void);

/**
 * @brief Check if button was pressed during wakeup
 * @return true if button was held during system wakeup
 */
bool Button_WasPressedOnWakeup(void);

/**
 * @brief Set the "pressed on wakeup" flag
 * Call this during wakeup sequence if button is detected as pressed
 */
void Button_SetPressedOnWakeup(void);

/**
 * @brief Clear the "pressed on wakeup" flag
 */
void Button_ClearPressedOnWakeup(void);

/**
 * @brief Get current timer tick count
 * @return Current millisecond count from TIM1
 */
uint32_t Button_GetTicks(void);

/**
 * @brief Button press interrupt handler
 * Call this from EXTI7 falling edge interrupt (button pressed)
 */
void Button_HandlePressInterrupt(void);

/**
 * @brief Button release interrupt handler  
 * Call this from EXTI7 rising edge interrupt (button released)
 */
void Button_HandleReleaseInterrupt(void);

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H__ */
