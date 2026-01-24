/**
 * @file sleep_wake.h
 * @brief Sleep/Wake management for N32G031K8
 * 
 * Provides sleep mode control with timeout and button wake capability.
 * Automatically powers down non-essential peripherals on sleep and
 * restores them on wake.
 */

#ifndef __SLEEP_WAKE_H__
#define __SLEEP_WAKE_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize sleep/wake system
 * @param timeout_sec Timeout in seconds before device goes to sleep (0 = disabled)
 * @note Must be called after all peripheral initialization
 */
void SleepWake_Init(uint32_t timeout_sec);

/**
 * @brief Enable sleep/wake with a timeout
 * @param timeout_sec Timeout in seconds before sleep (0 = disable timeout)
 */
void SleepWake_EnableTimeout(uint32_t timeout_sec);

/**
 * @brief Disable sleep/wake timeout
 */
void SleepWake_DisableTimeout(void);

/**
 * @brief Reset the inactivity timer (call on user activity)
 */
void SleepWake_ResetTimer(void);

/**
 * @brief Put device to sleep immediately
 * Turns off backlight and non-essential peripherals
 */
void SleepWake_GoToSleep(void);

/**
 * @brief Check if device should be sleeping (call periodically)
 * @return true if timeout expired and sleep should be triggered
 */
bool SleepWake_CheckTimeout(void);

/**
 * @brief Get current sleep state
 * @return true if device is in sleep mode
 */
bool SleepWake_IsSleeping(void);

/**
 * @brief Wake up from sleep (called by button interrupt or timeout logic)
 * Restores backlight and peripherals
 */
void SleepWake_WakeUp(void);

/**
 * @brief Get remaining time before sleep (in seconds)
 * @return Seconds until sleep (0 if timeout disabled or already sleeping)
 */
uint32_t SleepWake_GetTimeRemaining(void);

/**
 * @brief Initialize external interrupts for button and mic pins
 * Called automatically by SleepWake_Init()
 */
void SleepWake_InitExternalInterrupts(void);

/**
 * @brief Enable external wake interrupts
 */
void SleepWake_EnableExternalInterrupts(void);

/**
 * @brief Disable external wake interrupts
 */
void SleepWake_DisableExternalInterrupts(void);

/**
 * @brief Check if external interrupt triggered a wake event
 * @return true if BUTTON_PIN or MIC_PIN interrupt occurred
 */
bool SleepWake_IsWakeInterruptTriggered(void);

/**
 * @brief Clear the wake interrupt flag
 */
void SleepWake_ClearWakeInterrupt(void);

/**
 * @brief Initialize LPTIM for periodic wake-up from ultra-low power mode
 * @param wake_interval_sec Interval in seconds for LPTIM wake (e.g., 30 seconds)
 */
void SleepWake_InitLPTIM(uint32_t wake_interval_sec);

/**
 * @brief Enter ultra-low power mode (STOP mode)
 * Disables LCD_FLASH_PWR_EN and lV_CUTOFF_EN, keeps LP4086_ISET low
 * Wakes on LPTIM timeout or external interrupt
 */
void SleepWake_EnterUltraLowPower(void);

/**
 * @brief Check if device was woken by LPTIM
 * @return true if LPTIM triggered the wake
 */
bool SleepWake_IsLPTIMWake(void);

/**
 * @brief Check if SWD/JLink is connected
 * @return true if debugger is connected
 */
bool SleepWake_IsSWDConnected(void);

/**
 * @brief Handle wake from ultra-low power mode
 * Provides 10-second window for SWD connection or returns to ultra-low power
 * @return true if should remain fully awake (SWD connected or external interrupt)
 */
bool SleepWake_HandleUltraLowPowerWake(void);

/**
 * @brief Restore full power mode
 * Re-enables LCD_FLASH_PWR_EN and lV_CUTOFF_EN
 */
void SleepWake_RestoreFullPower(void);

/**
 * @brief Enable/disable debug mode
 * @param enable true to enable debug mode (full wake every minute)
 */
void SleepWake_SetDebugMode(bool enable);

/**
 * @brief Get current debug mode status
 * @return true if debug mode is enabled
 */
bool SleepWake_IsDebugMode(void);

#ifdef __cplusplus
}
#endif

#endif /* __SLEEP_WAKE_H__ */
