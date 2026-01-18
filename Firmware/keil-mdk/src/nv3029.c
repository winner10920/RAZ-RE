/* Minimal NV3029 driver implementation (renamed from st7735)
 * Uses the board's SPI peripheral (shared with sFLASH by default).
 * Pin mapping (adjustable in inc/nv3029.h):
 *   CS = PA15, SCLK = PB3 (board), MOSI = PB5 (board) [SPI pins may be shared],
 *   RST = PB6, RS(DC) = PB7
 * Provides basic init, fill screen, and text draw (5x8 font)
 */

#include "nv3029.h"
#include "n32g031.h"
#include "n32g031_gpio.h"
#include "n32g031_spi.h"
#include "dma.h"
#include <string.h>

volatile uint16_t peek_value[] = {0,0,0,0,0,0,0,0,0,0,0};
volatile uint32_t peek_len = 0;
volatile uint8_t spi_rx_data[4] = {0, 0, 0, 0};
volatile uint8_t spi_rx_data_d3[4] = {0,0, 0, 0};


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
{0x02,0x04,0x08,0x10,0x20}, // 92 backslash
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

static const LcdPacket screen_init_seq_0[] = {
    {CMD_LOW, 0xFF}, {DATA_HIGH, 0xA5},
    {CMD_LOW, 0x3E}, {DATA_HIGH, 0x08},
    {CMD_LOW, LCD_CMD_COLMOD}, {DATA_HIGH, 0x65},
    {CMD_LOW, 0x82}, {DATA_HIGH, 0x00},
    {CMD_LOW, 0x98}, {DATA_HIGH, 0x00},
    {CMD_LOW, 0x63}, {DATA_HIGH, 0x0F}, // 99 -> 0x63
    {CMD_LOW, 0x64}, {DATA_HIGH, 0x0F}, // 100 -> 0x64
    {CMD_LOW, LCD_CMD_INVCTR}, {DATA_HIGH, 0x34},
    {CMD_LOW, 0xB5}, {DATA_HIGH, 0x30},
    {CMD_LOW, 0x83}, {DATA_HIGH, 0x13},
    {CMD_LOW, 0x86}, {DATA_HIGH, 0x04},
    {CMD_LOW, 0x87}, {DATA_HIGH, 0x16},
    {CMD_LOW, 0x88}, {DATA_HIGH, 0x25},
    {CMD_LOW, 0x89}, {DATA_HIGH, 0x2F},
    {CMD_LOW, 0x93}, {DATA_HIGH, 0x63}, // 99 -> 0x63
    {CMD_LOW, 0x96}, {DATA_HIGH, 0x81},
    {CMD_LOW, LCD_CMD_PWCTR4}, {DATA_HIGH, 0x10},
    {CMD_LOW, 0xE6}, {DATA_HIGH, 0x00},
    {CMD_LOW, 0x99}, {DATA_HIGH, 0x01},
    {CMD_LOW, 0x44}, {DATA_HIGH, 0x00},
    {CMD_LOW, 0x70}, {DATA_HIGH, 0x07},
    {CMD_LOW, 0x71}, {DATA_HIGH, 0x19},
    {CMD_LOW, 0x72}, {DATA_HIGH, 0x1A},
    {CMD_LOW, 0x73}, {DATA_HIGH, 0x13},
    {CMD_LOW, 0x74}, {DATA_HIGH, 0x19},
    {CMD_LOW, 0x75}, {DATA_HIGH, 0x1D},
    {CMD_LOW, 0x76}, {DATA_HIGH, 0x47},
    {CMD_LOW, 0x77}, {DATA_HIGH, 0x0A}, // 10 -> 0x0A
    {CMD_LOW, 0x78}, {DATA_HIGH, 0x07},
    {CMD_LOW, 0x79}, {DATA_HIGH, 0x47},
    {CMD_LOW, 0x7A}, {DATA_HIGH, 0x05},
    {CMD_LOW, 0x7B}, {DATA_HIGH, 0x09},
    {CMD_LOW, 0x7C}, {DATA_HIGH, 0x0D},
    {CMD_LOW, 0x7D}, {DATA_HIGH, 0x0C},
    {CMD_LOW, 0x7E}, {DATA_HIGH, 0x0C},
    {CMD_LOW, 0x7F}, {DATA_HIGH, 0x08},
    {CMD_LOW, 0xA0}, {DATA_HIGH, 0x0B},
    {CMD_LOW, 0xA1}, {DATA_HIGH, 0x36},
    {CMD_LOW, 0xA2}, {DATA_HIGH, 0x09},
    {CMD_LOW, 0xA3}, {DATA_HIGH, 0x0D},
    {CMD_LOW, 0xA4}, {DATA_HIGH, 0x08},
    {CMD_LOW, 0xA5}, {DATA_HIGH, 0x23},
    {CMD_LOW, 0xA6}, {DATA_HIGH, 0x3B},
    {CMD_LOW, 0xA7}, {DATA_HIGH, 0x04},
    {CMD_LOW, 0xA8}, {DATA_HIGH, 0x07},
    {CMD_LOW, 0xA9}, {DATA_HIGH, 0x38},
    {CMD_LOW, 0xAA}, {DATA_HIGH, 0x0A}, // 10 -> 0x0A
    {CMD_LOW, 0xAB}, {DATA_HIGH, 0x12},
    {CMD_LOW, 0xAC}, {DATA_HIGH, 0x0C},
    {CMD_LOW, 0xAD}, {DATA_HIGH, 0x07},
    {CMD_LOW, 0xAE}, {DATA_HIGH, 0x2F},
    {CMD_LOW, 0xAF}, {DATA_HIGH, 0x07},
    {CMD_LOW, 0xFF}, {DATA_HIGH, 0x00}
};

