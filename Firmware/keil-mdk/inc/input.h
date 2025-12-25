/* PA7 input helper
 * Provides simple functions to configure PA7 as an input and read its state.
 */
#ifndef _INPUT_H__
#define _INPUT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef BUTTON_GPIO
#define BUTTON_GPIO GPIOA
#endif

#ifndef BUTTON_PIN
#define BUTTON_PIN GPIO_PIN_7
#endif

/**
 * Read the current logic level on PA7.
 * Returns 0 if low, 1 if high.
 */
uint8_t input_read(void);


#ifdef __cplusplus
}
#endif

#endif
