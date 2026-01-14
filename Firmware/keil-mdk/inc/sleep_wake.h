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

#ifdef __cplusplus
}
#endif

#endif /* __SLEEP_WAKE_H__ */