// Alternative sequence from decompiled firmware (cVar1 == 1)
static const LcdPacket screen_init_seq_1_alt[] = {
    {CMD_LOW, 0xFF}, {DATA_HIGH, 0xA5},
    {CMD_LOW, 0x3E}, {DATA_HIGH, 0x08},
    {CMD_LOW, LCD_CMD_COLMOD}, {DATA_HIGH, 0x65},
    {CMD_LOW, 0x82}, {DATA_HIGH, 0x00},
    {CMD_LOW, 0x98}, {DATA_HIGH, 0x00},
    {CMD_LOW, 0x63}, {DATA_HIGH, 0x0F},
    {CMD_LOW, 0x64}, {DATA_HIGH, 0x0F},
    {CMD_LOW, LCD_CMD_INVCTR}, {DATA_HIGH, 0x34},
    {CMD_LOW, 0xB5}, {DATA_HIGH, 0x30},
    {CMD_LOW, 0x83}, {DATA_HIGH, 0x13},
    {CMD_LOW, 0x86}, {DATA_HIGH, 0x04},
    {CMD_LOW, 0x87}, {DATA_HIGH, 0x19},  // Different from seq_0 (0x16)
    {CMD_LOW, 0x88}, {DATA_HIGH, 0x2F},  // Different from seq_0 (0x25)
    {CMD_LOW, 0x89}, {DATA_HIGH, 0x36},  // Different from seq_0 (0x2F)
    {CMD_LOW, 0x93}, {DATA_HIGH, 0x63},
    {CMD_LOW, 0x96}, {DATA_HIGH, 0x81},
    {CMD_LOW, LCD_CMD_PWCTR4}, {DATA_HIGH, 0x10},
    {CMD_LOW, 0xE6}, {DATA_HIGH, 0x00},
    {CMD_LOW, 0x99}, {DATA_HIGH, 0x01},
    {CMD_LOW, 0x44}, {DATA_HIGH, 0x00},
    {CMD_LOW, 0x70}, {DATA_HIGH, 0x07},
    {CMD_LOW, 0x71}, {DATA_HIGH, 0x19},
    {CMD_LOW, 0x72}, {DATA_HIGH, 0x1A},
    {CMD_LOW, 0x73}, {DATA_HIGH, 0x13},
    {CMD_LOW, 0x74}, {DATA_HIGH, 0x19},
    {CMD_LOW, 0x75}, {DATA_HIGH, 0x1D},
    {CMD_LOW, 0x76}, {DATA_HIGH, 0x47},
    {CMD_LOW, 0x77}, {DATA_HIGH, 0x0A},
    {CMD_LOW, 0x78}, {DATA_HIGH, 0x07},
    {CMD_LOW, 0x79}, {DATA_HIGH, 0x47},
    {CMD_LOW, 0x7A}, {DATA_HIGH, 0x05},
    {CMD_LOW, 0x7B}, {DATA_HIGH, 0x09},
    {CMD_LOW, 0x7C}, {DATA_HIGH, 0x0D},
    {CMD_LOW, 0x7D}, {DATA_HIGH, 0x0C},
    {CMD_LOW, 0x7E}, {DATA_HIGH, 0x0C},
    {CMD_LOW, 0x7F}, {DATA_HIGH, 0x08},
    {CMD_LOW, 0xA0}, {DATA_HIGH, 0x0B},
    {CMD_LOW, 0xA1}, {DATA_HIGH, 0x36},
    {CMD_LOW, 0xA2}, {DATA_HIGH, 0x09},
    {CMD_LOW, 0xA3}, {DATA_HIGH, 0x0D},
    {CMD_LOW, 0xA4}, {DATA_HIGH, 0x08},
    {CMD_LOW, 0xA5}, {DATA_HIGH, 0x23},
    {CMD_LOW, 0xA6}, {DATA_HIGH, 0x3B},
    {CMD_LOW, 0xA7}, {DATA_HIGH, 0x04},
    {CMD_LOW, 0xA8}, {DATA_HIGH, 0x07},
    {CMD_LOW, 0xA9}, {DATA_HIGH, 0x38},
    {CMD_LOW, 0xAA}, {DATA_HIGH, 0x0A},
    {CMD_LOW, 0xAB}, {DATA_HIGH, 0x12},
    {CMD_LOW, 0xAC}, {DATA_HIGH, 0x0C},
    {CMD_LOW, 0xAD}, {DATA_HIGH, 0x07},
    {CMD_LOW, 0xAE}, {DATA_HIGH, 0x2F},
    {CMD_LOW, 0xAF}, {DATA_HIGH, 0x07},
    {CMD_LOW, 0xFF}, {DATA_HIGH, 0x00}
};

static const LcdPacket screen_init_seq_1[] = {
    {CMD_LOW, 0xFF},  {DATA_HIGH, 0xA5},  
    {CMD_LOW, 0x3E},  {DATA_HIGH, 0x08},  
    {CMD_LOW, 0x3A},  {DATA_HIGH, 0x65},  
    {CMD_LOW, 0x82},  {DATA_HIGH, 0x00},  
    {CMD_LOW, 0x98},  {DATA_HIGH, 0x00},  
    {CMD_LOW, 0x63}, {DATA_HIGH, 0x0F}, 
    {CMD_LOW, 0x64}, {DATA_HIGH, 0x0F}, 
    {CMD_LOW, 0xB4}, {DATA_HIGH, 0x34}, 
    {CMD_LOW, 0xB5},  {DATA_HIGH, 0x30},  
    {CMD_LOW, 0x83}, {DATA_HIGH, 0x13}, 
    {CMD_LOW, 0x86}, {DATA_HIGH, 0x04}, 
    {CMD_LOW, 0x87},  {DATA_HIGH, 0x16},  
    {CMD_LOW, 0x88},  {DATA_HIGH, 0x25},  
    {CMD_LOW, 0x89}, {DATA_HIGH, 0x2F}, 
    {CMD_LOW, 0x93},  {DATA_HIGH, 0x63}, 
    {CMD_LOW, 0x96},  {DATA_HIGH, 0x81},  
    {CMD_LOW, 0xC3}, {DATA_HIGH, 0x10}, 
    {CMD_LOW, 0xE6},  {DATA_HIGH, 0x00},  
    {CMD_LOW, 0x99},   {DATA_HIGH, 0x01},
    {CMD_LOW, 0x44},  {DATA_HIGH, 0x00},  
    {CMD_LOW, 0x70},  {DATA_HIGH, 0x07},  
    {CMD_LOW, 0x71},  {DATA_HIGH, 0x19},  
    {CMD_LOW, 0x72},   {DATA_HIGH, 0x1A},
    {CMD_LOW, 0x73},   {DATA_HIGH, 0x13},
    {CMD_LOW, 0x74},  {DATA_HIGH, 0x19},
    {CMD_LOW, 0x75},  {DATA_HIGH, 0x1D},  
    {CMD_LOW, 0x76},  {DATA_HIGH, 0x47},  
    {CMD_LOW, 0x77},  {DATA_HIGH, 0x0A},  
    {CMD_LOW, 0x78},  {DATA_HIGH, 0x07},  
    {CMD_LOW, 0x79},  {DATA_HIGH, 0x47},  
    {CMD_LOW, 0x7A},  {DATA_HIGH, 0x05},  
    {CMD_LOW, 0x7B},  {DATA_HIGH, 0x09},  
    {CMD_LOW, 0x7C},  {DATA_HIGH, 0x0D},  
    {CMD_LOW, 0x7D},  {DATA_HIGH, 0x0C},  
    {CMD_LOW, 0x7E},   {DATA_HIGH, 0x0C},
    {CMD_LOW, 0x7F},  {DATA_HIGH, 0x08},  
    {CMD_LOW, 0xA0},  {DATA_HIGH, 0x0B},  
    {CMD_LOW, 0xA1},  {DATA_HIGH, 0x36},  
    {CMD_LOW, 0xA2},  {DATA_HIGH, 0x09},  
    {CMD_LOW, 0xA3},  {DATA_HIGH, 0x0D},  
    {CMD_LOW, 0xA4},  {DATA_HIGH, 0x08},  
    {CMD_LOW, 0xA5},  {DATA_HIGH, 0x23},  
    {CMD_LOW, 0xA6},  {DATA_HIGH, 0x3B},  
    {CMD_LOW, 0xA7},  {DATA_HIGH, 0x04},  
    {CMD_LOW, 0xA8},  {DATA_HIGH, 0x07},  
    {CMD_LOW, 0xA9},  {DATA_HIGH, 0x38},  
    {CMD_LOW, 0xAA},  {DATA_HIGH, 0x0A},  
    {CMD_LOW, 0xAB},  {DATA_HIGH, 0x12},  
    {CMD_LOW, 0xAC},  {DATA_HIGH, 0x0C},  
    {CMD_LOW, 0xAD},   {DATA_HIGH, 0x07},
    {CMD_LOW, 0xAE},  {DATA_HIGH, 0x2F},  
    {CMD_LOW, 0xAF},   {DATA_HIGH, 0x07},
    {CMD_LOW, 0xFF},  {DATA_HIGH, 0x00}
};

