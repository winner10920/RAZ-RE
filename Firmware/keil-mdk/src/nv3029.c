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
#include <stdlib.h>
#include <string.h>

volatile uint16_t peek_value[] = {0,0,0,0,0,0,0,0,0,0,0};
volatile uint32_t peek_len = 0;
volatile uint8_t spi_rx_data[3] = {0, 0, 0};
volatile uint8_t spi_rx_data_d3[3] = {0, 0, 0};


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


static void LCD_WriteCommand(uint8_t cmd)
{
    //LCD_cs_low();
    LCD_dc_cmd();
    SPI1_tx(&cmd, 1);
    //LCD_cs_high();

}

static void LCD_WriteData(const uint8_t* buff, uint32_t len)
{
    //LCD_cs_low();
    LCD_dc_data();
    SPI1_tx(buff, len);
    //LCD_cs_high();
}

static void LCD_WriteData16(const uint16_t* buff, uint32_t len)
{
    //LCD_cs_low();
    LCD_dc_data();

    SPI1_tx16(buff, len);
    //LCD_cs_high();
}

static void LCD_SetWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {

    // todo: change to 16 bit spi mode here
    //spi1_init_8();
    uint16_t data[2];

    // column addr
    spi1_init_8();
    LCD_WriteCommand(LCD_CMD_CASET);
    spi1_init_16();
    data[0] = x0; data[1] = x1;
    LCD_WriteData16(data, 2);
    //Delay(10);

    // row addr
    spi1_init_8();
    LCD_WriteCommand(LCD_CMD_RASET);
    spi1_init_16();
    data[0] = y0; data[1] = y1; 
    LCD_WriteData16(data, 2);
    //Delay(10);

    //write to RAM
    spi1_init_8();
    LCD_WriteCommand(LCD_CMD_RAMWR);
    spi1_init_16();
    //Delay(10);
    //spi1_init_16();
}

void LCD_DrawPixel(uint8_t x, uint8_t y, uint16_t color) {
    LCD_SetWindow(x, y, x, y);
    LCD_WriteData16(&color, 1);
}

