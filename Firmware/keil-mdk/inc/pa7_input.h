/* PA7 input helper
 * Provides simple functions to configure PA7 as an input and read its state.
 */
#ifndef __PA7_INPUT_H__
#define __PA7_INPUT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Configure PA7 as a GPIO input (floating input).
 * Enables the GPIOA clock if necessary.
 */
void pa7_input_init(void);

/**
 * Read the current logic level on PA7.
 * Returns 0 if low, 1 if high.
 */
uint8_t pa7_input_read(void);

/**
 * Configure internal pull on PA7.
 * @param pull_up  non-zero to enable pull-up, zero to enable pull-down.
 * Default behavior is pull-up when calling `pa7_input_init()`.
 */
void pa7_input_set_pull(uint8_t pull_up);

#ifdef __cplusplus
}
#endif

#endif