// static const LcdPacket screen_init_seq_1[] = {
//     {CMD_LOW, 0xFF}, {DELAY_TIME, 15}, {DATA_HIGH, 0xA5}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x3E}, {DELAY_TIME, 45}, {DATA_HIGH, 0x08}, {DELAY_TIME, 15}, 
//     {CMD_LOW, LCD_CMD_COLMOD}, {DELAY_TIME, 15}, {DATA_HIGH, 0x65}, {DELAY_TIME, 45}, 
//     {CMD_LOW, 0x82}, {DELAY_TIME, 15}, {DATA_HIGH, 0x00}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x98}, {DELAY_TIME, 45}, {DATA_HIGH, 0x00}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x63},{DELAY_TIME, 45}, 
//     {DATA_HIGH, 0x0F}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x64},{DELAY_TIME, 15}, {DATA_HIGH, 0x0F}, {DELAY_TIME, 45}, 
//     {CMD_LOW, LCD_CMD_INVCTR},{DELAY_TIME, 15}, {DATA_HIGH, 0x34}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0xB5}, {DELAY_TIME, 45}, {DATA_HIGH, 0x30}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x83}, {DELAY_TIME, 45},
//     {DATA_HIGH, 0x13}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x86}, {DELAY_TIME, 15},{DATA_HIGH, 0x04}, {DELAY_TIME, 45}, 
//     {CMD_LOW, 0x87}, {DELAY_TIME, 15}, {DATA_HIGH, 0x16}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x88}, {DELAY_TIME, 45}, {DATA_HIGH, 0x25}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x89}, {DELAY_TIME, 15},{DATA_HIGH, 0x2F}, {DELAY_TIME, 45}, 
//     {CMD_LOW, 0x93}, {DELAY_TIME, 15}, {DATA_HIGH, 0x63}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x96}, {DELAY_TIME, 45}, {DATA_HIGH, 0x81}, {DELAY_TIME, 15}, 
//     {CMD_LOW, LCD_CMD_PWCTR4}, {DELAY_TIME, 15},{DATA_HIGH, 0x10}, {DELAY_TIME, 45}, 
//     {CMD_LOW, 0xE6}, {DELAY_TIME, 65}, {DATA_HIGH, 0x00}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x99}, {DELAY_TIME, 15},  {DATA_HIGH, 0x01}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x44}, {DELAY_TIME, 45}, {DATA_HIGH, 0x00}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x70}, {DELAY_TIME, 15}, {DATA_HIGH, 0x07}, {DELAY_TIME, 45}, 
//     {CMD_LOW, 0x71}, {DELAY_TIME, 15}, {DATA_HIGH, 0x19}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x72}, {DELAY_TIME, 45},  {DATA_HIGH, 0x1A}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x73}, {DELAY_TIME, 15},  {DATA_HIGH, 0x13}, {DELAY_TIME, 45}, 
//     {CMD_LOW, 0x74}, {DELAY_TIME, 15}, {DATA_HIGH, 0x19},{DELAY_TIME, 15}, 
//     {CMD_LOW, 0x75}, {DELAY_TIME, 45}, {DATA_HIGH, 0x1D}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x76}, {DELAY_TIME, 15}, {DATA_HIGH, 0x47}, {DELAY_TIME, 45}, 
//     {CMD_LOW, 0x77}, {DELAY_TIME, 15}, {DATA_HIGH, 0x0A}, {DELAY_TIME, 45}, 
//     {CMD_LOW, 0x78}, {DELAY_TIME, 15}, {DATA_HIGH, 0x07}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x79}, {DELAY_TIME, 45}, {DATA_HIGH, 0x47}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x7A}, {DELAY_TIME, 15}, {DATA_HIGH, 0x05}, {DELAY_TIME, 45}, 
//     {CMD_LOW, 0x7B}, {DELAY_TIME, 15}, {DATA_HIGH, 0x09}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x7C}, {DELAY_TIME, 45}, {DATA_HIGH, 0x0D}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x7D}, {DELAY_TIME, 45}, {DATA_HIGH, 0x0C}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0x7E}, {DELAY_TIME, 15},  {DATA_HIGH, 0x0C}, {DELAY_TIME, 45}, 
//     {CMD_LOW, 0x7F}, {DELAY_TIME, 15}, {DATA_HIGH, 0x08}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0xA0}, {DELAY_TIME, 70}, {DATA_HIGH, 0x0B}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0xA1}, {DELAY_TIME, 45}, {DATA_HIGH, 0x36}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0xA2}, {DELAY_TIME, 45}, {DATA_HIGH, 0x09}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0xA3}, {DELAY_TIME, 15}, {DATA_HIGH, 0x0D}, {DELAY_TIME, 45}, 
//     {CMD_LOW, 0xA4}, {DELAY_TIME, 15}, {DATA_HIGH, 0x08}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0xA5}, {DELAY_TIME, 45}, {DATA_HIGH, 0x23}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0xA6}, {DELAY_TIME, 15}, {DATA_HIGH, 0x3B}, {DELAY_TIME, 45}, 
//     {CMD_LOW, 0xA7}, {DELAY_TIME, 15}, {DATA_HIGH, 0x04}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0xA8}, {DELAY_TIME, 45}, {DATA_HIGH, 0x07}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0xA9}, {DELAY_TIME, 45}, {DATA_HIGH, 0x38}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0xAA}, {DELAY_TIME, 15}, {DATA_HIGH, 0x0A}, {DELAY_TIME, 45}, 
//     {CMD_LOW, 0xAB}, {DELAY_TIME, 15}, {DATA_HIGH, 0x12}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0xAC}, {DELAY_TIME, 45}, {DATA_HIGH, 0x0C}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0xAD}, {DELAY_TIME, 15},  {DATA_HIGH, 0x07}, {DELAY_TIME, 45}, 
//     {CMD_LOW, 0xAE}, {DELAY_TIME, 15}, {DATA_HIGH, 0x2F}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0xAF}, {DELAY_TIME, 45},  {DATA_HIGH, 0x07}, {DELAY_TIME, 15}, 
//     {CMD_LOW, 0xFF}, {DELAY_TIME, 15}, {DATA_HIGH, 0x00}, {DELAY_TIME, 45}
// };


