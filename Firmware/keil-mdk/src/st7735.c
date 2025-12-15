/* Minimal ST7735 driver implementation (for 80x160 ST7735 variant)
 * Uses the board's SPI peripheral (shared with sFLASH by default).
 * Pin mapping (adjustable in inc/st7735.h):
 *   CS = PA15, SCLK = PB3 (board), MOSI = PB5 (board) [SPI pins may be shared],
 *   RST = PB6, RS(DC) = PB7
 * Provides basic init, fill screen, and text draw (5x8 font)
 */

#include "st7735.h"
#include <string.h>

// ST7735 commands
#define ST7735_SWRESET 0x01
#define ST7735_SLPOUT  0x11
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_MADCTL  0x36
#define ST7735_COLMOD  0x3A

// Display dimensions for ST7735 80x160
#define ST7735_WIDTH  80
#define ST7735_HEIGHT 160

// Standard 5x8 font (ASCII 32-127), 5 bytes per character
static const uint8_t font5x8[][5] = {
{0x00,0x00,0x00,0x00,0x00}, // 32 ' '
{0x00,0x00,0x5F,0x00,0x00}, // 33 !
{0x00,0x07,0x00,0x07,0x00}, // 34 "
{0x14,0x7F,0x14,0x7F,0x14}, // 35 #
{0x24,0x2A,0x7F,0x2A,0x12}, // 36 $
{0x23,0x13,0x08,0x64,0x62}, // 37 %
{0x36,0x49,0x55,0x22,0x50}, // 38 &
{0x00,0x05,0x03,0x00,0x00}, // 39 '
{0x00,0x1C,0x22,0x41,0x00}, // 40 (
{0x00,0x41,0x22,0x1C,0x00}, // 41 )
{0x14,0x08,0x3E,0x08,0x14}, // 42 *
{0x08,0x08,0x3E,0x08,0x08}, // 43 +
{0x00,0x50,0x30,0x00,0x00}, // 44 ,
{0x08,0x08,0x08,0x08,0x08}, // 45 -
{0x00,0x60,0x60,0x00,0x00}, // 46 .
{0x20,0x10,0x08,0x04,0x02}, // 47 /
{0x3E,0x51,0x49,0x45,0x3E}, // 48 0
{0x00,0x42,0x7F,0x40,0x00}, // 49 1
{0x42,0x61,0x51,0x49,0x46}, // 50 2
{0x21,0x41,0x45,0x4B,0x31}, // 51 3
{0x18,0x14,0x12,0x7F,0x10}, // 52 4
{0x27,0x45,0x45,0x45,0x39}, // 53 5
{0x3C,0x4A,0x49,0x49,0x30}, // 54 6
{0x01,0x71,0x09,0x05,0x03}, // 55 7
{0x36,0x49,0x49,0x49,0x36}, // 56 8
{0x06,0x49,0x49,0x29,0x1E}, // 57 9
{0x00,0x36,0x36,0x00,0x00}, // 58 :
{0x00,0x56,0x36,0x00,0x00}, // 59 ;
{0x08,0x14,0x22,0x41,0x00}, // 60 <
{0x14,0x14,0x14,0x14,0x14}, // 61 =
{0x00,0x41,0x22,0x14,0x08}, // 62 >
{0x02,0x01,0x51,0x09,0x06}, // 63 ?
{0x32,0x49,0x79,0x41,0x3E}, // 64 @
{0x7E,0x11,0x11,0x11,0x7E}, // 65 A
{0x7F,0x49,0x49,0x49,0x36}, // 66 B
{0x3E,0x41,0x41,0x41,0x22}, // 67 C
{0x7F,0x41,0x41,0x22,0x1C}, // 68 D
{0x7F,0x49,0x49,0x49,0x41}, // 69 E
{0x7F,0x09,0x09,0x09,0x01}, // 70 F
{0x3E,0x41,0x49,0x49,0x7A}, // 71 G
{0x7F,0x08,0x08,0x08,0x7F}, // 72 H
{0x00,0x41,0x7F,0x41,0x00}, // 73 I
{0x20,0x40,0x41,0x3F,0x01}, // 74 J
{0x7F,0x08,0x14,0x22,0x41}, // 75 K
{0x7F,0x40,0x40,0x40,0x40}, // 76 L
{0x7F,0x02,0x0C,0x02,0x7F}, // 77 M
{0x7F,0x04,0x08,0x10,0x7F}, // 78 N
{0x3E,0x41,0x41,0x41,0x3E}, // 79 O
{0x7F,0x09,0x09,0x09,0x06}, // 80 P
{0x3E,0x41,0x51,0x21,0x5E}, // 81 Q
{0x7F,0x09,0x19,0x29,0x46}, // 82 R
{0x46,0x49,0x49,0x49,0x31}, // 83 S
{0x01,0x01,0x7F,0x01,0x01}, // 84 T
{0x3F,0x40,0x40,0x40,0x3F}, // 85 U
{0x1F,0x20,0x40,0x20,0x1F}, // 86 V
{0x7F,0x20,0x18,0x20,0x7F}, // 87 W
{0x63,0x14,0x08,0x14,0x63}, // 88 X
{0x03,0x04,0x78,0x04,0x03}, // 89 Y
{0x61,0x51,0x49,0x45,0x43}, // 90 Z
{0x00,0x7F,0x41,0x41,0x00}, // 91 [
{0x02,0x04,0x08,0x10,0x20}, // 92 \
{0x00,0x41,0x41,0x7F,0x00}, // 93 ]
{0x04,0x02,0x01,0x02,0x04}, // 94 ^
{0x40,0x40,0x40,0x40,0x40}, // 95 _
{0x00,0x01,0x02,0x04,0x00}, // 96 `
{0x20,0x54,0x54,0x54,0x78}, // 97 a
{0x7F,0x48,0x44,0x44,0x38}, // 98 b
{0x38,0x44,0x44,0x44,0x20}, // 99 c
{0x38,0x44,0x44,0x48,0x7F}, //100 d
{0x38,0x54,0x54,0x54,0x18}, //101 e
{0x08,0x7E,0x09,0x01,0x02}, //102 f
{0x0C,0x52,0x52,0x52,0x3E}, //103 g
{0x7F,0x08,0x04,0x04,0x78}, //104 h
{0x00,0x44,0x7D,0x40,0x00}, //105 i
{0x20,0x40,0x44,0x3D,0x00}, //106 j
{0x7F,0x10,0x28,0x44,0x00}, //107 k
{0x00,0x41,0x7F,0x40,0x00}, //108 l
{0x7C,0x04,0x18,0x04,0x78}, //109 m
{0x7C,0x08,0x04,0x04,0x78}, //110 n
{0x38,0x44,0x44,0x44,0x38}, //111 o
{0x7C,0x14,0x14,0x14,0x08}, //112 p
{0x08,0x14,0x14,0x18,0x7C}, //113 q
{0x7C,0x08,0x04,0x04,0x08}, //114 r
{0x48,0x54,0x54,0x54,0x20}, //115 s
{0x04,0x3F,0x44,0x40,0x20}, //116 t
{0x3C,0x40,0x40,0x20,0x7C}, //117 u
{0x1C,0x20,0x40,0x20,0x1C}, //118 v
{0x3C,0x40,0x30,0x40,0x3C}, //119 w
{0x44,0x28,0x10,0x28,0x44}, //120 x
{0x0C,0x50,0x50,0x50,0x3C}, //121 y
{0x44,0x64,0x54,0x4C,0x44}, //122 z
{0x00,0x08,0x36,0x41,0x00}, //123 {
{0x00,0x00,0x7F,0x00,0x00}, //124 |
{0x00,0x41,0x36,0x08,0x00}, //125 }
{0x10,0x08,0x08,0x10,0x08}, //126 ~
{0x00,0x00,0x00,0x00,0x00}  //127 DEL
};

