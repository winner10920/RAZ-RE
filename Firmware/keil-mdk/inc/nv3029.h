/* Minimal NV3029 driver header for 80x160 SPI display (renamed from ST7735)
 *
 */
#ifndef __NV3029_H__
#define __NV3029_H__

#include <stdint.h>
#include "n32g031.h"

#ifdef __cplusplus
extern "C" {
#endif

// Color macro RGB565
#define NV3029_COLOR565(r,g,b)  (uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3))

// Pin configuration (can be changed if needed)
#define NV3029_CS_GPIO_PORT GPIOA
#define NV3029_CS_PIN       GPIO_PIN_15
#define NV3029_DC_GPIO_PORT GPIOB
#define NV3029_DC_PIN       GPIO_PIN_7
#define NV3029_RST_GPIO_PORT GPIOB
#define NV3029_RST_PIN      GPIO_PIN_6

// Bit-banged SPI pins (do not change spi_flash hardware)
// SCLK = PB3, MOSI = PB5 (as you indicated)
#define NV3029_SCLK_GPIO_PORT GPIOB
#define NV3029_SCLK_PIN       GPIO_PIN_3
#define NV3029_MOSI_GPIO_PORT GPIOB
#define NV3029_MOSI_PIN       GPIO_PIN_5

// Hardware SPI to use for display (SPI1 on PB3/PB5)
#define NV3029_SPI              SPI1
#define NV3029_SPI_CLK          RCC_APB2_PERIPH_SPI1
#define NV3029_SPI_AF           GPIO_AF5_SPI1


// NV3029 commands (mapped from ST7735 compatible commands used by this panel)
#define NV3029_SWRESET 0x01
#define NV3029_SLPOUT  0x11
#define NV3029_DISPON  0x29
#define NV3029_CASET   0x2A
#define NV3029_RASET   0x2B
#define NV3029_RAMWR   0x2C
#define NV3029_MADCTL  0x36
#define NV3029_COLMOD  0x3A

// Display dimensions for NV3029 80x160
#define NV3029_WIDTH  80
#define NV3029_HEIGHT 160


void nv3029_init(void);
void nv3029_fill_screen(uint16_t color);
void nv3029_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bgcolor);
void nv3029_draw_string(uint16_t x, uint16_t y, const char* s, uint16_t color, uint16_t bgcolor);
void nv3029_diag(void);
extern void Delay(volatile uint32_t count);


// --- The Initialization Sequence (From your dump) ---
extern const uint8_t NV3029_Init_Sequence[];

// Standard 5x8 font (ASCII 32-127), 5 bytes per character
extern const uint8_t font5x8[][5];

#ifdef __cplusplus
}
#endif

#endif

