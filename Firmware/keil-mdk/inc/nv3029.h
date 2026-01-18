/* Minimal NV3029 driver header for 80x160 SPI display (renamed from ST7735)
 *
 */
#ifndef __NV3029_H__
#define __NV3029_H__

#include <stdint.h>
#include <stdbool.h>
#include "n32g031.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * LCD Command Definitions (ST7735R / NV3029 Compatible)
 * Source: ST7735R V0.2 Datasheet - Page 79 onwards
 * ============================================================================ */

/* System Function Commands */
#define LCD_CMD_NOP         0x00    /* No Operation */
#define LCD_CMD_SWRESET     0x01    /* Software Reset */
#define LCD_CMD_RDDID       0x04    /* Read Display ID */
#define LCD_CMD_SWRESET2    0x08    /* S/W Reset */
#define LCD_CMD_RDDST       0x09    /* Read Display Status */
#define LCD_CMD_RDDPM       0x0A    /* Read Display Power Mode */
#define LCD_CMD_RDDMADCTL   0x0B    /* Read Display MADCTL */
#define LCD_CMD_RDDCOLMOD   0x0C    /* Read Display Pixel Format (Image Mode) */
#define LCD_CMD_RDDIM       0x0D    /* Read Display Image Mode */
#define LCD_CMD_RDDSM       0x0E    /* Read Display Signal Mode */

/* Sleep/Display Control Commands */
#define LCD_CMD_SLPIN       0x10    /* Sleep In (Sleep in & booster off) */
#define LCD_CMD_SLPOUT      0x11    /* Sleep Out (Sleep out & booster on) */
#define LCD_CMD_PTLON       0x12    /* Partial Mode On */
#define LCD_CMD_PTLOFF      0x13    /* Partial Off (Normal Display Mode) */
#define LCD_CMD_INVOFF      0x20    /* Display Inversion Off */
#define LCD_CMD_INVON       0x21    /* Display Inversion On */
#define LCD_CMD_GAMSET      0x26    /* Gamma Curve Select */
#define LCD_CMD_DISPOFF     0x28    /* Display Off */
#define LCD_CMD_DISPON      0x29    /* Display On */

/* Memory Access Commands */
#define LCD_CMD_CASET       0x2A    /* Column Address Set */
#define LCD_CMD_RASET       0x2B    /* Row Address Set (Page Address Set) */
#define LCD_CMD_RAMWR       0x2C    /* Memory Write */
#define LCD_CMD_LUTSEL      0x2D    /* LUT Select (for 4k, 65k, 262k color) */
#define LCD_CMD_RAMRD       0x2E    /* Memory Read */
#define LCD_CMD_PTLAR       0x30    /* Partial Area (Partial start/end address set) */
#define LCD_CMD_VSCRDEF     0x33    /* Vertical Scrolling Definition */
#define LCD_CMD_TEOFF       0x34    /* Tearing Effect Line OFF */
#define LCD_CMD_TEON        0x35    /* Tearing Effect Line ON (Mode set & on) */
#define LCD_CMD_MADCTL      0x36    /* Memory Data Access Control (MADCTL) */
#define LCD_CMD_VSCRSADD    0x37    /* Vertical Scrolling Start Address */
#define LCD_CMD_IDMOFF      0x38    /* Idle Mode Off */
#define LCD_CMD_IDMON       0x39    /* Idle Mode On */
#define LCD_CMD_COLMOD      0x3A    /* Interface Pixel Format (COLMOD) */

/* Frame Rate Control Commands */
#define LCD_CMD_FRMCTR1     0xB1    /* Frame Rate Control (Normal mode) */
#define LCD_CMD_FRMCTR2     0xB2    /* Frame Rate Control (Idle mode) */
#define LCD_CMD_FRMCTR3     0xB3    /* Frame Rate Control (Partial mode) */
#define LCD_CMD_INVCTR      0xB4    /* Display Inversion Control */

/* Power Control Commands */
#define LCD_CMD_PWCTR1      0xC0    /* Power Control 1 */
#define LCD_CMD_PWCTR2      0xC1    /* Power Control 2 */
#define LCD_CMD_PWCTR3      0xC2    /* Power Control 3 (VGH/VGL adjustments) */
#define LCD_CMD_PWCTR4      0xC3    /* Power Control 4 (Idle mode) */
#define LCD_CMD_PWCTR5      0xC4    /* Power Control 5 (Partial + Full color mode) */
#define LCD_CMD_VMCTR1      0xC5    /* VCOM Control 1 */
#define LCD_CMD_VMCTR2      0xC7    /* VCOM Offset (Set VCOM offset control) */

/* LCM/NVM Commands */
#define LCD_CMD_LCMVER      0xD1    /* LCM Version (Set LCM version code) */
#define LCD_CMD_FUNCSET     0xD2    /* Function Setting */
#define LCD_CMD_NVMCTRL     0xD9    /* NVM Control */
#define LCD_CMD_RDID1       0xDA    /* Read ID1 */
#define LCD_CMD_RDID2       0xDB    /* Read ID2 */
#define LCD_CMD_RDID3       0xDC    /* Read ID3 */
#define LCD_CMD_NVMREAD     0xDE    /* NVM Read Command */
#define LCD_CMD_NVMWRITE    0xDF    /* NVM Write Command */

/* Gamma Correction Commands */
#define LCD_CMD_GMCTRP1     0xE0    /* Positive Gamma Correction */
#define LCD_CMD_GMCTRN1     0xE1    /* Negative Gamma Correction */