// Transmit buffer using hardware SPI (SPI1 configured for display)
static void st7735_spi_tx(const uint8_t* buff, uint32_t len)
{
    for (uint32_t i = 0; i < len; ++i) {
        // wait until transmit buffer empty
        while (SPI_I2S_GetStatus(ST7735_SPI, SPI_I2S_TE_FLAG) == RESET) ;
        SPI_I2S_TransmitData(ST7735_SPI, buff[i]);
        // wait until a byte is received (read to clear RX flag)
        while (SPI_I2S_GetStatus(ST7735_SPI, SPI_I2S_RNE_FLAG) == RESET) ;
        (void)SPI_I2S_ReceiveData(ST7735_SPI);
    }
}

static void st7735_cs_low(void)  { GPIO_ResetBits(ST7735_CS_GPIO_PORT, ST7735_CS_PIN); }
static void st7735_cs_high(void) { GPIO_SetBits(ST7735_CS_GPIO_PORT, ST7735_CS_PIN); }
static void st7735_dc_cmd(void)  { GPIO_ResetBits(ST7735_DC_GPIO_PORT, ST7735_DC_PIN); }
static void st7735_dc_data(void) { GPIO_SetBits(ST7735_DC_GPIO_PORT, ST7735_DC_PIN); }
static void st7735_rst_low(void) { GPIO_ResetBits(ST7735_RST_GPIO_PORT, ST7735_RST_PIN); }
static void st7735_rst_high(void){ GPIO_SetBits(ST7735_RST_GPIO_PORT, ST7735_RST_PIN); }

