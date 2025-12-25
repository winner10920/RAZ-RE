/* Minimal NV3029 driver implementation (renamed from st7735)
 * Uses the board's SPI peripheral (shared with sFLASH by default).
 * Pin mapping (adjustable in inc/nv3029.h):
 *   CS = PA15, SCLK = PB3 (board), MOSI = PB5 (board) [SPI pins may be shared],
 *   RST = PB6, RS(DC) = PB7
 * Provides basic init, fill screen, and text draw (5x8 font)
 */

#include "nv3029.h"
#include <string.h>

// --- The Initialization Sequence (From your dump) ---
const uint8_t NV3029_Init_Sequence[] = {
    0xFF, 0xA5, // Unlock
    0x3E, 0x08, 0x3A, 0x65, 0x82, 0x00, 0x98, 0x00, 0x63, 0x0F, 0x64, 0x0F,
    0xB4, 0x34, 0xB5, 0x30, 0x83, 0x13, 0x86, 0x04, 0x87, 0x16, 0x88, 0x25,
    0x89, 0x2F, 0x93, 0x63, 0x96, 0x81, 0xC3, 0x10, 0xE6, 0x00, 0x99, 0x01,
    0x44, 0x00,
    // Positive Gamma (0x70-0x7F)
    0x70, 0x07, 0x71, 0x19, 0x72, 0x1A, 0x73, 0x13, 0x74, 0x19, 0x75, 0x1D,
    0x76, 0x47, 0x77, 0x0A, 0x78, 0x07, 0x79, 0x47, 0x7A, 0x05, 0x7B, 0x09,
    0x7C, 0x0D, 0x7D, 0x0C, 0x7E, 0x0C, 0x7F, 0x08,
    // Negative Gamma (0xA0-0xAF)
    0xA0, 0x0B, 0xA1, 0x36, 0xA2, 0x09, 0xA3, 0x0D, 0xA4, 0x08, 0xA5, 0x23,
    0xA6, 0x3B, 0xA7, 0x04, 0xA8, 0x07, 0xA9, 0x38, 0xAA, 0x0A, 0xAB, 0x12,
    0xAC, 0x0C, 0xAD, 0x07, 0xAE, 0x2F, 0xAF, 0x07,
    0xFF, 0x00 // Lock
};

// Standard 5x8 font (ASCII 32-127), 5 bytes per character
const uint8_t font5x8[][5] = {
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
static void nv3029_spi_tx(const uint8_t* buff, uint32_t len)
{
    for (uint32_t i = 0; i < len; ++i) {
        // wait until transmit buffer empty
        while (SPI_I2S_GetStatus(NV3029_SPI, SPI_I2S_BUSY_FLAG) == SET) ;
        SPI_I2S_TransmitData(NV3029_SPI, buff[i]);
        //while (SPI_I2S_GetFlagStatus(NV3029_SPI, SPI_I2S_FLAG_BUSY) == SET);

        // RX data is ignored for display writes; read if required by peripheral
        // (void)SPI_I2S_ReceiveData(NV3029_SPI);
    }
}

static void nv3029_cs_low(void)  { GPIO_ResetBits(NV3029_CS_GPIO_PORT, NV3029_CS_PIN); }
static void nv3029_cs_high(void) { GPIO_SetBits(NV3029_CS_GPIO_PORT, NV3029_CS_PIN); }
static void nv3029_dc_cmd(void)  { GPIO_ResetBits(NV3029_DC_GPIO_PORT, NV3029_DC_PIN); }
static void nv3029_dc_data(void) { GPIO_SetBits(NV3029_DC_GPIO_PORT, NV3029_DC_PIN); }
static void nv3029_rst_low(void) { GPIO_ResetBits(NV3029_RST_GPIO_PORT, NV3029_RST_PIN); }
static void nv3029_rst_high(void){ GPIO_SetBits(NV3029_RST_GPIO_PORT, NV3029_RST_PIN); }

static void nv3029_write_command(uint8_t cmd)
{
    nv3029_dc_cmd();
    nv3029_cs_low();

    nv3029_spi_tx(&cmd, 1);

    nv3029_cs_high();
}

static void nv3029_write_data(const uint8_t* buff, uint32_t len)
{
    nv3029_dc_data();
    nv3029_cs_low();

    nv3029_spi_tx(buff, len);

    nv3029_cs_high();
}

static void nv3029_set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    uint8_t data[4];
    // column addr
    nv3029_write_command(NV3029_CASET);
    data[0] = 0x00; data[1] = x0; data[2] = 0x00; data[3] = x1;
    nv3029_write_data(data, 4);
    // row addr
    nv3029_write_command(NV3029_RASET);
    data[0] = 0x00; data[1] = y0; data[2] = 0x00; data[3] = y1;
    nv3029_write_data(data, 4);
    // write to RAM
    nv3029_write_command(NV3029_RAMWR);
}

