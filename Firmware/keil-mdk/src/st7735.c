/* Minimal ST7735 driver implementation (for 80x160 ST7735 variant)
 * Uses the board's SPI peripheral (shared with sFLASH by default).
 * Pin mapping (adjustable in inc/st7735.h):
 *   CS = PA15, SCLK = PB3 (board), MOSI = PB5 (board) [SPI pins may be shared],
 *   RST = PB6, RS(DC) = PB7
 * Provides basic init, fill screen, and text draw (5x8 font)
 */

#include "st7735.h"
//#include <string.h>

 

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

// void Innolux_CT018TN01::powerSet()
// {
//     uint16_t r0c_val = 0x0000;
//     uint16_t r0d_val_1 = 0x0000;
//     uint16_t r0d_val_2 = 0x0000;

//     [span_14](start_span) // Determine values based on Page 11 Lookup Tables[span_14](end_span)
//         if (SYSTEM_VCI_VOLTAGE == VCI_2_5V)
//     {
//         r0c_val = 0x0004;
//         r0d_val_1 = 0x0609;
//         r0d_val_2 = 0x0619;
//     }
//     else if (SYSTEM_VCI_VOLTAGE == VCI_2_8V)
//     {
//         r0c_val = 0x0000;
//         r0d_val_1 = 0x0608;
//         r0d_val_2 = 0x0618;
//     }
//     else
//     { // VCI_3_3V
//         r0c_val = 0x0001;
//         r0d_val_1 = 0x0605;
//         r0d_val_2 = 0x0615;
//     }

//     [span_15](start_span)            // Sequence Start[span_15](end_span)
//         writeRegister(0x04, 0x0C0C); // R04 << 0C0Ch
//     writeRegister(0x0C, r0c_val);    // R0C << xxxx (Based on VCI)
//     writeRegister(0x0D, r0d_val_1);  // R0D << xxxx (Based on VCI)

//     writeRegister(0x0A, 0x0101); // R0A << 0101h
//     delayMs(1);                  // Delay 1ms

//     writeRegister(0x0E, 0x141A); // R0E << 141Ah

//     writeRegister(0x0A, 0x0102); // R0A << 0102h
//     delayMs(1);                  // Delay 1ms

//     writeRegister(0x03, 0x2010); // R03 << 2010h

//     // Continue Flow (Page 11 Right Column)
//     writeRegister(0x0A, 0x0100); // R0A << 0100h
//     delayMs(40);                 // Delay 40ms (2 Frames or More)

//     writeRegister(0x0E, 0x341A); // R0E << 341Ah

//     writeRegister(0x0A, 0x0102); // R0A << 0102h
//     delayMs(1);                  // Delay 1ms

//     writeRegister(0x0A, 0x0100); // R0A << 0100h
//     delayMs(40);                 // Delay 40ms (2 Frames or More)

//     writeRegister(0x0D, r0d_val_2); // R0D << xxxx (Based on VCI)

//     writeRegister(0x0A, 0x0101); // R0A << 0101h
//     delayMs(1);                  // Delay 1ms
// }
// }

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

// Diagnostic routine: reset display, then fill with a series of colors
void st7735_diag(void)
{
    // Hardware reset sequence
    st7735_rst_low();
    Delay(20000);
    st7735_rst_high();
    Delay(20000);

    // A few colors to verify SPI and LCD functioning
    st7735_fill_screen(ST7735_COLOR565(255,0,0)); // red
    Delay(5000);
    st7735_fill_screen(ST7735_COLOR565(0,255,0)); // green
    Delay(5000);
    st7735_fill_screen(ST7735_COLOR565(0,0,255)); // blue
    Delay(5000);
    st7735_fill_screen(ST7735_COLOR565(255,255,255)); // white
    Delay(5000);
    st7735_fill_screen(ST7735_COLOR565(0,0,0)); // black
    Delay(2000);
}
