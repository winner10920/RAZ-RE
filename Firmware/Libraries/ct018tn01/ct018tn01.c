/* CT018TN01 driver implementation
 * Implements the sequences and register writes required to initialize and
 * configure the CT018TN01 panel. Uses the same ST7735 pin/SPI macros so it
 * works with the wiring used by the existing ST7735 code.
 */

#include "ct018tn01.h"
#include "n32g031.h"
#include <string.h>

/* Low-level helpers: send command and data using the configured SPI peripheral */
static void ct018_spi_tx(const uint8_t* buff, uint32_t len)
{
    for (uint32_t i = 0; i < len; ++i) {
        while (SPI_I2S_GetStatus(CT018_SPI, SPI_I2S_TE_FLAG) == RESET) ;
        SPI_I2S_TransmitData(CT018_SPI, buff[i]);
        while (SPI_I2S_GetStatus(CT018_SPI, SPI_I2S_RNE_FLAG) == RESET) ;
        (void)SPI_I2S_ReceiveData(CT018_SPI);
    }
}

static void ct018_send_cmd(uint8_t cmd)
{
    GPIO_ResetBits(CT018_DC_GPIO_PORT, CT018_DC_PIN); // DC = 0 for command
    GPIO_ResetBits(CT018_CS_GPIO_PORT, CT018_CS_PIN);
    ct018_spi_tx(&cmd, 1);
    GPIO_SetBits(CT018_CS_GPIO_PORT, CT018_CS_PIN);
}

static void ct018_send_data(const uint8_t* data, uint32_t len)
{
    GPIO_SetBits(CT018_DC_GPIO_PORT, CT018_DC_PIN); // DC = 1 for data
    GPIO_ResetBits(CT018_CS_GPIO_PORT, CT018_CS_PIN);
    ct018_spi_tx(data, len);
    GPIO_SetBits(CT018_CS_GPIO_PORT, CT018_CS_PIN);
}

/* internal state */
static ct018_voltage_t current_vopt = CT018_VOLTAGE_3V3;



/* Voltage option -> register values mapping. The bytes below are controller
 * register payloads chosen to match the panel datasheet recommendations for
 * each supply option. The implementation will write these values into the
 * appropriate controller registers when the user selects a different voltage.
 * Modify these arrays if your panel revision requires tuned numbers.
 */
struct voltage_regs {
    uint8_t vcom_byte; /* single-byte representation used by C5-like command */
};

static const struct voltage_regs vmap[] = {
    /* CT018_VOLTAGE_3V3 */ { 0x40 },
    /* CT018_VOLTAGE_2V8 */ { 0x34 },
    /* CT018_VOLTAGE_5V  */ { 0x50 }
};

