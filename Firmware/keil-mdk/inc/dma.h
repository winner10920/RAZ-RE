/* dma.h - convenience wrapper for N32G031 DMA with callbacks, polling, and double-buffer support
 */
#ifndef N32G031_DMA_WRAPPER_H
#define N32G031_DMA_WRAPPER_H

#include <stdint.h>
#include <stdbool.h>
#include "n32g031.h"

#include "n32g031_dma.h"

typedef void (*dma_callback_t)(void* ctx, uint32_t flags);

/* ===== Initialization ===== */
void dma_init(void);

/* ===== Callback Management ===== */
void dma_set_callback(DMA_ChannelType* channel, dma_callback_t cb, void* ctx);
void dma_clear_callback(DMA_ChannelType* channel);

/* ===== Basic Transfer Control ===== */
/* Start a DMA transfer - convenience wrapper around DMA_Init + EnableChannel
 * direction: DMA_DIR_PERIPH_SRC or DMA_DIR_PERIPH_DST
 * periph_inc/mem_inc: DMA_PERIPH_INC_ENABLE/DMA_PERIPH_INC_DISABLE and DMA_MEM_INC_ENABLE/DMA_MEM_INC_DISABLE
 * periph_size / mem_size: DMA_PERIPH_DATA_SIZE_* / DMA_MemoryDataSize_*
 * mode: DMA_MODE_NORMAL or DMA_MODE_CIRCULAR
 */
void dma_start_transfer(DMA_ChannelType* channel,
                        uint32_t periph_addr,
                        uint32_t mem_addr,
                        uint16_t bufsize,
                        uint32_t direction,
                        uint32_t periph_inc,
                        uint32_t mem_inc,
                        uint32_t periph_size,
                        uint32_t mem_size,
                        uint32_t mode,
                        uint32_t priority,
                        uint32_t m2m);

void dma_stop_transfer(DMA_ChannelType* channel);

/* ===== Status and Polling ===== */
void dma_clear_flags(uint32_t flags, DMA_Module* DMAx);
FlagStatus dma_get_flag(uint32_t flag, DMA_Module* DMAx);

/* Poll until transfer complete (TC) flag set or timeout (ms). Returns true if complete. */
bool dma_wait_complete(DMA_ChannelType* channel, uint32_t timeout_ms);

/* Get remaining byte count in current transfer */
uint16_t dma_get_remaining_count(DMA_ChannelType* channel);

/* ===== Double Buffering ===== */
typedef struct {
    DMA_ChannelType* channel;
    uint32_t periph_addr;
    uint32_t buf[2];           /* Two buffers */
    uint16_t buf_size;
    int active_buf;            /* 0 or 1 */
} dma_double_buffer_t;

/* Initialize double-buffered transfer. Call this once. */
void dma_double_buffer_init(dma_double_buffer_t* db,
                           DMA_ChannelType* channel,
                           uint32_t periph_addr,
                           uint32_t buf0, uint32_t buf1,
                           uint16_t buf_size,
                           uint32_t direction,
                           uint32_t periph_size,
                           uint32_t mem_size,
                           uint32_t priority);

/* Start double-buffered transfer. Uses circular mode. */
void dma_double_buffer_start(dma_double_buffer_t* db, dma_callback_t cb, void* ctx);

/* Get the inactive buffer (safe to write/read while transfer ongoing) */
uint32_t dma_double_buffer_get_inactive(dma_double_buffer_t* db);

// /* ===== IRQ Handler ===== */
// /* IRQ handler helper; call from DMA_IRQ_HANDLER() in `src/n32g031_it.c`. */
// void dma_irq_handler(void);

#endif
