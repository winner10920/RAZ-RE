/* dma_example.c - demonstrates using Libraries/n32g031_dma wrapper
 * 
 * Example 1: Simple SPI1 TX with DMA (memory to SPI1 data register)
 * Example 2: Double-buffered SPI1 TX for continuous streaming
 */
#include "dma.h"
#include "n32g031_dma.h"
#include "n32g031_rcc.h"
#include "n32g031_spi.h"
#include "n32g031_gpio.h"
#include "n32g031_spi.h"

/* ===== Example 1: Simple SPI1 TX Transfer ===== */

#define SPI1_DATA_REG   ((uint32_t)&(SPI1->DAT))
#define TX_BUFFER_SIZE  256

static uint8_t tx_buffer[TX_BUFFER_SIZE] = {
    0xAA, 0xBB, 0xCC, 0xDD, /* Initialize with test pattern */
};

static void spi1_dma_complete(void* ctx, uint32_t flags)
{
    (void)ctx;
    if (flags & DMA_INT_TXC)
    {
        /* Transfer complete - can reload or disable SPI */
        SPI_Enable(SPI1, DISABLE);
    }
    else if (flags & DMA_INT_ERR)
    {
        /* Error occurred */
    }
}

void dma_example_spi1_simple(void)
{
    SPI_InitType spi_init;
    GPIO_InitType gpio_init;
    
    /* Enable clocks */
    dma_init();
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_SPI1 | RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_AFIO, ENABLE);
    
    /* Configure SPI1 pins: PA5=SCK, PA6=MISO, PA7=MOSI */
    GPIO_InitStruct(&gpio_init);
    gpio_init.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpio_init.GPIO_Mode = GPIO_MODE_AF_PP;
    gpio_init.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitPeripheral(GPIOA, &gpio_init);
    
    /* Configure SPI1: 8-bit, MSB first, mode 0 */
    SPI_InitStruct(&spi_init);
    spi_init.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
    spi_init.SpiMode = SPI_MODE_MASTER;
    spi_init.DataLen = SPI_DATA_SIZE_8BITS;
    spi_init.CLKPOL = SPI_CLKPOL_LOW;
    spi_init.CLKPHA = SPI_CLKPHA_FIRST_EDGE;
    spi_init.NSS = SPI_NSS_SOFT;
    spi_init.BaudRatePres = SPI_BR_PRESCALER_8;  /* Clock / 8 */
    spi_init.FirstBit = SPI_FB_MSB;
    spi_init.CRCPoly = 7;
    SPI_Init(SPI1, &spi_init);
    SPI_Enable(SPI1, ENABLE);
    
    /* Register callback for DMA completion */
    dma_set_callback(DMA_CH3, spi1_dma_complete, NULL);
    
    /* Start DMA transfer: memory -> SPI1_DR */
    dma_start_transfer(
        DMA_CH3,
        SPI1_DATA_REG,              /* Peripheral address (SPI1 data register) */
        (uint32_t)tx_buffer,        /* Memory address */
        TX_BUFFER_SIZE,             /* Number of bytes */
        DMA_DIR_PERIPH_DST,         /* Direction: memory to peripheral */
        DMA_PERIPH_INC_DISABLE,     /* Don't increment peripheral address */
        DMA_MEM_INC_ENABLE,         /* Increment memory address */
        DMA_PERIPH_DATA_SIZE_BYTE,  /* 8-bit peripheral accesses */
        DMA_MemoryDataSize_Byte,    /* 8-bit memory accesses */
        DMA_MODE_NORMAL,            /* Normal mode (not circular) */
        DMA_PRIORITY_HIGH,          /* High priority */
        DMA_M2M_DISABLE             /* Not memory-to-memory */
    );
}

/* ===== Example 2: Double-Buffered SPI1 TX (Streaming) ===== */

#define DOUBLE_BUF_SIZE  256

static uint8_t dma_buf_a[DOUBLE_BUF_SIZE];
static uint8_t dma_buf_b[DOUBLE_BUF_SIZE];
static dma_double_buffer_t spi1_db;

static void spi1_db_callback(void* ctx, uint32_t flags)
{
    (void)ctx;
    if (flags & DMA_INT_TXC)
    {
        /* Buffer transfer complete - swap active buffer */
        spi1_db.active_buf = 1 - spi1_db.active_buf;
        
        /* Get the inactive buffer and refill it for next transfer */
        uint32_t inactive = dma_double_buffer_get_inactive(&spi1_db);
        uint8_t* buf = (uint8_t*)inactive;
        
        /* Application can refill buf here for continuous streaming */
        for (int i = 0; i < DOUBLE_BUF_SIZE; ++i)
            buf[i] = (i + 1) & 0xFF;
    }
}