static const LcdPacket screen_init_seq_2[] = {
    {CMD_LOW, LCD_CMD_FRMCTR1}, {DATA_HIGH, 0x05}, {DATA_HIGH, 0x3C}, {DATA_HIGH, 0x3C},
    {CMD_LOW, LCD_CMD_FRMCTR2}, {DATA_HIGH, 0x05}, {DATA_HIGH, 0x3C}, {DATA_HIGH, 0x3C},
    {CMD_LOW, LCD_CMD_FRMCTR3}, {DATA_HIGH, 0x05}, {DATA_HIGH, 0x3C}, {DATA_HIGH, 0x3C}, {DATA_HIGH, 0x05}, {DATA_HIGH, 0x3C}, {DATA_HIGH, 0x3C},
    {CMD_LOW, LCD_CMD_INVCTR}, {DATA_HIGH, 0x03},
    {CMD_LOW, LCD_CMD_PWCTR1}, {DATA_HIGH, 0xA4}, {DATA_HIGH, 0x04}, {DATA_HIGH, 0x84},
    {CMD_LOW, LCD_CMD_PWCTR2}, {DATA_HIGH, 0xC1},
    {CMD_LOW, LCD_CMD_PWCTR3}, {DATA_HIGH, 0x0D}, {DATA_HIGH, 0x00},
    {CMD_LOW, LCD_CMD_PWCTR4}, {DATA_HIGH, 0x8D}, {DATA_HIGH, 0x2A},
    {CMD_LOW, LCD_CMD_PWCTR5}, {DATA_HIGH, 0x8D}, {DATA_HIGH, 0xEE},
    {CMD_LOW, LCD_CMD_VMCTR1}, {DATA_HIGH, 0x0C},
    // Gamma Correction positive polarity
    {CMD_LOW, LCD_CMD_GMCTRP1}, {DATA_HIGH, 0x0B}, {DATA_HIGH, 0x21}, {DATA_HIGH, 0x0C}, {DATA_HIGH, 0x16}, {DATA_HIGH, 0x2E}, {DATA_HIGH, 0x27}, {DATA_HIGH, 0x1F}, {DATA_HIGH, 0x25}, {DATA_HIGH, 0x25}, {DATA_HIGH, 0x24}, {DATA_HIGH, 0x2C}, {DATA_HIGH, 0x38}, {DATA_HIGH, 0x00}, {DATA_HIGH, 0x03}, {DATA_HIGH, 0x00}, {DATA_HIGH, 0x10},
    // Gamma Correction negative polarity
    {CMD_LOW, LCD_CMD_GMCTRN1}, {DATA_HIGH, 0x0E}, {DATA_HIGH, 0x1E}, {DATA_HIGH, 0x0E}, {DATA_HIGH, 0x18}, {DATA_HIGH, 0x33}, {DATA_HIGH, 0x2D}, {DATA_HIGH, 0x28}, {DATA_HIGH, 0x2A}, {DATA_HIGH, 0x28}, {DATA_HIGH, 0x26}, {DATA_HIGH, 0x2F}, {DATA_HIGH, 0x3C}, {DATA_HIGH, 0x00}, {DATA_HIGH, 0x05}, {DATA_HIGH, 0x05}, {DATA_HIGH, 0x10},
    {CMD_LOW, LCD_CMD_TEON}, {DATA_HIGH, 0x00},
    {CMD_LOW, LCD_CMD_COLMOD}, {DATA_HIGH, 0x55}
};

static void LCD_cs_low(void)  { GPIO_ResetBits(NV3029_CS_GPIO_PORT, NV3029_CS_PIN); }
static void LCD_cs_high(void) { GPIO_SetBits(NV3029_CS_GPIO_PORT, NV3029_CS_PIN); }
static void LCD_dc_cmd(void)  { GPIO_ResetBits(NV3029_DC_GPIO_PORT, NV3029_DC_PIN); }
static void LCD_dc_data(void) { GPIO_SetBits(NV3029_DC_GPIO_PORT, NV3029_DC_PIN); }
static void LCD_rst_low(void) { GPIO_ResetBits(NV3029_RST_GPIO_PORT, NV3029_RST_PIN); }
static void LCD_rst_high(void){ GPIO_SetBits(NV3029_RST_GPIO_PORT, NV3029_RST_PIN); }
static void LCD_sclk_high(void){ GPIO_SetBits(NV3029_SCLK_GPIO_PORT, NV3029_SCLK_PIN); }
static void LCD_sclk_low(void) { GPIO_ResetBits(NV3029_SCLK_GPIO_PORT, NV3029_SCLK_PIN); }
static void LCD_mosi_high(void){ GPIO_SetBits(NV3029_MOSI_GPIO_PORT, NV3029_MOSI_PIN); }
static void LCD_mosi_low(void) { GPIO_ResetBits(NV3029_MOSI_GPIO_PORT, NV3029_MOSI_PIN); }  



// Transmit buffer using hardware SPI (SPI1 configured for display)
static void SPI1_tx(const uint8_t* buff, uint32_t len)
{
    
    SPI_Enable(NV3029_SPI, ENABLE);
    for (uint32_t i = 0; i < len; ++i) {
        peek_value[0] = buff[i];
        peek_len = len;
        
        while (SPI_I2S_GetStatus(NV3029_SPI, SPI_I2S_TE_FLAG) == RESET);
        SPI_I2S_TransmitData(NV3029_SPI, buff[i]);
    }

    while (SPI_I2S_GetStatus(NV3029_SPI, SPI_I2S_BUSY_FLAG) == SET);
    SPI_Enable(NV3029_SPI, DISABLE);

}

// Transmit buffer using hardware SPI (SPI1 configured for display)
static void SPI1_tx16(const uint16_t* buff, uint32_t len)
{
    
    SPI_Enable(NV3029_SPI, ENABLE);
    for (uint32_t i = 0; i < len; ++i) {
        peek_value[0] = buff[i];
        peek_len = len;
        
        while (SPI_I2S_GetStatus(NV3029_SPI, SPI_I2S_TE_FLAG) == RESET);
        SPI_I2S_TransmitData(NV3029_SPI, buff[i]);
    }

    while (SPI_I2S_GetStatus(NV3029_SPI, SPI_I2S_BUSY_FLAG) == SET);
    SPI_Enable(NV3029_SPI, DISABLE);

}