void LCD_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
    LCD_SetWindow(x, y, x + w - 1, y + h - 1);
    
    uint32_t pixels = w * h;

    
    for (uint32_t i = 0; i < pixels; i++) {
        SPI1_tx16((uint16_t *)&color, 1);

    }
    
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
            LCD_WriteCommand(packet_array[i].value);
        } else if(packet_array[i].type == DATA_HIGH){
            LCD_WriteData(&packet_array[i].value, 1);
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

#define SPI_MODE 3

void spi1_init_8(void) {
    SPI_Enable(NV3029_SPI, DISABLE);
    {
        SPI_InitType SPI_InitStructure;
        SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
        SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;
        SPI_InitStructure.DataLen       = SPI_DATA_SIZE_8BITS;
        #if SPI_MODE == 3
        SPI_InitStructure.CLKPOL        = SPI_CLKPOL_HIGH;
        SPI_InitStructure.CLKPHA        = SPI_CLKPHA_SECOND_EDGE;
        #else
        SPI_InitStructure.CLKPOL        = SPI_CLKPOL_LOW;
        SPI_InitStructure.CLKPHA        = SPI_CLKPHA_FIRST_EDGE;
        #endif
        SPI_InitStructure.NSS           = SPI_NSS_SOFT;
        SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_4;
        SPI_InitStructure.FirstBit      = SPI_FB_MSB;
        SPI_InitStructure.CRCPoly       = 7;
        SPI_Init(NV3029_SPI, &SPI_InitStructure);
        SPI_Enable(NV3029_SPI, ENABLE);

    }
}

void spi1_init_16(void) {
    SPI_Enable(NV3029_SPI, DISABLE);
    {
        SPI_InitType SPI_InitStructure;
        SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
        SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;
        SPI_InitStructure.DataLen       = SPI_DATA_SIZE_16BITS;
        #if SPI_MODE == 3
        SPI_InitStructure.CLKPOL        = SPI_CLKPOL_HIGH;
        SPI_InitStructure.CLKPHA        = SPI_CLKPHA_SECOND_EDGE;
        #else
        SPI_InitStructure.CLKPOL        = SPI_CLKPOL_LOW;
        SPI_InitStructure.CLKPHA        = SPI_CLKPHA_FIRST_EDGE;
        #endif
        SPI_InitStructure.NSS           = SPI_NSS_SOFT;
        SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_4;
        SPI_InitStructure.FirstBit      = SPI_FB_MSB;
        SPI_InitStructure.CRCPoly       = 7;
        SPI_Init(NV3029_SPI, &SPI_InitStructure);
        SPI_Enable(NV3029_SPI, ENABLE);
    }
}

/**
 * @brief One-wire SPI protocol: send 0x04, then receive 3 bytes using bit-banging
 * Manually toggles SCLK and reads/writes MOSI pin for deterministic timing
 */
static void spi1_onewire_tx_rx(void) {
    uint8_t tx_byte = 0x04;
    uint8_t byte_idx, bit_idx;
    uint8_t bit_val;
    
    // ========== TRANSMIT: Send 0x04 ==========
    // Set MOSI to output mode
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = NV3029_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitPeripheral(NV3029_MOSI_GPIO_PORT, &GPIO_InitStructure);
    
    LCD_cs_high();
    GPIO_SetBits(NV3029_MOSI_GPIO_PORT, NV3029_MOSI_PIN);
    LCD_mosi_high();
    LCD_dc_data();
    LCD_cs_low();
    LCD_dc_cmd();
    LCD_mosi_low();
    GPIO_ResetBits(NV3029_SCLK_GPIO_PORT, NV3029_SCLK_PIN);

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
        GPIO_SetBits(NV3029_SCLK_GPIO_PORT, NV3029_SCLK_PIN);
        Delay(1);
        GPIO_ResetBits(NV3029_SCLK_GPIO_PORT, NV3029_SCLK_PIN);

    }
    
    // ========== RECEIVE: Get 3 bytes ==========
    // Set MOSI to input mode (read-only)
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = NV3029_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(NV3029_MOSI_GPIO_PORT, &GPIO_InitStructure);
    
    // Receive 3 bytes
    for (byte_idx = 0; byte_idx < 3; ++byte_idx) {
        spi_rx_data[byte_idx] = 0x00;
        
        // Receive 8 bits MSB first
        for (bit_idx = 0; bit_idx < 8; ++bit_idx) {
            // Clock pulse: SCLK high (sample on rising edge)
            GPIO_SetBits(NV3029_SCLK_GPIO_PORT, NV3029_SCLK_PIN);
            //Delay(100);
            
            // Read bit from MOSI
            bit_val = (GPIO_ReadInputDataBit(NV3029_MOSI_GPIO_PORT, NV3029_MOSI_PIN) != 0) ? 1 : 0;
            
            // Shift bit into byte (MSB first)
            spi_rx_data[byte_idx] = (spi_rx_data[byte_idx] << 1) | bit_val;
            
            // Clock pulse: SCLK low
            GPIO_ResetBits(NV3029_SCLK_GPIO_PORT, NV3029_SCLK_PIN);
            Delay(100);
        }
    }
    
    // Set MOSI back to output mode for normal operation
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = NV3029_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitPeripheral(NV3029_MOSI_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief One-wire SPI protocol: send 0xD3, then receive 3 bytes using bit-banging
 * DC pin is set LOW. Manually toggles SCLK and reads/writes MOSI pin for deterministic timing
 */
static void spi1_onewire_tx_rx_d3(void) {
    uint8_t tx_byte = 0xD3;
    uint8_t byte_idx, bit_idx;
    uint8_t bit_val;
    
    // Set DC pin LOW
    GPIO_ResetBits(NV3029_DC_GPIO_PORT, NV3029_DC_PIN);
    
    // ========== TRANSMIT: Send 0xD3 ==========
    // Set MOSI to output mode
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = NV3029_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitPeripheral(NV3029_MOSI_GPIO_PORT, &GPIO_InitStructure);
    
    // Send 0xD3 MSB first (1101 0011)
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
        GPIO_SetBits(NV3029_SCLK_GPIO_PORT, NV3029_SCLK_PIN);
        GPIO_ResetBits(NV3029_SCLK_GPIO_PORT, NV3029_SCLK_PIN);
    }
    
    // ========== RECEIVE: Get 3 bytes ==========
    // Set MOSI to input mode (read-only)
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = NV3029_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(NV3029_MOSI_GPIO_PORT, &GPIO_InitStructure);
    
    // Receive 3 bytes
    for (byte_idx = 0; byte_idx < 3; ++byte_idx) {
        spi_rx_data_d3[byte_idx] = 0x00;
        
        // Receive 8 bits MSB first
        for (bit_idx = 0; bit_idx < 8; ++bit_idx) {
            // Clock pulse: SCLK high (sample on rising edge)
            GPIO_SetBits(NV3029_SCLK_GPIO_PORT, NV3029_SCLK_PIN);
            
            // Read bit from MOSI
            bit_val = (GPIO_ReadInputDataBit(NV3029_MOSI_GPIO_PORT, NV3029_MOSI_PIN) != 0) ? 1 : 0;
            
            // Shift bit into byte (MSB first)
            spi_rx_data_d3[byte_idx] = (spi_rx_data_d3[byte_idx] << 1) | bit_val;
            
            // Clock pulse: SCLK low
            GPIO_ResetBits(NV3029_SCLK_GPIO_PORT, NV3029_SCLK_PIN);
        }
    }
    
    // Set MOSI back to output mode for normal operation
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = NV3029_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitPeripheral(NV3029_MOSI_GPIO_PORT, &GPIO_InitStructure);
    
    // Set DC pin back HIGH
    GPIO_SetBits(NV3029_DC_GPIO_PORT, NV3029_DC_PIN);
}

void spi1_pin_init(void)
{
 GPIO_InitType GPIO_InitStructure;

    GPIO_InitStruct(&GPIO_InitStructure);
    // Enable GPIO clocks for used ports (explicit, do not depend on sFLASH macros)
    RCC_EnableAPB2PeriphClk(NV3029_SPI_CLK | RCC_APB2_PERIPH_AFIO | RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB, ENABLE);

    // init CS, DC, RST pins as outputs
    GPIO_InitStructure.Pin = NV3029_CS_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitPeripheral(NV3029_CS_GPIO_PORT, &GPIO_InitStructure);
        // init CS, DC, RST pins as outputs
    GPIO_InitStructure.Pin = NV3029_DC_PIN | NV3029_RST_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitPeripheral(NV3029_DC_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStruct(&GPIO_InitStructure);
    // Configure SCLK (PB3) and MOSI (PB5) as SPI1 alternate-function outputs
    GPIO_InitStructure.Pin = NV3029_SCLK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = NV3029_SPI_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitPeripheral(NV3029_SCLK_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = NV3029_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = NV3029_SPI_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_HIGH;
    GPIO_InitPeripheral(NV3029_MOSI_GPIO_PORT, &GPIO_InitStructure);
}

void LCD_init(void)
{
    LCD_rst_low();
    Delay(500);
    LCD_rst_high();
    Delay(500);
    LCD_rst_low();
    Delay(500);
 

    // One-wire SPI protocol: send 0x04 and receive 3 bytes
    spi1_onewire_tx_rx();
    
    // One-wire SPI protocol: send 0xD3 (with DC low) and receive 3 bytes
    spi1_onewire_tx_rx_d3();

    spi1_pin_init();
    
    spi1_init_8();



    // hardware reset
    LCD_cs_high();
    LCD_dc_cmd();
    LCD_rst_high();

    Delay(1000);
    LCD_rst_low();
    Delay(500);
    LCD_rst_high();
    Delay(500);
    LCD_cs_low();
    Delay(100);
    LCD_WriteCommand(LCD_CMD_SWRESET); // 0x01
    Delay(150);
    LCD_WriteCommand(LCD_CMD_SLPOUT);
    Delay(255);

   
     //Send the initialization sequence
     uint8_t screen_type = 2; // Set the desired screen type here (0, 1, 2, or 3)
     screen_Init(screen_type); 
    //LCD_WriteCommand(LCD_CMD_COLMOD); // 0x3A
    //Delay(1);
    LCD_WriteData((uint8_t*)0x55, 1); // 16 bits per pixel
    Delay(10);
    LCD_WriteCommand(LCD_CMD_MADCTL); // 0x36
    Delay(10);
    if(screen_type == 0) {
        uint8_t madctl = 0x80; // MADCTL for screen type 
        LCD_WriteData(&madctl, 1);
    } else if(screen_type == 1) {
        uint8_t madctl = 0x80; // MADCTL for screen type 
        LCD_WriteData(&madctl, 1);
    } else if(screen_type == 2) {
        uint8_t madctl = 0x80; // MADCTL for screen type 
        LCD_WriteData(&madctl, 1);
    } else if(screen_type == 3) {
        uint8_t madctl = 0xC0; // MADCTL for screen type 
        LCD_WriteData(&madctl, 1);
    }
    Delay(5);
    //LCD_WriteCommand(LCD_CMD_NORON); // 0x13
    Delay(10);
    //LCD_WriteCommand(LCD_CMD_IDMOFF); // 0x29
    Delay(10);
    //LCD_WriteCommand(LCD_CMD_INVON); // 0x20
    Delay(10);
    LCD_WriteCommand(LCD_CMD_DISPON); // 0x29
    Delay(1000);


    Delay(11);
    LCD_WriteCommand(LCD_CMD_CASET); // 0x2A
    spi1_init_16();

    uint16_t data[2] = {0x0000, 0x007F};
    LCD_WriteData16(data, 2);
    spi1_init_8();
    LCD_WriteCommand(LCD_CMD_RASET); // 0x2B
    spi1_init_16();
    data[1] = 0x009F;
    LCD_WriteData16(data, 2);
    spi1_init_8();
    LCD_WriteCommand(LCD_CMD_RAMWR);
    spi1_init_16();
    LCD_cs_high();
    Delay(10);
    LCD_cs_low();
    // Send 54ms of 0x00 data
    data[1] = 0x0000;
    for (uint32_t i = 0; i < (128*160); ++i) { 
        LCD_WriteData16(data, 2);
    }
    LCD_cs_high();
    Delay(700);
    LCD_cs_low();
    Delay(1000);




    /*
    Remaining Commands before data from sigrok pulseview implemented above

    CMD:   0xAC
    DELAY: 71uS
    CMD:   0x2A
    DELAY: 15uS
    DATA:  0x00
    DELAY: 15uS
    DATA:  0x00
    DELAY: 45uS
    DATA:  0x00
    DELAY: 15uS
    DATA:  0x7F
    DELAY: 15uS
    CMD:   0x2B
    DELAY: 45uS
    DATA:  0x00
    DELAY: 15uS
    DATA:  0x00
    DELAY: 15uS
    DATA:  0x00
    DELAY: 45uS
    DATA:  0x9F
    DELAY: 15uS
    CMD:   0x2C
    DELAY: 15uS
    DATA:  0x00
    CS:    HIGH
    DELAY: 10uS
    CS:    LOW
    DATA:  54mS OF 0X00
    CS:    HIGH
    DELAY: 700uS
    CS:    LOW
    */

     /*
    Remaining Commands AFTER NULL DATA TRAIN from sigrok pulseview
    CMD:   0x2A
    DELAY: 15uS
    CMD:   0x00
    DELAY: 15uS
    CMD:   0x54
    DELAY: 45uS
    DATA:  0x00
    DELAY: 15uS
    DATA:  0x7B
    DELAY: 15uS
    CMD:   0x2B
    DELAY: 45uS
    DATA:  0x00
    DELAY: 15uS
    DATA:  0x03
    DELAY: 50uS
    DATA:  0x00
    DELAY: 15uS
    DATA:  0x28
    DELAY: 15uS
    CMD:   0x2C
    DELAY: 438uS
    DATA:  2.234mS OF DATA
    CS:    HIGH
    DELAY: 80uS
    CS:    LOW

    */






    LCD_cs_high();
    Delay(100);
    
}


void LCD_fill_screen(uint16_t color)
{
    LCD_cs_low();
    Delay(10);
    // fill entire screen with a single color
    LCD_SetWindow(0, 0, NV3029_WIDTH-1, NV3029_HEIGHT-1);
    // send color data as high byte then low byte repeatedly using hardware SPI
    
    peek_value[0] = color;

    for (int i = 0; i < (NV3029_WIDTH * NV3029_HEIGHT); ++i) {
        LCD_WriteData16(&color, 1);
    }
    Delay(10);
    LCD_cs_high();
}

// Draw a single 5x8 character (x,y in pixels)
void LCD_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bgcolor)
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
            LCD_SetWindow(px, py, px, py);
            
            uint8_t hi = (line & (1<<row)) ? (color >> 8) : (bgcolor >> 8);
            uint8_t lo = (line & (1<<row)) ? (color & 0xFF) : (bgcolor & 0xFF);
            uint16_t tmp = (hi << 8) | lo;
            LCD_WriteData16(&tmp, 1);
        }
    }
    // one column spacing
    for (uint8_t row = 0; row < 8; ++row){
        uint16_t px = x + 5; uint16_t py = y + row;
        LCD_SetWindow(px, py, px, py);
        uint8_t hi = bgcolor >> 8; uint8_t lo = bgcolor & 0xFF;
        uint16_t tmpb = (hi << 8) | lo;
        LCD_WriteData16(&tmpb, 1);
    }
}

void LCD_draw_string(uint16_t x, uint16_t y, const char* s, uint16_t color, uint16_t bgcolor)
{
    uint16_t cx = x;
    while (*s){
        LCD_draw_char(cx, y, *s, color, bgcolor);
        cx += 6; // 5px + 1 spacing
        s++;
    }
}

// Diagnostic routine: reset display, then fill with a series of colors
void LCD_diag(void)
{
    // Hardware reset sequence

    //LCD_dc_data();
    //LCD_rst_low();
    //Delay(200);
    //LCD_rst_high();
    //Delay(200);
   // LCD_cs_high();
    //Delay(10);
    //LCD_cs_low();
    //Delay(10);

    //LCD_init();
    // A few colors to verify SPI and LCD functioning
    LCD_fill_screen(COLOR_BLACK); // red
    Delay(100);
    LCD_fill_screen(COLOR_GREEN); // green
    Delay(100);
    LCD_fill_screen(COLOR_BLUE); // blue
    Delay(100);
    LCD_fill_screen(COLOR_WHITE); // white
    Delay(100);
    LCD_fill_screen(COLOR_BLUE); // black
    Delay(200);

}
