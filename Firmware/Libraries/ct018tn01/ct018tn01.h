/* CT018TN01 panel driver
 * Implements power/init sequencing, voltage options and gamma/VCOM control
 * based on the CT018TN01 datasheet (pages 10-16).
 *
 * This driver uses the same pin/SPI macros as the existing ST7735 driver
 * (see `inc/st7735.h`) and sends commands/data directly over the configured
 * SPI peripheral. It does not control external power rails; voltage options
 * select appropriate register values to send to the panel controller.
 */
#ifndef __CT018TN01_H__
#define __CT018TN01_H__

#include <stdint.h>
#include <stddef.h>
#include "n32g031.h"

/* Pin and SPI mapping for CT018TN01 panel. These defaults match the wiring
 * you previously used for the ST7735 driver (CS=PA15, DC=PB7, RST=PB6,
 * SCK=PB3, MOSI=PB5, SPI1). Override by editing these macros if your
 * hardware is different.
 */
#ifndef CT018_CS_GPIO_PORT
#define CT018_CS_GPIO_PORT    GPIOA
#define CT018_CS_PIN          GPIO_PIN_15
#endif
#ifndef CT018_DC_GPIO_PORT
#define CT018_DC_GPIO_PORT    GPIOB
#define CT018_DC_PIN          GPIO_PIN_7
#endif
#ifndef CT018_RST_GPIO_PORT
#define CT018_RST_GPIO_PORT   GPIOB
#define CT018_RST_PIN         GPIO_PIN_6
#endif
#ifndef CT018_SCLK_GPIO_PORT
#define CT018_SCLK_GPIO_PORT  GPIOB
#define CT018_SCLK_PIN        GPIO_PIN_3
#endif
#ifndef CT018_MOSI_GPIO_PORT
#define CT018_MOSI_GPIO_PORT  GPIOB
#define CT018_MOSI_PIN        GPIO_PIN_5
#endif

#ifndef CT018_SPI
#define CT018_SPI             SPI1
#define CT018_SPI_CLK         RCC_APB2_PERIPH_SPI1
#define CT018_SPI_AF          GPIO_AF5_SPI1
#endif

#ifdef __cplusplus
extern "C" {
#endif


extern void Delay(volatile uint32_t count);


typedef enum {
    CT018_VOLTAGE_3V3 = 0, /* default */
    CT018_VOLTAGE_2V8,
    CT018_VOLTAGE_5V
} ct018_voltage_t;

/* Display geometry and color macro (RGB565) — use conservative 120x160
 * values to match the earlier ST7735 wiring used on this board. If your
 * CT018 panel uses different dimensions, update these macros accordingly.
 */
#define CT018_WIDTH  120
#define CT018_HEIGHT 160
#define CT018_COLOR565(r,g,b)  (uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3))

/** Initialize internal driver state; does not perform full power-on.
 *  @param vopt voltage selection (affects register values). */
void ct018_init(ct018_voltage_t vopt);

/** Power on sequence and panel initialization. Safe to call after `ct018_init()`.
 *  This performs reset, controller init commands, sets color mode and default
 *  gamma / VCOM values from the datasheet.
 */
void ct018_power_on(void);

/** Power off sequence (puts panel into sleep if supported). */
void ct018_power_off(void);


void ct018_gamma_set(void);
void ct018_chip_set(void);
void ct018_display_on(void);
void ct018_display_off(void);

/* Fill the entire panel with a single RGB565 color. Useful for quick tests. */
void ct018_fill_screen(uint16_t color);

/** Reconfigure voltage option at runtime (re-applies recommended registers).
 */

#ifdef __cplusplus
}
#endif

#endif