/**
 * DMA-based SPI1 transmit with mode support
 * mode: 0 = 8-bit standard, 1 = 16-bit (pixel data), 3 = 8-bit with async/interrupt
 */
bool SPI1_tx_dma_mode(const uint8_t* buff, uint16_t len, uint32_t timeout_ms, uint8_t mode)
{
    if (buff == NULL || len == 0)
        return false;

    /* Configure DMA channel 3 for SPI1_TX */
    DMA_RequestRemap(DMA_REMAP_SPI1_TX, DMA, DMA_CH3, ENABLE);

    /* Clear any lingering flags for channel 3 (SPI1_TX) */
    dma_clear_flags(DMA_FLAG_TC3 | DMA_FLAG_HT3 | DMA_FLAG_TE3, DMA);

    
    /* Configure DMA data size based on mode */
    uint32_t mem_size   = DMA_MemoryDataSize_Byte;      /* default 8-bit */
    uint32_t peri_size  = DMA_PERIPH_DATA_SIZE_BYTE;    /* default 8-bit */
    if (mode == 1) {
        mem_size  = DMA_MemoryDataSize_HalfWord;        /* 16-bit for pixel data */
        peri_size = DMA_PERIPH_DATA_SIZE_HALFWORD;      /* SPI DAT in 16-bit frame */
    }
/* Enable SPI peripheral */
    SPI_Enable(NV3029_SPI, ENABLE);
    
    /* Enable SPI DMA request */
    SPI_I2S_EnableDma(NV3029_SPI, SPI_I2S_DMA_TX, ENABLE);

    /* Start DMA transfer */
    dma_start_transfer(DMA_CH3,
                       (uint32_t)&(NV3029_SPI->DAT),
                       (uint32_t)buff,
                       len,
                       DMA_DIR_PERIPH_DST,
                       DMA_PERIPH_INC_DISABLE,
                       DMA_MEM_INC_ENABLE,
                       peri_size,
                       mem_size,
                       DMA_MODE_NORMAL,
                       DMA_PRIORITY_HIGH,
                       DMA_M2M_DISABLE);

                       

    /* Wait for DMA transfer complete */
    bool ok = dma_wait_complete(DMA_CH3, timeout_ms);

    /* Wait for SPI to finish transmitting the last byte */
    uint32_t wait_count = 10000;
    while (SPI_I2S_GetStatus(NV3029_SPI, SPI_I2S_BUSY_FLAG) == SET && wait_count--)
        ;

    return ok;
}

/* Backward compatibility wrapper for mode 0 (8-bit standard) */
bool SPI1_tx_dma(const uint8_t* buff, uint16_t len, uint32_t timeout_ms)
{
    return SPI1_tx_dma_mode(buff, len, timeout_ms, 0);
}

/* Mode 1: 16-bit transfers (pixel data) */
bool SPI1_tx_dma_16bit(const uint16_t* buff, uint16_t count, uint32_t timeout_ms)
{
    return SPI1_tx_dma_mode((const uint8_t*)buff, count, timeout_ms, 1);
}

/* Mode 3: 8-bit with interrupt/callback support (future async enhancement) */
bool SPI1_tx_dma_async(const uint8_t* buff, uint16_t len, uint32_t timeout_ms)
{
    return SPI1_tx_dma_mode(buff, len, timeout_ms, 3);
}

/**
 * DMA-based SPI1 fill operation with fixed-address read
 * Repeatedly reads from single memory location to fill area
 */
bool SPI1_tx_dma_fill(const uint16_t* pattern, uint32_t total_count, uint32_t timeout_ms)
{
    if (pattern == NULL || total_count == 0)
        return false;

    /* Configure DMA channel 3 for SPI1_TX */
    DMA_RequestRemap(DMA_REMAP_SPI1_TX, DMA, DMA_CH3, ENABLE);

    /* Clear any lingering flags for channel 3 (SPI1_TX) */
    dma_clear_flags(DMA_FLAG_TC3 | DMA_FLAG_HT3 | DMA_FLAG_TE3, DMA);

    /* SPI should already be enabled and configured by caller */
    /* Enable SPI DMA request */
    SPI_I2S_EnableDma(NV3029_SPI, SPI_I2S_DMA_TX, ENABLE);

    /* Start DMA transfer with MEM_INC_DISABLE to read same address repeatedly */
    dma_start_transfer(DMA_CH3,
                       (uint32_t)&(NV3029_SPI->DAT),
                       (uint32_t)pattern,
                       total_count,
                       DMA_DIR_PERIPH_DST,
                       DMA_PERIPH_INC_DISABLE,
                       DMA_MEM_INC_DISABLE,    /* Read from same address repeatedly */
                       DMA_PERIPH_DATA_SIZE_HALFWORD,
                       DMA_MemoryDataSize_HalfWord,
                       DMA_MODE_NORMAL,
                       DMA_PRIORITY_HIGH,
                       DMA_M2M_DISABLE);

    /* Wait for DMA transfer complete */
    bool ok = dma_wait_complete(DMA_CH3, timeout_ms);

    /* Wait for SPI to finish transmitting the last byte */
    uint32_t wait_count = 10000;
    while (SPI_I2S_GetStatus(NV3029_SPI, SPI_I2S_BUSY_FLAG) == SET && wait_count--)
        ;

    return ok;
}


/**
 * Cleaner DMA-based LCD I/O helpers with explicit CS/DC control
 */

void LCD_SendCommand_DMA(uint8_t cmd)
{
    LCD_dc_cmd();
    SPI1_tx_dma(&cmd, 1, 100);
}

void LCD_SendData_DMA(const uint8_t* data, uint16_t len)
{
    LCD_dc_data();
    SPI1_tx_dma(data, len, 100);
}

void LCD_SendPixels_DMA(const uint16_t* pixels, uint16_t count)
{
    LCD_dc_data();
    SPI1_tx_dma_16bit(pixels, count, 100);  /* Mode 1: efficient 16-bit transfers */
}

void LCD_SetWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {

    
    uint8_t data[4];

    // column addr
    spi1_config(0); // 8-bit mode
    LCD_SendCommand_DMA(LCD_CMD_CASET);
    data[0] = 0x00; data[1] = x0; data[2] = 0x00; data[3] = x1;
    LCD_SendData_DMA(data, 4);

    // row addr
    LCD_SendCommand_DMA(LCD_CMD_RASET);
    data[0] = 0x00; data[1] = y0; data[2] = 0x00; data[3] = y1; 
    LCD_SendData_DMA(data, 4);

    //write to RAM
    LCD_SendCommand_DMA(LCD_CMD_RAMWR);
   
}

