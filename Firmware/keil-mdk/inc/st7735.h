/* Minimal ST7735 driver header for 80x160 SPI display
 * Uses SPI2 peripheral. CS on PB7, DC on PB6, RST on PB5 by default.
 */
#ifndef __ST7735_H__
#define __ST7735_H__

#include <stdint.h>
#include "spi_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

// Color macro RGB565
#define ST7735_COLOR565(r,g,b)  (uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3))

// Pin configuration (can be changed if needed)
#define ST7735_CS_GPIO_PORT GPIOB
#define ST7735_CS_PIN       GPIO_PIN_7
#define ST7735_DC_GPIO_PORT GPIOB
#define ST7735_DC_PIN       GPIO_PIN_6
#define ST7735_RST_GPIO_PORT GPIOB
#define ST7735_RST_PIN      GPIO_PIN_5

void st7735_init(void);
void st7735_fill_screen(uint16_t color);
void st7735_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bgcolor);
void st7735_draw_string(uint16_t x, uint16_t y, const char* s, uint16_t color, uint16_t bgcolor);
extern void Delay(volatile uint32_t count);

#ifdef __cplusplus
}
#endif

#endif