void nv3029_init(void)
{
    GPIO_InitType GPIO_InitStructure;

    GPIO_InitStruct(&GPIO_InitStructure);
    // Enable GPIO clocks for used ports (explicit, do not depend on sFLASH macros)
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB, ENABLE);

    // Enable SPI clock and AFIO
    RCC_EnableAPB2PeriphClk(NV3029_SPI_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);

    // init CS, DC, RST pins as outputs
    GPIO_InitStructure.Pin = NV3029_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitPeripheral(NV3029_CS_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = NV3029_DC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitPeripheral(NV3029_DC_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = NV3029_RST_PIN;
    GPIO_InitPeripheral(NV3029_RST_GPIO_PORT, &GPIO_InitStructure);

    // Configure SCLK (PB3) and MOSI (PB5) as SPI1 alternate-function outputs
    GPIO_InitStructure.Pin = NV3029_SCLK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = NV3029_SPI_AF;
    GPIO_InitPeripheral(NV3029_SCLK_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = NV3029_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = NV3029_SPI_AF;
    GPIO_InitPeripheral(NV3029_MOSI_GPIO_PORT, &GPIO_InitStructure);

    // Initialize SPI peripheral (master, 8-bit, MSB)
    {
        SPI_InitType SPI_InitStructure;
        SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
        SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;
        SPI_InitStructure.DataLen       = SPI_DATA_SIZE_8BITS;
        SPI_InitStructure.CLKPOL        = SPI_CLKPOL_HIGH;
        SPI_InitStructure.CLKPHA        = SPI_CLKPHA_SECOND_EDGE;
        SPI_InitStructure.NSS           = SPI_NSS_SOFT;
        SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_2;
        SPI_InitStructure.FirstBit      = SPI_FB_MSB;
        SPI_InitStructure.CRCPoly       = 7;
        SPI_Init(NV3029_SPI, &SPI_InitStructure);
        SPI_Enable(NV3029_SPI, ENABLE);
    }

    

    // hardware reset
    nv3029_rst_high();
    Delay(10);
    nv3029_rst_low();
    Delay(10);
    nv3029_rst_high();
    Delay(120);

    nv3029_write_command(NV3029_SLPOUT);
    Delay(120);
    // Use the reverse-engineered NV3029_Init_Sequence array to initialize the panel.
    // The array contains pairs {cmd, data}
    const uint16_t len = sizeof(NV3029_Init_Sequence);
    for (uint16_t i = 0; i + 1 < len; i += 2) {
        uint8_t cmd = NV3029_Init_Sequence[i];
        uint8_t data = NV3029_Init_Sequence[i + 1];
        nv3029_write_command(cmd);
        nv3029_write_data(&data, 1);
        // small delay between commands may be required for some entries; add if necessary
    }

    nv3029_write_command(NV3029_MADCTL);
    nv3029_write_data((uint8_t*)"\x80", 1); // MADCTL: MY=1, MX=0, MV=0, ML=0, RGB=0, MH=0
    // Finally, ensure display on
    nv3029_write_command(NV3029_DISPON);
    Delay(10);
    nv3029_write_command(NV3029_RAMWR);

}


void nv3029_fill_screen(uint16_t color)
{
    // fill entire screen with a single color
    nv3029_set_addr_window(0, 0, NV3029_WIDTH-1, NV3029_HEIGHT-1);
    // send color data as high byte then low byte repeatedly using hardware SPI
    nv3029_dc_data();
    nv3029_cs_low();
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    uint8_t pix[2];
    pix[0] = hi; pix[1] = lo;
    for (int i = 0; i < (NV3029_WIDTH * NV3029_HEIGHT); ++i) {
        nv3029_spi_tx(pix, 2);
    }
    nv3029_cs_high();
}

// Draw a single 5x8 character (x,y in pixels)
void nv3029_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bgcolor)
{
    if (c < 32 || c > 127) c = '?';
    uint8_t index = c - 32;
    const uint8_t* glyph = font5x8[0];
    if (index < (sizeof(font5x8)/5)) glyph = font5x8[index];

    for (uint8_t col = 0; col < 5; ++col){
        uint8_t line = glyph[col];
        for (uint8_t row = 0; row < 8; ++row){
            uint16_t px = x + col;
            uint16_t py = y + row;
            nv3029_set_addr_window(px, py, px, py);
            uint8_t hi = (line & (1<<row)) ? (color >> 8) : (bgcolor >> 8);
            uint8_t lo = (line & (1<<row)) ? (color & 0xFF) : (bgcolor & 0xFF);
            uint8_t tmp[2] = { hi, lo };
            nv3029_write_data(tmp, 2);
        }
    }
    // one column spacing
    for (uint8_t row = 0; row < 8; ++row){
        uint16_t px = x + 5; uint16_t py = y + row;
        nv3029_set_addr_window(px, py, px, py);
        uint8_t hi = bgcolor >> 8; uint8_t lo = bgcolor & 0xFF;
        uint8_t tmpb[2] = { hi, lo };
        nv3029_write_data(tmpb, 2);
    }
}

void nv3029_draw_string(uint16_t x, uint16_t y, const char* s, uint16_t color, uint16_t bgcolor)
{
    uint16_t cx = x;
    while (*s){
        nv3029_draw_char(cx, y, *s, color, bgcolor);
        cx += 6; // 5px + 1 spacing
        s++;
    }
}

// Diagnostic routine: reset display, then fill with a series of colors
void nv3029_diag(void)
{
    // Hardware reset sequence
    nv3029_rst_low();
    Delay(20000);
    nv3029_rst_high();
    Delay(20000);

    // A few colors to verify SPI and LCD functioning
    nv3029_fill_screen(NV3029_COLOR565(255,0,0)); // red
    Delay(5000);
    nv3029_fill_screen(NV3029_COLOR565(0,255,0)); // green
    Delay(5000);
    nv3029_fill_screen(NV3029_COLOR565(0,0,255)); // blue
    Delay(5000);
    nv3029_fill_screen(NV3029_COLOR565(255,255,255)); // white
    Delay(5000);
    nv3029_fill_screen(NV3029_COLOR565(0,0,0)); // black
    Delay(2000);
}