void LCD_DrawPixel(uint8_t x, uint8_t y, uint16_t color) {
    LCD_cs_low();
    LCD_SetWindow(x, y, x, y);
    spi1_config(true); // Switch to 16-bit mode for pixel data
    LCD_SendPixels_DMA(&color, 1);
    LCD_cs_high();
}

void LCD_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
    uint32_t total_pixels = w * h;
    LCD_cs_low();
    LCD_SetWindow(x, y, x + w - 1, y + h - 1);
    
    LCD_dc_data();
    
    // Switch to 16-bit mode for efficient pixel data transfer
    spi1_config(true);
    
    // Use DMA fill (reads same color repeatedly)
    // DMA counter is 16-bit max, so chunk if needed
    if (total_pixels <= 0xFFFF) {
        SPI1_tx_dma_fill(&color, total_pixels, 1000);
    }
    else {
        uint16_t full_chunks = total_pixels / 0xFFFF;
        uint16_t remainder = total_pixels % 0xFFFF;
        
        for (uint16_t i = 0; i < full_chunks; i++) {
            SPI1_tx_dma_fill(&color, 0xFFFF, 1000);
        }
        
        if (remainder > 0) {
            SPI1_tx_dma_fill(&color, remainder, 1000);
        }
    }
    
    LCD_cs_high();
}

/**
 * @brief Iterates through an LcdPacket array and sends it to the display
 * @param packet_array Pointer to the LcdPacket structure array
 * @param length Number of elements in the array
 */
void LCD_Send_Sequence(const LcdPacket* packet_array, uint32_t length) {

    for (uint32_t i = 0; i < length; i++) {
        // 1. Set the DC pin based on the packet type
        if (packet_array[i].type == CMD_LOW) {
            LCD_SendCommand_DMA(packet_array[i].value);
        } else if(packet_array[i].type == DATA_HIGH){
            LCD_SendData_DMA(&packet_array[i].value, 1);
        } else if(packet_array[i].type == DELAY_TIME){
            Delay(packet_array[i].value);
        }
    }
}

void screen_Init(uint8_t screen_type) {
    switch (screen_type) {
        case 0:
            LCD_Send_Sequence(screen_init_seq_0, sizeof(screen_init_seq_0) / sizeof(LcdPacket));
            break;
            
        case 1:
            LCD_Send_Sequence(screen_init_seq_1, sizeof(screen_init_seq_1) / sizeof(LcdPacket));
            break;

         case 2:
            LCD_Send_Sequence(screen_init_seq_1_alt, sizeof(screen_init_seq_1_alt) / sizeof(LcdPacket));
            break;
            
        case 3:
            LCD_Send_Sequence(screen_init_seq_2, sizeof(screen_init_seq_2) / sizeof(LcdPacket));
            break;
            
        default:
            // Error handling or HardFault
            break;
    }
}

void spi1_pin_init(void)
{

    // Enable GPIO clocks for used ports (explicit, do not depend on sFLASH macros)
    RCC_EnableAPB2PeriphClk(NV3029_SPI_CLK | RCC_APB2_PERIPH_AFIO | RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB, ENABLE);
   
    // init CS
    {GPIO_InitType GPIO_InitStructure;
    GPIO_InitStructure.Pin = NV3029_CS_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(NV3029_CS_GPIO_PORT, &GPIO_InitStructure);
    }
        // init DC, RST pins as outputs
        {GPIO_InitType GPIO_InitStructure;
    GPIO_InitStructure.Pin = NV3029_DC_PIN | NV3029_RST_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(NV3029_DC_GPIO_PORT, &GPIO_InitStructure);
        }
    
    {GPIO_InitType(GPIO_InitStructure);
    // Configure SCLK (PB3) and MOSI (PB5) as SPI1 alternate-function outputs
    GPIO_InitStructure.Pin = NV3029_SCLK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = NV3029_SPI_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(NV3029_SCLK_GPIO_PORT, &GPIO_InitStructure);}
    
    //INIT MOSI
    {GPIO_InitType(GPIO_InitStructure);
    GPIO_InitStructure.Pin = NV3029_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = NV3029_SPI_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(NV3029_MOSI_GPIO_PORT, &GPIO_InitStructure);
    }
}

// Drop-in helper: configure SPI1 for display, 8- or 16-bit frames
void spi1_config(bool sixteen_bit)
{
    // Ensure PA15 (CS) is output
    spi1_pin_init(); 


    SPI_Enable(NV3029_SPI, DISABLE);

    SPI_InitType spi;
    SPI_InitStruct(&spi);
    spi.DataDirection  = SPI_DIR_DOUBLELINE_FULLDUPLEX;
    spi.SpiMode        = SPI_MODE_MASTER;
    spi.DataLen        = sixteen_bit ? SPI_DATA_SIZE_16BITS : SPI_DATA_SIZE_8BITS;
    spi.CLKPOL         = SPI_CLKPOL_HIGH;          // Mode 3
    spi.CLKPHA         = SPI_CLKPHA_SECOND_EDGE;   // Mode 3
    spi.NSS            = SPI_NSS_SOFT;
    spi.BaudRatePres   = SPI_BR_PRESCALER_4;       // ~0x200 in CR1
    spi.FirstBit       = SPI_FB_MSB;
    spi.CRCPoly        = 7;

    SPI_Init(NV3029_SPI, &spi);
    SPI_Enable(NV3029_SPI, ENABLE);
}




void spi1_init_bitbang(void) {
    // Disable hardware SPI
    SPI_Enable(NV3029_SPI, DISABLE);
    
    // Configure SCLK (PB3) as output
    {
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = NV3029_SCLK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitPeripheral(NV3029_SCLK_GPIO_PORT, &GPIO_InitStructure);
    }

    // Configure MOSI (PB5) as output initially
    {
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = NV3029_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitPeripheral(NV3029_MOSI_GPIO_PORT, &GPIO_InitStructure);
    }
    
}





/**
 * @brief One-wire SPI protocol: send byte, then receive 3 bytes using bit-banging
 * Manually toggles SCLK and reads/writes MOSI pin for deterministic timing
 */