static void st7735_write_command(uint8_t cmd)
{
    st7735_dc_cmd();
    st7735_cs_low();
    st7735_spi_tx(&cmd, 1);
    st7735_cs_high();
}

static void st7735_write_data(const uint8_t* buff, uint32_t len)
{
    st7735_dc_data();
    st7735_cs_low();
    st7735_spi_tx(buff, len);
    st7735_cs_high();
}

static void st7735_set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    uint8_t data[4];
    // column addr
    st7735_write_command(ST7735_CASET);
    data[0] = 0x00; data[1] = x0; data[2] = 0x00; data[3] = x1;
    st7735_write_data(data, 4);
    // row addr
    st7735_write_command(ST7735_RASET);
    data[0] = 0x00; data[1] = y0; data[2] = 0x00; data[3] = y1;
    st7735_write_data(data, 4);
    // write to RAM
    st7735_write_command(ST7735_RAMWR);
}

void st7735_init(void)
{
    GPIO_InitType gpio;
    GPIO_InitStruct(&gpio);
    // Enable GPIO clocks for used ports
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB, ENABLE);

    // init CS, DC, RST pins as outputs
    gpio.Pin = ST7735_CS_PIN;
    gpio.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitPeripheral(ST7735_CS_GPIO_PORT, &gpio);

    gpio.Pin = ST7735_DC_PIN;
    GPIO_InitPeripheral(ST7735_DC_GPIO_PORT, &gpio);

    gpio.Pin = ST7735_RST_PIN;
    GPIO_InitPeripheral(ST7735_RST_GPIO_PORT, &gpio);

    // Configure SCLK (PB3) and MOSI (PB5) as SPI1 alternate-function outputs
    gpio.Pin = ST7735_SCLK_PIN;
    gpio.GPIO_Mode = GPIO_MODE_AF_PP;
    gpio.GPIO_Alternate = ST7735_SPI_AF;
    GPIO_InitPeripheral(ST7735_SCLK_GPIO_PORT, &gpio);

    gpio.Pin = ST7735_MOSI_PIN;
    gpio.GPIO_Mode = GPIO_MODE_AF_PP;
    gpio.GPIO_Alternate = ST7735_SPI_AF;
    GPIO_InitPeripheral(ST7735_MOSI_GPIO_PORT, &gpio);

    // Enable SPI1 clock and AFIO
    RCC_EnableAPB2PeriphClk(ST7735_SPI_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);

    // Initialize SPI1 peripheral (master, 8-bit, MSB)
    {
        SPI_InitType SPI_InitStructure;
        SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
        SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;
        SPI_InitStructure.DataLen       = SPI_DATA_SIZE_8BITS;
        SPI_InitStructure.CLKPOL        = SPI_CLKPOL_HIGH;
        SPI_InitStructure.CLKPHA        = SPI_CLKPHA_SECOND_EDGE;
        SPI_InitStructure.NSS           = SPI_NSS_SOFT;
        SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_16;
        SPI_InitStructure.FirstBit      = SPI_FB_MSB;
        SPI_InitStructure.CRCPoly       = 7;
        SPI_Init(ST7735_SPI, &SPI_InitStructure);
        SPI_Enable(ST7735_SPI, ENABLE);
    }

    // hardware reset
    st7735_rst_low();
    for (volatile int i=0;i<200000;i++);
    st7735_rst_high();
    for (volatile int i=0;i<200000;i++);

    // init sequence (minimal)
    st7735_write_command(ST7735_SWRESET);
    Delay(50);
    st7735_write_command(ST7735_SLPOUT);
    Delay(120);
    // color mode 16-bit
    uint8_t colmod = 0x05; // 16-bit/pixel
    st7735_write_command(ST7735_COLMOD);
    st7735_write_data(&colmod, 1);
    Delay(10);
    st7735_write_command(ST7735_MADCTL);
    uint8_t mad = 0x00; // row/col order, may require tweak
    st7735_write_data(&mad, 1);
    Delay(10);
    st7735_write_command(ST7735_DISPON);
    Delay(100);
}

