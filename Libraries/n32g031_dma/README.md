DMA wrapper for N32G031
=======================

Convenience wrapper around `firmware/n32g031_std_periph_driver` DMA API with callback registration, polling helpers, and double-buffering support.

## Features

- **Channel callbacks**: Register interrupt callbacks per DMA channel
- **Polling/blocking transfers**: `dma_wait_complete()` for simple blocking transfers
- **Double-buffering**: Built-in helpers for continuous circular transfers
- **Status helpers**: Remaining count, flag management
- **IRQ integration**: Single `dma_irq_handler()` call from application interrupt handler

## Quick Start

### 1. Basic Single Transfer

```c
#include "dma.h"

uint8_t buffer[256];
dma_init();  // Call once at startup

dma_set_callback(DMA_CH1, my_callback, NULL);
dma_start_transfer(
    DMA_CH1,
    PERIPH_ADDR,           // e.g., SPI1 data register
    (uint32_t)buffer,      // Memory buffer
    256,                   // Size in bytes
    DMA_DIR_PERIPH_SRC,    // From peripheral to memory
    DMA_PERIPH_INC_DISABLE,
    DMA_MEM_INC_ENABLE,
    DMA_PERIPH_DATA_SIZE_BYTE,
    DMA_MemoryDataSize_Byte,
    DMA_MODE_NORMAL,
    DMA_PRIORITY_HIGH,
    DMA_M2M_DISABLE
);
```

### 2. Blocking Transfer (with polling)

```c
dma_start_transfer(...);  // Configure transfer
if (dma_wait_complete(DMA_CH1, 1000))  // Wait up to 1000ms
{
    // Success
    uint16_t remaining = dma_get_remaining_count(DMA_CH1);
}
dma_stop_transfer(DMA_CH1);
```

### 3. Double-Buffered (Continuous Streaming)

```c
dma_double_buffer_t db;
uint8_t buf_a[256], buf_b[256];

dma_double_buffer_init(&db, DMA_CH3, PERIPH_ADDR,
                       (uint32_t)buf_a, (uint32_t)buf_b, 256,
                       DMA_DIR_PERIPH_DST,
                       DMA_PERIPH_DATA_SIZE_BYTE,
                       DMA_MemoryDataSize_Byte,
                       DMA_PRIORITY_HIGH);

dma_double_buffer_start(&db, double_buf_callback, NULL);

// In callback:
static void double_buf_callback(void* ctx, uint32_t flags)
{
    if (flags & DMA_INT_TXC)
    {
        uint32_t inactive = dma_double_buffer_get_inactive(&db);
        // Refill inactive buffer for next cycle
    }
}
```

### 4. Interrupt Handler Integration

In `src/n32g031_it.c`:

```c
void DMA_IRQ_HANDLER(void)
{
    dma_irq_handler();  // Dispatch to registered callbacks
}
```

## API Reference

### Initialization
- `void dma_init()` – Enable DMA clock and clear callbacks

### Callbacks
- `void dma_set_callback(channel, callback, context)`
- `void dma_clear_callback(channel)`

### Transfer Control
- `void dma_start_transfer(...)` – Configure and start a transfer
- `void dma_stop_transfer(channel)` – Disable channel

### Status & Polling
- `FlagStatus dma_get_flag(flag)` – Check status (TC, HT, TE flags)
- `void dma_clear_flags(flags)`
- `bool dma_wait_complete(channel, timeout_ms)` – Poll for TC
- `uint16_t dma_get_remaining_count(channel)` – Get remaining bytes

### Double Buffering
- `void dma_double_buffer_init(db, channel, periph, buf0, buf1, size, ...)`
- `void dma_double_buffer_start(db, callback, context)`
- `uint32_t dma_double_buffer_get_inactive(db)` – Get buffer to refill

### IRQ Handling
- `void dma_irq_handler()` – Call from `DMA_IRQ_HANDLER()`

## Examples

See `src/dma_example.c` for:
- Simple SPI1 TX transfer
- Continuous SPI1 streaming with double-buffering
- Blocking transfer with polling

## References

- N32G031 Datasheet, DMA Controller (page 148+)
- `firmware/n32g031_std_periph_driver/inc/n32g031_dma.h`