void dma_example_spi1_double_buffer(void)
{
    SPI_InitType spi_init;
    GPIO_InitType gpio_init;
    
    /* Enable clocks */
    dma_init();
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_SPI1 | RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_AFIO, ENABLE);
    
    /* Configure SPI1 pins */
    GPIO_InitStruct(&gpio_init);
    gpio_init.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpio_init.GPIO_Mode = GPIO_MODE_AF_PP;
    gpio_init.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitPeripheral(GPIOA, &gpio_init);
    
     /* Configure SPI1: 8-bit, MSB first, mode 0 */
    SPI_InitStruct(&spi_init);
    spi_init.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
    spi_init.SpiMode = SPI_MODE_MASTER;
    spi_init.DataLen = SPI_DATA_SIZE_8BITS;
    spi_init.CLKPOL = SPI_CLKPOL_LOW;
    spi_init.CLKPHA = SPI_CLKPHA_FIRST_EDGE;
    spi_init.NSS = SPI_NSS_SOFT;
    spi_init.BaudRatePres = SPI_BR_PRESCALER_8;  /* Clock / 8 */
    spi_init.FirstBit = SPI_FB_MSB;
    spi_init.CRCPoly = 7;
    SPI_Init(SPI1, &spi_init);
    SPI_Enable(SPI1, ENABLE);
    
    /* Initialize double-buffered transfer */
    dma_double_buffer_init(&spi1_db,
                          DMA_CH3,
                          SPI1_DATA_REG,
                          (uint32_t)dma_buf_a,
                          (uint32_t)dma_buf_b,
                          DOUBLE_BUF_SIZE,
                          DMA_DIR_PERIPH_DST,
                          DMA_PERIPH_DATA_SIZE_BYTE,
                          DMA_MemoryDataSize_Byte,
                          DMA_PRIORITY_HIGH);
    
    /* Pre-fill both buffers */
    for (int i = 0; i < DOUBLE_BUF_SIZE; ++i)
    {
        dma_buf_a[i] = i & 0xFF;
        dma_buf_b[i] = (i + 1) & 0xFF;
    }
    
    /* Start continuous transfer with callback */
    dma_double_buffer_start(&spi1_db, spi1_db_callback, NULL);
}

/* ===== Example 3: Polling-based transfer (blocking) ===== */

void dma_example_spi1_blocking(uint8_t* data, uint16_t len)
{
    SPI_InitType spi_init;
    GPIO_InitType gpio_init;
    
    /* Enable clocks */
    dma_init();
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_SPI1 | RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_AFIO, ENABLE);
    
    /* Configure SPI1 pins */
    GPIO_InitStruct(&gpio_init);
    gpio_init.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpio_init.GPIO_Mode = GPIO_MODE_AF_PP;
    gpio_init.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitPeripheral(GPIOA, &gpio_init);
    
    /* Configure SPI1: 8-bit, MSB first, mode 0 */
    SPI_InitStruct(&spi_init);
    spi_init.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
    spi_init.SpiMode = SPI_MODE_MASTER;
    spi_init.DataLen = SPI_DATA_SIZE_8BITS;
    spi_init.CLKPOL = SPI_CLKPOL_LOW;
    spi_init.CLKPHA = SPI_CLKPHA_FIRST_EDGE;
    spi_init.NSS = SPI_NSS_SOFT;
    spi_init.BaudRatePres = SPI_BR_PRESCALER_8;  /* Clock / 8 */
    spi_init.FirstBit = SPI_FB_MSB;
    spi_init.CRCPoly = 7;
    SPI_Init(SPI1, &spi_init);
    SPI_Enable(SPI1, ENABLE);
    
    /* Start transfer without interrupt */
    dma_clear_callback(DMA_CH3);
    dma_start_transfer(DMA_CH3,
                      SPI1_DATA_REG,
                      (uint32_t)data,
                      len,
                      DMA_DIR_PERIPH_DST,
                      DMA_PERIPH_INC_DISABLE,
                      DMA_MEM_INC_ENABLE,
                      DMA_PERIPH_DATA_SIZE_BYTE,
                      DMA_MemoryDataSize_Byte,
                      DMA_MODE_NORMAL,
                      DMA_PRIORITY_HIGH,
                      DMA_M2M_DISABLE);
    
    /* Wait for completion (timeout = 1000 ms) */
    if (!dma_wait_complete(DMA_CH3, 1000))
    {
        /* Timeout - transfer failed */
    }
    
    dma_stop_transfer(DMA_CH3);
    SPI_Enable(SPI1, DISABLE);
}