/* Vendor Specific Commands (ILI9341 Compatible) */
#define LCD_CMD_PWCTRA      0xCB    /* Power Control A (ILI9341) */
#define LCD_CMD_PWCTRB      0xCF    /* Power Control B (ILI9341) */

/* ============================================================================
 * MADCTL (Memory Data Access Control) Register Bits
 * ============================================================================ */
#define LCD_MADCTL_MY       0x80    /* Row Address Order */
#define LCD_MADCTL_MX       0x40    /* Column Address Order */
#define LCD_MADCTL_MV       0x20    /* Row/Column Exchange */
#define LCD_MADCTL_ML       0x10    /* Vertical Refresh Order */
#define LCD_MADCTL_RGB      0x00    /* RGB Order */
#define LCD_MADCTL_BGR      0x08    /* BGR Order */
#define LCD_MADCTL_MH       0x04    /* Horizontal Refresh Order */

/* ============================================================================
 * COLMOD (Pixel Format) Values
 * ============================================================================ */
#define LCD_COLMOD_12BIT    0x03    /* 12-bit/pixel */
#define LCD_COLMOD_16BIT    0x05    /* 16-bit/pixel (RGB565) */
#define LCD_COLMOD_18BIT    0x06    /* 18-bit/pixel */

/* ============================================================================
 * Common Color Definitions (RGB565)
 * ============================================================================ */
#define COLOR_BLACK         0x0000
#define COLOR_NAVY          0x000F
#define COLOR_DARKGREEN     0x03E0
#define COLOR_DARKCYAN      0x03EF
#define COLOR_MAROON        0x7800
#define COLOR_PURPLE        0x780F
#define COLOR_OLIVE         0x7BE0
#define COLOR_LIGHTGREY     0xC618
#define COLOR_DARKGREY      0x7BEF
#define COLOR_BLUE          0x001F
#define COLOR_GREEN         0x07E0
#define COLOR_CYAN          0x07FF
#define COLOR_RED           0xF800
#define COLOR_MAGENTA       0xF81F
#define COLOR_YELLOW        0xFFE0
#define COLOR_WHITE         0xFFFF
#define COLOR_ORANGE        0xFD20
#define COLOR_GREENYELLOW   0xAFE5
#define COLOR_PINK          0xF81F

//POTENTIAL SCRRENS 
//#define ILI9341_RDID4   0xD3
//#define ST7796_RDID4   0xD3

// Color macro RGB565
#define NV3029_COLOR565(r,g,b)  (uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3))

// Convert RGB888 to RGB565
#define RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))



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
#define NV3029_SPI_AF           GPIO_AF0_SPI1


// // NV3029 commands (mapped from ST7735 compatible commands used by this panel)
// #define NV3029_SWRESET 0x01
// #define NV3029_SLPOUT  0x11
// #define NV3029_DISPON  0x29
// #define NV3029_CASET   0x2A
// #define NV3029_RASET   0x2B
// #define NV3029_RAMWR   0x2C
// #define NV3029_MADCTL  0x36
// #define NV3029_COLMOD  0x3A

// Display dimensions for 
#define NV3029_WIDTH  128
#define NV3029_HEIGHT 160




// Define the signal types based on the function calls
typedef enum {
        CMD_LOW  = 0, // Maps to lcd_cmd_LOW_SEND_FUN
        DATA_HIGH = 1, // Maps to lcd_cmd_HIGH_SEND_FUN
        DELAY_TIME = 2  // Maps to lcd_cmd_DELAY_SEND_FUN
} SignalType;

// The structure to hold the transmission data
typedef struct {
        SignalType type;
            uint8_t value;
} LcdPacket;

void spi1_config(bool sixteen_bit);
static void spi1_onewire_bitbang(uint8_t tx_byte, uint8_t low_or_high);
void spi1_init_bitbang(void);
void spi1_pin_init(void);

void LCD_init(void);
void LCD_fill_screen(uint16_t color);
void LCD_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color);
void LCD_SetWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) ;
void LCD_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bgcolor, uint8_t scale);
void LCD_draw_string(uint16_t x, uint16_t y, const char* s, uint16_t color, uint16_t bgcolor, uint8_t scale);

/* DMA-based SPI transmit functions */
bool SPI1_tx_dma_mode(const uint8_t* buff, uint16_t len, uint32_t timeout_ms, uint8_t mode);
bool SPI1_tx_dma(const uint8_t* buff, uint16_t len, uint32_t timeout_ms);
bool SPI1_tx_dma_16bit(const uint16_t* buff, uint16_t count, uint32_t timeout_ms);
bool SPI1_tx_dma_async(const uint8_t* buff, uint16_t len, uint32_t timeout_ms);

/* High-level DMA I/O helpers with CS/DC control */
void LCD_SendCommand_DMA(uint8_t cmd);
void LCD_SendData_DMA(const uint8_t* data, uint16_t len);
void LCD_SendPixels_DMA(const uint16_t* pixels, uint16_t count);

void LCD_diag(void);
void LCD_test_grid(void);
void LCD_test_char(void);
void LCD_Send_Sequence(const LcdPacket* packet_array, uint32_t length);
void screen_Init(uint8_t screen_type);
extern void Delay(volatile uint32_t count);




// Standard 5x8 font (ASCII 32-127), 5 bytes per character
extern const uint8_t font5x8[][5];

#ifdef __cplusplus
}
#endif

#endif