static void spi1_onewire_bitbang(uint8_t tx_byte, uint8_t low_or_high) {

    uint8_t byte_idx, bit_idx;
    uint8_t bit_val;
    spi1_init_bitbang();

    
    LCD_cs_high();
    LCD_mosi_high();
    LCD_sclk_high();
    LCD_dc_data();
    LCD_cs_low();

    if (low_or_high)
        LCD_dc_data();
    else
        LCD_dc_cmd();


    // Send 0x04 MSB first (0000 0100)
    for (bit_idx = 0; bit_idx < 8; ++bit_idx) {
        // Extract bit from MSB to LSB
        bit_val = (tx_byte >> (7 - bit_idx)) & 0x01;
        
        // Set MOSI to bit value
        if (bit_val) {
            GPIO_SetBits(NV3029_MOSI_GPIO_PORT, NV3029_MOSI_PIN);
        } else {
            GPIO_ResetBits(NV3029_MOSI_GPIO_PORT, NV3029_MOSI_PIN);
        }
        
        // Clock pulse: toggle SCLK
        
        LCD_sclk_low();
        LCD_sclk_high();

    }
    
    // ========== RECEIVE: Get 3 bytes ==========
    // Set MOSI to input mode (read-only)
    {GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = NV3029_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(NV3029_MOSI_GPIO_PORT, &GPIO_InitStructure);
    }
    LCD_sclk_low();
    LCD_sclk_high();
    
     
    // Receive 4 bytes
    for (byte_idx = 0; byte_idx < 4; ++byte_idx) {
        spi_rx_data[byte_idx] = 0x00;
        
        // Receive 8 bits MSB first
        for (bit_idx = 0; bit_idx < 8; ++bit_idx) {
            // Clock pulse: SCLK high (sample on rising edge)
           
            GPIO_ResetBits(NV3029_SCLK_GPIO_PORT, NV3029_SCLK_PIN);
            
            // Read bit from MOSI
             bit_val = (GPIO_ReadInputDataBit(NV3029_MOSI_GPIO_PORT, NV3029_MOSI_PIN) != 0) ? 1 : 0;
            
            // Shift bit into byte (MSB first)
            spi_rx_data[byte_idx] = (spi_rx_data[byte_idx] << 1) | bit_val;
            
            // Clock pulse: SCLK low
            GPIO_SetBits(NV3029_SCLK_GPIO_PORT, NV3029_SCLK_PIN);
        }
    }
}








void LCD_init(void)
{
    
    
    LCD_rst_high();
    Delay(50);
    LCD_rst_low();
    Delay(50);
    LCD_rst_high();
    Delay(200);
    LCD_cs_low();
    LCD_dc_cmd();
    volatile uint8_t cmd_byte = 0x4;
    volatile uint8_t cmd_or_data = 0;
    spi1_onewire_bitbang(cmd_byte, cmd_or_data);
    uint8_t screen_type = 0xff;
    if(spi_rx_data[0] == 0x33 && spi_rx_data[1] == 0x30 && spi_rx_data[2] == 0x25) {
        spi1_onewire_bitbang(0xd3, 0x00);
        if(spi_rx_data[0] == 0x33 && spi_rx_data[1] == 0x30 && spi_rx_data[2] == 0x25) {
            screen_type = 2;
        }
        else {
            screen_type = 0; 
        }
    }
    else {
        screen_type = 2; 
    }


    cmd_byte = 0xd3;
    spi1_onewire_bitbang(cmd_byte, cmd_or_data);
    volatile uint8_t debug_mode = 0;
     
     while (debug_mode)
     {
     Delay(100);
     spi1_onewire_bitbang(cmd_byte, cmd_or_data);
     
     }

    /*
    
    command and feedback decimal


    0x09 -> 0,1,0,0              // read status
    0x04 -> 51,48,37,128         // read id          : manufacturer id, driver id, version id, ?
    0x00 -> 0,0,0,0              // nop
    0xd3 -> 192,0,0,0            //
    0xda -> 102,0,0,0            // read id1         : manufacturer id
    0xdb -> 96,0,0,0             // read id2         : version id
    0xdc -> 74,0,0,0             // read id3         : driver id
    0x0a -> 16,0,0,0             // read power mode  : default value
    //slp out cmd sent 
    0x11 -> 192,0,0,0             // sleep out cmd
    0x0a -> 48,0,0,0              // read power mode  : noron,slpout, bston
    0x0b -> 0,0,0,0               // read display mode
    0x09 -> 128,3,0,0             // read status      : mx--,noron, vertical scroll on
    0x0c -> 0,0,0,0               // read colmod
    0x04 -> 51,48,37,128          // read id          : manufacturer id, driver id, version id, ?
    0X29 -> 128,0,0,0             // display on cmd

    0x01 -> 192,0,0,0,             // software reset cmd

    */


    /*


    ghidra xd011_f
      0x4->
      byte 0 : 0x33 '3' d51
      byte 1 : 0x30 '0' d48
      byte 2 : 0x25 '%' d37
      -> 0


      ghidra xd011_e
      0x4->
      byte 0 : 0x33 '3' d51
      byte 1 : 0x30 '0' d48
      byte 2 : 0x25 '%' d37
      -> 0

      0xd3->
      byte 0 : 0x33 '3' d51
      byte 1 : 0x30 '0' d48
      byte 2 : 0x25 '%' d37
      -> 1
      
      byte 0 : 0x7c '|'    d124
      byte 1 : 0x89 -0x77  d137
      byte 2 : 0xf0 -0x10  d240
      -> 2


*/



    /*
    miami mint Firmware output:
    Successfully recieved bytes:
    
    0x4->
      byte 0 : 0x33 '3' d51
      byte 1 : 0x30 '0' d48
      byte 2 : 0x25 '%' d37
      -> 0
    

    */


    // One-wire SPI protocol: send 0xD3 (with DC low) and receive 3 bytes


    spi1_config(false); // 8-bit mode
    LCD_rst_low();
    Delay(150);
    LCD_rst_high();
    Delay(200);
    LCD_SendCommand_DMA(LCD_CMD_SLPOUT);
    Delay(200);


   
     //Send the initialization sequence
     //screen_type = 1; // Set the desired screen type here (0, 1, 2, or 3)
    screen_Init(screen_type); 

     
    LCD_SendCommand_DMA(LCD_CMD_MADCTL); // 0x36
    if(screen_type == 2) {
        uint8_t madctl = 0x80; // MADCTL for screen type 
        LCD_SendData_DMA(&madctl, 1);
    } else if(screen_type == 1) {
        uint8_t madctl = 0x80; // MADCTL for screen type 
        LCD_SendData_DMA(&madctl, 1);
    } else if(screen_type == 2) {
        uint8_t madctl = 0x80; // MADCTL for screen type 
        LCD_SendData_DMA(&madctl, 1);
    } else if(screen_type == 3) {
        uint8_t madctl = 0xC0; // MADCTL for screen type 
        LCD_SendData_DMA(&madctl, 1);
    }

    LCD_SendCommand_DMA(LCD_CMD_DISPON); // 0x29
    LCD_SendCommand_DMA(LCD_CMD_RAMWR); // 0x2C
    LCD_cs_high();
    LCD_fill_screen(COLOR_BLACK); // yellow




    
}


