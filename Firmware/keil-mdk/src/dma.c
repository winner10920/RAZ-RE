/* dma.c - convenience wrapper for N32G031 DMA
 */
#include "dma.h"
#include "n32g031_rcc.h"
#include <stddef.h>

static dma_callback_t callbacks[6];
static void* cb_ctx[6];

void dma_init(void)
{
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_DMA, ENABLE);
    for (int i = 0; i < 6; ++i)
    {
        callbacks[i] = NULL;
        cb_ctx[i] = NULL;
    }
}

void dma_set_callback(DMA_ChannelType* channel, dma_callback_t cb, void* ctx)
{
    int idx = channel - (DMA_ChannelType*)DMA_CH1;
    if (idx >= 0 && idx < 5)
    {
        callbacks[idx] = cb;
        cb_ctx[idx] = ctx;
    }
}

void dma_clear_callback(DMA_ChannelType* channel)
{
    dma_set_callback(channel, NULL, NULL);
}

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
                        uint32_t m2m)
{
    DMA_InitType init;
    DMA_DeInit(channel);
    DMA_StructInit(&init);
    init.PeriphAddr = periph_addr;
    init.MemAddr = mem_addr;
    init.Direction = direction;
    init.BufSize = bufsize;
    init.PeriphInc = periph_inc;
    init.DMA_MemoryInc = mem_inc;
    init.PeriphDataSize = periph_size;
    init.MemDataSize = mem_size;
    init.CircularMode = mode;
    init.Priority = priority;
    init.Mem2Mem = m2m;
    DMA_Init(channel, &init);
    DMA_ConfigInt(channel, DMA_INT_TXC | DMA_INT_HTX | DMA_INT_ERR, ENABLE);
    DMA_EnableChannel(channel, ENABLE);
}

void dma_stop_transfer(DMA_ChannelType* channel)
{
    DMA_EnableChannel(channel, DISABLE);
}

void dma_clear_flags(uint32_t flags, DMA_Module* DMAx)
{
    DMA_ClearFlag(flags, DMAx);
}

FlagStatus dma_get_flag(uint32_t flag, DMA_Module* DMAx)
{
    return DMA_GetFlagStatus(flag, DMAx);
}

bool dma_wait_complete(DMA_ChannelType* channel, uint32_t timeout_ms)
{
    /* Determine TC flag for this channel */
    uint32_t tc_flag;
    DMA_Module* DMAx = (DMA_Module*)((uintptr_t)channel & 0xFFFFF000);
    int ch_idx = channel - (DMA_ChannelType*)DMA_CH1;
    if (ch_idx < 0 || ch_idx >= 5) return false;
    
    switch (ch_idx)
    {
        case 0: tc_flag = DMA_FLAG_TC1; break;
        case 1: tc_flag = DMA_FLAG_TC2; break;
        case 2: tc_flag = DMA_FLAG_TC3; break;
        case 3: tc_flag = DMA_FLAG_TC4; break;
        case 4: tc_flag = DMA_FLAG_TC5; break;
        default: return false;
    }
    
    /* Simple polling loop (not tick-based; adjust for real timer if needed) */
    volatile uint32_t wait_loops = timeout_ms * 1000; /* rough approximation */
    while (wait_loops--)
    {
        if (dma_get_flag(tc_flag, DMAx) == SET)
            return true;
    }
    return false;
}

uint16_t dma_get_remaining_count(DMA_ChannelType* channel)
{
    return DMA_GetCurrDataCounter(channel);
}

void dma_double_buffer_init(dma_double_buffer_t* db,
                           DMA_ChannelType* channel,
                           uint32_t periph_addr,
                           uint32_t buf0, uint32_t buf1,
                           uint16_t buf_size,
                           uint32_t direction,
                           uint32_t periph_size,
                           uint32_t mem_size,
                           uint32_t priority)
{
    db->channel = channel;
    db->periph_addr = periph_addr;
    db->buf[0] = buf0;
    db->buf[1] = buf1;
    db->buf_size = buf_size;
    db->active_buf = 0;
}

void dma_double_buffer_start(dma_double_buffer_t* db, dma_callback_t cb, void* ctx)
{
    dma_set_callback(db->channel, cb, ctx);
    dma_start_transfer(db->channel,
                       db->periph_addr,
                       db->buf[0],
                       db->buf_size,
                       DMA_DIR_PERIPH_DST,  /* Memory to Peripheral */
                       DMA_PERIPH_INC_DISABLE,
                       DMA_MEM_INC_ENABLE,
                       DMA_PERIPH_DATA_SIZE_BYTE,
                       DMA_MemoryDataSize_Byte,
                       DMA_MODE_CIRCULAR,
                       DMA_PRIORITY_HIGH,
                       DMA_M2M_DISABLE);
}

uint32_t dma_double_buffer_get_inactive(dma_double_buffer_t* db)
{
    return db->buf[1 - db->active_buf];
}

// void dma_irq_handler(void)
// {
//     uint32_t sr;
//     for (int ch = 0; ch < 5; ++ch)
//     {
//         uint32_t tc = (DMA_INT_TXC1 << (ch * 4));
//         uint32_t ht = (DMA_INT_HTX1 << (ch * 4));
//         uint32_t err = (DMA_INT_ERR1 << (ch * 4));
//         DMA_Module DMAx = *(DMA_Module*)DMA_BASE + (ch / 2) * 0x14;
//         uint32_t flags = 0;

//         if (DMA_GetIntStatus(tc, &DMAx) == SET)
//             flags |= DMA_INT_TXC;
//         if (DMA_GetIntStatus(ht, &DMAx) == SET)
//             flags |= DMA_INT_HTX;
//         if (DMA_GetIntStatus(err, &DMAx) == SET)
//             flags |= DMA_INT_ERR;

//         if (flags)
//         {
//             DMA_ClrIntPendingBit((tc|ht|err), &DMAx);
//             if (callbacks[ch])
//                 callbacks[ch](cb_ctx[ch], flags);
//         }
//     }
// }