void ct018_init(ct018_voltage_t vopt)
{
    current_vopt = vopt;

    /* enable GPIO clocks for ports used by the panel */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB, ENABLE);

    /* Configure CS, DC, RST as outputs */
    GPIO_InitType gpio;
    GPIO_InitStruct(&gpio);
    gpio.Pin = CT018_CS_PIN; gpio.GPIO_Mode = GPIO_MODE_OUTPUT_PP; GPIO_InitPeripheral(CT018_CS_GPIO_PORT, &gpio);
    gpio.Pin = CT018_DC_PIN; gpio.GPIO_Mode = GPIO_MODE_OUTPUT_PP; GPIO_InitPeripheral(CT018_DC_GPIO_PORT, &gpio);
    gpio.Pin = CT018_RST_PIN; gpio.GPIO_Mode = GPIO_MODE_OUTPUT_PP; GPIO_InitPeripheral(CT018_RST_GPIO_PORT, &gpio);

    /* Configure SCLK (PB3) and MOSI (PB5) as SPI alternate-function outputs */
    gpio.Pin = CT018_SCLK_PIN;
    gpio.GPIO_Mode = GPIO_MODE_AF_PP;
    gpio.GPIO_Alternate = CT018_SPI_AF;
    GPIO_InitPeripheral(CT018_SCLK_GPIO_PORT, &gpio);

    gpio.Pin = CT018_MOSI_PIN;
    gpio.GPIO_Mode = GPIO_MODE_AF_PP;
    gpio.GPIO_Alternate = CT018_SPI_AF;
    GPIO_InitPeripheral(CT018_MOSI_GPIO_PORT, &gpio);

    /* Enable SPI clock + AFIO */
    RCC_EnableAPB2PeriphClk(CT018_SPI_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);

    /* Initialize SPI peripheral for display transfers. Use the highest
     * reasonable baudrate prescaler so the panel sees the fastest clock the
     * MCU can provide (user requested highest datasheet-recommended speed).
     * If you encounter stability issues, reduce the prescaler (e.g. to
     * SPI_BR_PRESCALER_4) or add small delays between transfers.
     */
    {
        SPI_InitType SPI_InitStructure;
        SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
        SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;
        SPI_InitStructure.DataLen       = SPI_DATA_SIZE_8BITS;
        SPI_InitStructure.CLKPOL        = SPI_CLKPOL_HIGH;
        SPI_InitStructure.CLKPHA        = SPI_CLKPHA_SECOND_EDGE;
        SPI_InitStructure.NSS           = SPI_NSS_SOFT;
        /* fastest prescaler (2) for highest SCLK; change if unstable */
        SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_2;
        SPI_InitStructure.FirstBit      = SPI_FB_MSB;
        SPI_InitStructure.CRCPoly       = 7;
        SPI_Init(CT018_SPI, &SPI_InitStructure);
        SPI_Enable(CT018_SPI, ENABLE);
    }

    /* ensure CS high by default */
    GPIO_SetBits(CT018_CS_GPIO_PORT, CT018_CS_PIN);

}