void LCD_fill_screen(uint16_t color)
{
    uint32_t total_pixels = NV3029_WIDTH * NV3029_HEIGHT;
    
    LCD_cs_low();
    LCD_SetWindow(0, 0, NV3029_WIDTH - 1, NV3029_HEIGHT - 1);
    LCD_dc_data();
    
    // Switch to 16-bit mode for efficient pixel data transfer
    spi1_config(true);
    
    // Use DMA fill (reads same color repeatedly)
    // DMA counter is 16-bit max, so chunk if needed
    if (total_pixels <= 0xFFFF) {
        SPI1_tx_dma_fill(&color, total_pixels, 1000);
    }
    else {
        uint16_t full_chunks = total_pixels / 0xFFFF;
        uint16_t remainder = total_pixels % 0xFFFF;
        
        for (uint16_t i = 0; i < full_chunks; i++) {
            SPI1_tx_dma_fill(&color, 0xFFFF, 1000);
        }
        
        if (remainder > 0) {
            SPI1_tx_dma_fill(&color, remainder, 1000);
        }
    }
    
    LCD_cs_high();
}

// Draw a single 5x8 character with configurable scaling (x,y in pixels)

void LCD_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bgcolor, uint8_t scale)
{
    if (c < 32 || c > 127) c = '?';
    if (scale == 0) scale = 1;  // Minimum scale of 1
    
     uint8_t index = c - 32;
    
    // Clamp index to valid range
    if (index >= 96) index = 0;
    
    const uint8_t* glyph = font5x8[index];

    uint16_t char_width = 6 * scale;  // 5 cols + 1 spacing
    uint16_t char_height = 8 * scale;
    
    // Set window for entire character (CS controlled by caller)
    LCD_SetWindow(x, y, x + char_width - 1, y + char_height - 1);
    spi1_config(true);
    
    LCD_dc_data();
    
    // Draw character row by row (top to bottom), then column by column within row
    // Y outer, X inner to match display frame increment order
    for (uint16_t py = 0; py < char_height; py++) {
        uint8_t row = py / scale;  // Which font row (0-7)
        
        for (uint16_t px = 0; px < char_width; px++) {
            uint8_t col = px / scale;  // Which font column (0-5)
            uint16_t pixel_color;
            
            if (col >= 5) {
                // Spacing column
                pixel_color = bgcolor;
            } else {
                // Character pixel
                uint8_t line = glyph[col];
                pixel_color = (line & (1 << row)) ? color : bgcolor;
            }
            
            SPI1_tx_dma_fill(&pixel_color, 1, 100);
        }
    }
}


void LCD_draw_string(uint16_t x, uint16_t y, const char* s, uint16_t color, uint16_t bgcolor, uint8_t scale)
{
    if (scale == 0) scale = 1;
    if (s == NULL) return;

    LCD_cs_low();  // Keep CS low for entire string
    uint16_t cx = x;
    // Use indexed loop to avoid pointer side effects
    for (size_t i = 0; s[i] != '\0'; ++i) {
        LCD_draw_char(cx, y, s[i], color, bgcolor, scale);
        cx += (5 + 1) * scale;  // 5px char + 1px spacing, scaled
    }
    LCD_cs_high();  // Release CS after string complete
}

/**
 * Diagnostic: Draw a simple grid pattern to understand pixel mapping
 */
void LCD_test_grid(void)
{
    // Fill with black background
    //LCD_fill_screen(COLOR_BLACK);
    Delay(500);
    
    // Draw vertical lines at X positions (every 16 pixels)
    for (uint16_t x = 0; x < NV3029_WIDTH; x += 16) {
        LCD_cs_low();
        LCD_SetWindow(x, 0, x, NV3029_HEIGHT - 1);
        spi1_config(true);
        LCD_dc_data();
        uint16_t white = COLOR_WHITE;
        for (uint16_t y = 0; y < NV3029_HEIGHT; y++) {
            SPI1_tx_dma_fill(&white, 1, 100);
        }
        LCD_cs_high();
    }
    Delay(500);
    
    // Draw horizontal lines at Y positions (every 16 pixels)
    for (uint16_t y = 0; y < NV3029_HEIGHT; y += 16) {
        LCD_cs_low();
        LCD_SetWindow(0, y, NV3029_WIDTH - 1, y);
        spi1_config(true);
        LCD_dc_data();
        uint16_t red = COLOR_RED;
        for (uint16_t x = 0; x < NV3029_WIDTH; x++) {
            SPI1_tx_dma_fill(&red, 1, 100);
        }
        LCD_cs_high();
    }
    Delay(500);
    
    // Draw a 10x10 white square in corner
    LCD_cs_low();
    LCD_SetWindow(10, 10, 19, 19);
    spi1_config(true);
    LCD_dc_data();
    uint16_t white = COLOR_WHITE;
    for (uint16_t i = 0; i < 100; i++) {
        SPI1_tx_dma_fill(&white, 1, 100);
    }
    LCD_cs_high();
}

/**
 * Diagnostic: Test single character rendering
 */
void LCD_test_char(void)
{
    //LCD_fill_screen(COLOR_BLACK);
    //Delay(500);
    LCD_cs_low();
    // Draw a single large "A" in top-left
    
    for (uint8_t index = 32; index <= 127; index++) {
    LCD_draw_char(0, 0, index, COLOR_WHITE, COLOR_BLACK, 4);        
    Delay(100);
    }
    LCD_cs_high();
    //LCD_draw_char(0, 0, 'A', COLOR_WHITE, COLOR_BLACK, 8);
}

// Diagnostic routine: reset display, then fill with a series of colors
void LCD_diag(void)
{
    // Hardware reset sequence

 

    //LCD_init();
    // A few colors to verify SPI and LCD functioning
    LCD_fill_screen(COLOR_RED); // red
    Delay(100);
    LCD_fill_screen(COLOR_GREEN); // green
    Delay(100);
    LCD_fill_screen(COLOR_BLUE); // blue
    Delay(100);
    LCD_fill_screen(COLOR_WHITE); // white
    //Delay(1000);
    //LCD_fill_screen(COLOR_BLUE); // black
    Delay(100);
    LCD_FillRect(0, 10, 10, 100, COLOR_RED);
	Delay(100);
    LCD_fill_screen(COLOR_BLACK); // black
    Delay(100);
    LCD_test_grid();
	Delay(100);
    LCD_fill_screen(COLOR_BLUE); // black
    Delay(100);
	LCD_test_char();
    Delay(100);
    LCD_fill_screen(COLOR_WHITE); // white
    Delay(100);
	LCD_draw_string(0, 0, "Hello world", COLOR_WHITE, COLOR_MAGENTA, 1);
	LCD_draw_string(0, 20, "Hello world", COLOR_RED, COLOR_WHITE, 2);
	LCD_draw_string(0, 40, "Hello world", COLOR_BLACK, COLOR_NAVY, 1);
	LCD_draw_string(0, 60, "Hello world", COLOR_NAVY, COLOR_GREEN, 2);
    Delay(1000);
    LCD_fill_screen(COLOR_BLACK); // black

}
