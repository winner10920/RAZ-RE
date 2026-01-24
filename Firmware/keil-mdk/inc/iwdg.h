/**
 * @file iwdg.h
 * @brief Independent Watchdog Timer driver for N32G031
 */

#ifndef __IWDG_H__
#define __IWDG_H__

#include <stdint.h>
#include "n32g031.h"

/* IWDG Key Values */
#define IWDG_KEY_RELOAD    0xAAAA  /* Reload counter */
#define IWDG_KEY_ENABLE    0xCCCC  /* Enable IWDG */
#define IWDG_KEY_ACCESS    0x5555  /* Enable register access */

/* IWDG Prescaler Values */
typedef enum {
    IWDG_PRESCALER_4   = 0x00,  /* Divider /4   - Max timeout ~410ms @ 40kHz LSI */
    IWDG_PRESCALER_8   = 0x01,  /* Divider /8   - Max timeout ~820ms @ 40kHz LSI */
    IWDG_PRESCALER_16  = 0x02,  /* Divider /16  - Max timeout ~1.64s @ 40kHz LSI */
    IWDG_PRESCALER_32  = 0x03,  /* Divider /32  - Max timeout ~3.28s @ 40kHz LSI */
    IWDG_PRESCALER_64  = 0x04,  /* Divider /64  - Max timeout ~6.55s @ 40kHz LSI */
    IWDG_PRESCALER_128 = 0x05,  /* Divider /128 - Max timeout ~13.1s @ 40kHz LSI */
    IWDG_PRESCALER_256 = 0x06   /* Divider /256 - Max timeout ~26.2s @ 40kHz LSI */
} IWDG_Prescaler_Type;

/* Common timeout presets (in milliseconds) */
#define IWDG_TIMEOUT_100MS   100
#define IWDG_TIMEOUT_250MS   250
#define IWDG_TIMEOUT_500MS   500
#define IWDG_TIMEOUT_1S      1000
#define IWDG_TIMEOUT_2S      2000
#define IWDG_TIMEOUT_5S      5000
#define IWDG_TIMEOUT_10S     10000

/**
 * @brief Initialize IWDG with specified timeout in milliseconds
 * @param timeout_ms Desired timeout in milliseconds (100ms to 26000ms)
 * @note LSI clock assumed to be 40kHz (typical). Actual may vary 30-60kHz.
 * @note Once started, IWDG cannot be stopped except by MCU reset
 */
void IWDG_Init(uint32_t timeout_ms);

/**
 * @brief Initialize IWDG with custom prescaler and reload values
 * @param prescaler IWDG prescaler divider (use IWDG_PRESCALER_x enum)
 * @param reload Reload value (0 to 4095)
 * @note Timeout = (prescaler_value / LSI_freq) * reload
 * @note Once started, IWDG cannot be stopped except by MCU reset
 */
void IWDG_InitCustom(IWDG_Prescaler_Type prescaler, uint16_t reload);

/**
 * @brief Feed/refresh the watchdog timer (reset counter)
 * @note Must be called periodically before timeout expires
 */
void IWDG_Feed(void);

/**
 * @brief Force an immediate MCU reset via watchdog
 * @note This function does not return - MCU will reset
 */
void IWDG_ForceReset(void);

/**
 * @brief Check if last reset was caused by IWDG
 * @return 1 if reset was caused by IWDG, 0 otherwise
 */
uint8_t IWDG_WasResetCausedByWatchdog(void);

/**
 * @brief Calculate optimal prescaler and reload for desired timeout
 * @param timeout_ms Desired timeout in milliseconds
 * @param prescaler Pointer to store calculated prescaler
 * @param reload Pointer to store calculated reload value
 * @return Actual timeout in milliseconds that will be achieved
 */
uint32_t IWDG_CalculateTimings(uint32_t timeout_ms, IWDG_Prescaler_Type *prescaler, uint16_t *reload);

/**
 * @brief Check if IWDG is currently enabled
 * @return 1 if IWDG is running, 0 if not started
 * @note IWDG cannot be disabled once enabled - only MCU reset can stop it
 */
uint8_t IWDG_IsEnabled(void);

/**
 * @brief Extend watchdog timeout temporarily for critical sections
 * @param extended_timeout_ms New timeout in milliseconds (must be longer than current)
 * @note Feed watchdog before calling this to maximize available time
 * @note Original timeout is NOT restored - you must call IWDG_Init again if needed
 * @warning This reconfigures the watchdog - only use before long operations
 */
void IWDG_ExtendTimeout(uint32_t extended_timeout_ms);

/**
 * @brief Enter critical section - feed watchdog and prepare for longer operation
 * @param critical_section_duration_ms Expected duration of critical section
 * @note This feeds the watchdog and extends timeout if needed
 * @note Call IWDG_ExitCriticalSection() when done
 */
void IWDG_EnterCriticalSection(uint32_t critical_section_duration_ms);

/**
 * @brief Exit critical section - feed watchdog immediately
 * @note Call this after completing the critical section started with IWDG_EnterCriticalSection
 */
void IWDG_ExitCriticalSection(void);

/**
 * @brief Suspend watchdog functionality (conditional initialization helper)
 * @note This does NOT disable hardware watchdog - it only prevents initialization
 * @note Call IWDG_Resume() to allow initialization again
 * @note If IWDG is already running, this has no effect
 */
void IWDG_Suspend(void);

/**
 * @brief Resume watchdog functionality (allow initialization)
 * @note This allows IWDG_Init() to be called again
 */
void IWDG_Resume(void);

#endif /* __IWDG_H__ */