void ct018_power_on(void)
{
    /* Reset sequence (hardware reset as recommended by datasheet) */
    GPIO_ResetBits(CT018_RST_GPIO_PORT, CT018_RST_PIN);
    Delay(20);
    GPIO_SetBits(CT018_RST_GPIO_PORT, CT018_RST_PIN);
    Delay(20);

    /* R00 <- 0x0001 */
    {
        uint16_t v = 0x0001;
        uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x00); ct018_send_data(b, 2);
    }







    /* The panel's datasheet (pages 10-16) prescribes a specific register
     * initialization flow. The flow below follows that order verbatim:
     *  - Write R04, R0C, R0D with VCI-dependent values
     *  - Pulse R0A values with short delays and set R0E/R03 as shown
     *  - Wait frame delays where required
     */
    uint16_t r0c_val = 0x0001;
    uint16_t r0d_val_1 = 0x0605;
    uint16_t r0d_val_2 = 0x0615;
    /* choose values based on the selected VCI option (approximate mapping)
     * (these values were taken from the datasheet lookup table example)
     */
    switch (current_vopt) {
        case CT018_VOLTAGE_2V8:
            r0c_val = 0x0000;
            r0d_val_1 = 0x0608;
            r0d_val_2 = 0x0618;
            break;
        case CT018_VOLTAGE_5V:
            r0c_val = 0x0004; /* if your 5V mapping differs, change here */
            r0d_val_1 = 0x0609;
            r0d_val_2 = 0x0619;
            break;
        case CT018_VOLTAGE_3V3:
        default:
            r0c_val = 0x0001;
            r0d_val_1 = 0x0605;
            r0d_val_2 = 0x0615;
            break;
    }

    /* R04 <- 0x0C0C */
    {
        uint16_t v = 0x0C0C;
        uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x04); ct018_send_data(b, 2);
    }
    /* R0C <- r0c_val */
    {
        uint16_t v = r0c_val;
        uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0C); ct018_send_data(b, 2);
    }
    /* R0D <- r0d_val_1 */
    {
        uint16_t v = r0d_val_1;
        uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0D); ct018_send_data(b, 2);
    }

    /* R0A <- 0x0101 */
    {
        uint16_t v = 0x0101; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0A); ct018_send_data(b, 2);
    }
    Delay(1);

    /* R0E <- 0x141A */
    {
        uint16_t v = 0x141A; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0E); ct018_send_data(b, 2);
    }

    /* R0A <- 0x0102 */
    {
        uint16_t v = 0x0102; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0A); ct018_send_data(b, 2);
    }
    Delay(1);

    /* R03 <- 0x2010 */
    {
        uint16_t v = 0x2010; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x03); ct018_send_data(b, 2);
    }

    /* R0A <- 0x0100; delay 40ms */
    {
        uint16_t v = 0x0100; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0A); ct018_send_data(b, 2);
    }
    Delay(40);

    /* R0E <- 0x341A */
    {
        uint16_t v = 0x341A; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0E); ct018_send_data(b, 2);
    }

    /* R0A <- 0x0102; delay 1ms */
    {
        uint16_t v = 0x0102; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0A); ct018_send_data(b, 2);
    }
    Delay(1);

    /* R0A <- 0x0100; delay 40ms */
    {
        uint16_t v = 0x0100; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0A); ct018_send_data(b, 2);
    }
    Delay(40);

    /* R0D <- r0d_val_2 */
    {
        uint16_t v = r0d_val_2; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0D); ct018_send_data(b, 2);
    }

    /* R0A <- 0x0101; delay 1ms */
    {
        uint16_t v = 0x0101; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0A); ct018_send_data(b, 2);
    }
    Delay(1);

    // /* Continue with standard display on flow: SWRESET/SLPOUT, color mode and MADCTL */
    // ct018_send_cmd(0x01); /* SWRESET */
    // Delay(120);
    // ct018_send_cmd(0x11); /* SLPOUT */
    // Delay(120);

    // /* Set color mode to 16-bit by default */
    // ct018_send_cmd(0x3A); /* COLMOD */
    // uint8_t colmod = 0x05; ct018_send_data(&colmod, 1);
    // Delay(10);

    // /* MADCTL default (row/col order) */
    // ct018_send_cmd(0x36); uint8_t mad = 0x00; ct018_send_data(&mad, 1);
    // Delay(10);

    // /* Apply VCOM control according to selected voltage mapping */
    // {
    //     uint8_t c5 = vmap[current_vopt].vcom_byte;
    //     ct018_send_cmd(0xC5); ct018_send_data(&c5, 1);
    // }

    /* Apply default gamma */
    ct018_gamma_set();
    ct018_chip_set();

    /* Display on */
    ct018_display_on();
    Delay(100);
}

void ct018_power_off(void)
{
    ct018_display_off();
    /* R0E <- 0x0000 */
    {
        uint16_t v = 0x0000; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0E); ct018_send_data(b, 2);
    }




    GPIO_ResetBits(CT018_RST_GPIO_PORT, CT018_RST_PIN);
}




void ct018_gamma_set(void)
{
    uint8_t gamma_regs[9] = {
        0x30,
        0x31,
        0x32,
        0x33,
        0x34,
        0x35,
        0x36,
        0x37,
        0x3f
    };

    uint16_t gamma_vals[9] = {
        0x0604,
        0x0407,
        0x0107,
        0x0302,
        0x0006,
        0x0003,
        0x0301,
        0x0203,
        0x0000
    };
    for (int i = 0; i < 9; ++i) {
        ct018_send_cmd(gamma_regs[i]);
        uint8_t b[2] = { (uint8_t)(gamma_vals[i] >> 8), (uint8_t)(gamma_vals[i] & 0xFF) };
        ct018_send_data(b, 2);
    }

}

void ct018_chip_set(void)
{

    uint8_t chip_set_regs[13] = {
        0x01,
        0x02,
        0x05,
        0x06,
        0x07,
        0x0B,
        0x0F,
        0x11,
        0x14,
        0x15,
        0x16,
        0x17,
        0x20
    };

    uint16_t chip_set_vals[13] = {
        0x0113,
        0x0700,
        0x0230,
        0x0000,
        0x0700,
        0x0000,
        0x000A,
        0x0000,
        0x9F00,
        0x8050,
        0x7F00,
        0x9F00,
        0x0000
    };

    for (int i = 0; i < 13; ++i) {
        ct018_send_cmd(chip_set_regs[i]);
        uint8_t b[2] = { (uint8_t)(chip_set_vals[i] >> 8), (uint8_t)(chip_set_vals[i] & 0xFF) };
        ct018_send_data(b, 2);
    }

    /* R0A <- 0x0106 */
    {
        uint16_t v = 0x0106; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0A); ct018_send_data(b, 2);
    }
    Delay(1);
    /* R0A <- 0x0107 */
    {
        uint16_t v = 0x0107; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0A); ct018_send_data(b, 2);
    }
    Delay(1);



}

