/* Header for reverse-compiled utilities extracted from the firmware
 * Provides macro definitions, external data and function prototypes
 * for the implementations in geminireversecompile.c
 */
#ifndef __GEMINI_REVC_H__
#define __GEMINI_REVC_H__

#include <stdint.h>
#include <stdbool.h>
#include "n32g031.h"

#ifdef __cplusplus
extern "C" {
#endif

// --- Hardware Registers ---
#define RCC_BASE        0x40021000
#define RCC_CTRL        ((volatile int32_t *)(RCC_BASE + 0x00))
#define RCC_CFG         ((volatile int32_t *)(RCC_BASE + 0x04))
#define RCC_APB2PRST    ((volatile int32_t *)(RCC_BASE + 0x0C))
#define RCC_APB1PRST    ((volatile int32_t *)(RCC_BASE + 0x10))
#define RCC_AHBPCLKEN   ((volatile int32_t *)(RCC_BASE + 0x14))
#define RCC_APB2PCLKEN  ((volatile int32_t *)(RCC_BASE + 0x18))
#define RCC_APB1PCLKEN  ((volatile int32_t *)(RCC_BASE + 0x1C))
#define RCC_CFG2        ((volatile int32_t *)(RCC_BASE + 0x2C))

// FLASH Registers
#define FLASH_AC        ((volatile int32_t *)0x40022000)

// IWDG Registers
#define IWDG_KEY        ((volatile int32_t *)0x40003000)
#define IWDG_PREDIV     ((volatile int32_t *)0x40003004)
#define IWDG_RELV       ((volatile int32_t *)0x40003008)

// DMA Registers
#define DMA_BASE        0x40020000
#define DMA_INTSTS      ((volatile int32_t *)(DMA_BASE + 0x00))
#define DMA_INTCLR      ((volatile int32_t *)(DMA_BASE + 0x04))
#define DMA_CH3_CFG     ((volatile int32_t *)(DMA_BASE + 0x30))
#define DMA_CH3_TXNUM   ((volatile int32_t *)(DMA_BASE + 0x34))
#define DMA_CH3_PADDR   ((volatile int32_t *)(DMA_BASE + 0x38))
#define DMA_CH3_MADDR   ((volatile int32_t *)(DMA_BASE + 0x3C))

// GPIO Offsets
#define GPIO_PMODE      0x00
#define GPIO_POTYPE     0x04
#define GPIO_SR         0x08
#define GPIO_PUPD       0x0C
#define GPIO_POD        0x14
#define GPIO_PBSC       0x18
#define GPIO_PBC        0x28
#define GPIO_AFL        0x20
#define GPIO_AFH        0x24
#define GPIO_DS         0x2C

// --- Pin Definitions ---
#define LCD_GPIO_PORT       GPIOB
#define LCD_RST_PIN         GPIO_PIN_6
#define LCD_DC_PIN          GPIO_PIN_7

#define SPI_GPIO_PORT       GPIOA
#define SPI_SCK_PIN         GPIO_PIN_5
#define SPI_MOSI_PIN        GPIO_PIN_7

// --- Commands ---
#define CMD_SLEEP_OUT       0x11
#define CMD_DISPLAY_ON      0x29
#define CMD_MADCTL          0x36
#define CMD_COLMOD          0x3A
#define CMD_RAMWR           0x2C

/* External data defined in the .c file */
extern const uint8_t init_cmds[];
extern const unsigned init_cmds_size;

/* Function prototypes (forward declarations) */
int32_t FLASH_WriteWord(int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6);
int32_t CallFunctionPointers(void);
void ClearBit54(void* arg1);
int32_t ADC_InitWrapper(int32_t arg1, int32_t arg2);
int32_t ADC_Config(int32_t arg1);

int32_t Math_DivMod(int32_t arg1, int32_t arg2);
int32_t Math_AbsDiv(int32_t arg1, int32_t arg2);
void Math_Min(int32_t arg1, int32_t arg2);

int32_t GPIO_Init(int32_t* port_base, int32_t* config);

int32_t Main_Loop(void);
void Enter_LowPowerMode(int32_t arg1);

int32_t IWDG_Reload(void);
int32_t IWDG_Start(void);
int32_t IWDG_Init(void);

int32_t Data_Transfer(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int16_t* arg5);

void RCC_EnableAPB2(int32_t bit_mask, int32_t enable);
void RCC_EnableAPB1(int32_t bit_mask, int32_t enable);

void LCD_WriteCmd(uint8_t cmd);
void LCD_WriteData(uint8_t data);
void LCD_Init(void);

#ifdef __cplusplus
}
#endif

#endif