void st7735_fill_screen(uint16_t color)
{
    // fill entire screen with a single color
    st7735_set_addr_window(0, 0, ST7735_WIDTH-1, ST7735_HEIGHT-1);
    // send color data as high byte then low byte repeatedly using hardware SPI
    st7735_dc_data();
    st7735_cs_low();
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    uint8_t pix[2];
    pix[0] = hi; pix[1] = lo;
    for (int i = 0; i < (ST7735_WIDTH * ST7735_HEIGHT); ++i) {
        st7735_spi_tx(pix, 2);
    }
    st7735_cs_high();
}

// Draw a single 5x8 character (x,y in pixels)
void st7735_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bgcolor)
{
    if (c < 32 || c > 127) c = '?';
    uint8_t index = c - 32;
    // for simplicity we only support space and '!'..'"' above; fallback to space
    const uint8_t* glyph = font5x8[0];
    if (index < (sizeof(font5x8)/5)) glyph = font5x8[index];

    for (uint8_t col = 0; col < 5; ++col){
        uint8_t line = glyph[col];
        for (uint8_t row = 0; row < 8; ++row){
            uint16_t px = x + col;
            uint16_t py = y + row;
            if (line & (1<<row)){
                st7735_set_addr_window(px, py, px, py);
                uint8_t hi = color >> 8; uint8_t lo = color & 0xFF;
                uint8_t tmpc[2] = { hi, lo };
                st7735_write_data(tmpc, 2);
            } else {
                st7735_set_addr_window(px, py, px, py);
                uint8_t hi = bgcolor >> 8; uint8_t lo = bgcolor & 0xFF;
                uint8_t tmpb[2] = { hi, lo };
                st7735_write_data(tmpb, 2);
            }
        }
    }
    // one column spacing
    for (uint8_t row = 0; row < 8; ++row){
        uint16_t px = x + 5; uint16_t py = y + row;
        st7735_set_addr_window(px, py, px, py);
        uint8_t hi = bgcolor >> 8; uint8_t lo = bgcolor & 0xFF;
        uint8_t tmpb[2] = { hi, lo };
        st7735_write_data(tmpb, 2);
    }
}

void st7735_draw_string(uint16_t x, uint16_t y, const char* s, uint16_t color, uint16_t bgcolor)
{
    uint16_t cx = x;
    while (*s){
        st7735_draw_char(cx, y, *s, color, bgcolor);
        cx += 6; // 5px + 1 spacing
        s++;
    }
}