void ct018_display_on(void)
{
    /* R07 <- 0x0005 */
    {
        uint16_t v = 0x0005; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x07); ct018_send_data(b, 2);
    }
    Delay(1);
    /* R07 <- 0x0025 */
    {
        uint16_t v = 0x0025; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x07); ct018_send_data(b, 2);
    }
    /* R0A <- 0x0100 */
    {
        uint16_t v = 0x0100; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0A); ct018_send_data(b, 2);
    }
    Delay(1);
    /* R07 <- 0x0027 */
    {
        uint16_t v = 0x0027; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x07); ct018_send_data(b, 2);
    }
     /* R07 <- 0x0037 */
    {
        uint16_t v = 0x0037; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x07); ct018_send_data(b, 2);
    }
}

void ct018_display_off(void)
{
    /* R07 <- 0x00036 */
    {
        uint16_t v = 0x0036; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x07); ct018_send_data(b, 2);
    }
    Delay(1);
    /* R07 <- 0x00026 */
    {
        uint16_t v = 0x0026; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x07); ct018_send_data(b, 2);
    }
    /* R07 <- 0x0004 */
    {
        uint16_t v = 0x0004; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x07); ct018_send_data(b, 2);
    }
    /* R0A <- 0x00100 */
    {
        uint16_t v = 0x00100; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0A); ct018_send_data(b, 2);
    }
    Delay(1);
    /* R03 <- 0x0000 */
    {
        uint16_t v = 0x0000; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x03); ct018_send_data(b, 2);
    }
    /* R0A <- 0x00100 */
    {
        uint16_t v = 0x00100; uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)(v & 0xFF) };
        ct018_send_cmd(0x0A); ct018_send_data(b, 2);
    }
    Delay(1);
}


void ct018_standby(void)
{
//
}

void ct018_sleep_in(void)
{
//
}


/* Fill the whole panel with a single RGB565 color. Implemented here so
 * main.c can perform a quick hardware smoke-test without depending on the
 * ST7735 driver. Uses standard CASET/RASET/RAMWR commands (ST77xx style).
 */
void ct018_fill_screen(uint16_t color)
{
    /* set column address (0..CT018_WIDTH-1) */
    uint8_t col[4] = { 0x00, 0x00, (uint8_t)((CT018_WIDTH-1) >> 8), (uint8_t)((CT018_WIDTH-1) & 0xFF) };
    ct018_send_cmd(0x2A); ct018_send_data(col, 4);
    /* set row address (0..CT018_HEIGHT-1) */
    uint8_t row[4] = { 0x00, 0x00, (uint8_t)((CT018_HEIGHT-1) >> 8), (uint8_t)((CT018_HEIGHT-1) & 0xFF) };
    ct018_send_cmd(0x2B); ct018_send_data(row, 4);

    /* write memory */
    ct018_send_cmd(0x2C);
    /* send pixel data: high byte then low byte, repeated */
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    /* send in reasonable chunks to avoid huge stack usage */
    const int chunk_pixels = 256;
    uint8_t chunk[chunk_pixels * 2];
    for (int i = 0; i < chunk_pixels; ++i) { chunk[i*2] = hi; chunk[i*2+1] = lo; }

    uint32_t total = CT018_WIDTH * CT018_HEIGHT;
    while (total > 0) {
        uint32_t send = (total > chunk_pixels) ? chunk_pixels : total;
        ct018_send_data(chunk, send * 2);
        total -= send;
    }
}


