/**
 * @file iwdg.c
 * @brief Independent Watchdog Timer driver implementation for N32G031
 */

#include "iwdg.h"
#include "n32g031_rcc.h"

/* LSI oscillator frequency (typical value - can vary 30-60kHz) */
#define LSI_FREQ_HZ 40000

/* Global state tracking */
static uint8_t g_iwdg_suspended = 0;
static uint8_t g_iwdg_enabled = 0;

/**
 * @brief Wait for IWDG register update to complete
 */
static void IWDG_WaitForUpdate(void)
{
    /* Wait until PVU, RVU flags are reset */
    while ((IWDG->STS & (IWDG_STS_PVU | IWDG_STS_CRVU)) != 0)
    {
        /* Wait for registers to update */
    }
}

/**
 * @brief Calculate optimal prescaler and reload for desired timeout
 */
uint32_t IWDG_CalculateTimings(uint32_t timeout_ms, IWDG_Prescaler_Type *prescaler, uint16_t *reload)
{
    uint32_t timeout_ticks = (timeout_ms * LSI_FREQ_HZ) / 1000;
    uint32_t prescaler_value;
    uint32_t actual_timeout_ms;
    
    /* Try each prescaler from smallest to largest */
    const uint32_t prescaler_values[] = {4, 8, 16, 32, 64, 128, 256};
    
    for (int i = 0; i < 7; i++)
    {
        prescaler_value = prescaler_values[i];
        uint32_t reload_value = timeout_ticks / prescaler_value;
        
        if (reload_value <= 4095)  /* 12-bit reload register */
        {
            *prescaler = (IWDG_Prescaler_Type)i;
            *reload = (uint16_t)reload_value;
            
            /* Calculate actual timeout achieved */
            actual_timeout_ms = (reload_value * prescaler_value * 1000) / LSI_FREQ_HZ;
            return actual_timeout_ms;
        }
    }
    
    /* If no valid combination found, use maximum values */
    *prescaler = IWDG_PRESCALER_256;
    *reload = 4095;
    actual_timeout_ms = (4095 * 256 * 1000) / LSI_FREQ_HZ;
    
    return actual_timeout_ms;
}

/**
 * @brief Initialize IWDG with specified timeout in milliseconds
 */
void IWDG_Init(uint32_t timeout_ms)
{
    /* Check if initialization is suspended */
    if (g_iwdg_suspended)
    {
        return;  /* Don't initialize if suspended */
    }
    
    IWDG_Prescaler_Type prescaler;
    uint16_t reload;
    
    /* Calculate optimal prescaler and reload values */
    IWDG_CalculateTimings(timeout_ms, &prescaler, &reload);
    
    /* Initialize with calculated values */
    IWDG_InitCustom(prescaler, reload);
}

/**
 * @brief Initialize IWDG with custom prescaler and reload values
 */
void IWDG_InitCustom(IWDG_Prescaler_Type prescaler, uint16_t reload)
{
    /* Check if initialization is suspended */
    if (g_iwdg_suspended)
    {
        return;  /* Don't initialize if suspended */
    }
    
    /* Ensure reload value is within 12-bit range */
    if (reload > 4095)
    {
        reload = 4095;
    }
    
    /* Enable write access to IWDG registers */
    IWDG->KEY = IWDG_KEY_ACCESS;
    
    /* Set prescaler */
    IWDG->PREDIV = (uint32_t)prescaler;
    
    /* Set reload value */
    IWDG->RELV = reload;
    
    /* Wait for registers to update */
    IWDG_WaitForUpdate();
    
    /* Reload counter with RELV value */
    IWDG->KEY = IWDG_KEY_RELOAD;
    
    /* Enable IWDG (cannot be disabled after this) */
    IWDG->KEY = IWDG_KEY_ENABLE;
    
    /* Mark as enabled */
    g_iwdg_enabled = 1;
}

/**
 * @brief Feed/refresh the watchdog timer
 */
void IWDG_Feed(void)
{
    /* Reload counter */
    IWDG->KEY = IWDG_KEY_RELOAD;
}

/**
 * @brief Force an immediate MCU reset via watchdog
 */
void IWDG_ForceReset(void)
{
    /* Enable write access */
    IWDG->KEY = IWDG_KEY_ACCESS;
    
    /* Set minimum timeout (prescaler /4, reload = 1) */
    IWDG->PREDIV = IWDG_PRESCALER_4;
    IWDG->RELV = 1;
    
    /* Wait for update */
    IWDG_WaitForUpdate();
    
    /* Enable IWDG if not already enabled */
    IWDG->KEY = IWDG_KEY_ENABLE;
    
    /* Do NOT feed the watchdog - let it expire and reset */
    /* Timeout will occur in ~100us */
    
    /* Wait for reset */
    while (1)
    {
        /* MCU will reset shortly */
    }
}

/**
 * @brief Check if last reset was caused by IWDG
 */
uint8_t IWDG_WasResetCausedByWatchdog(void)
{
    /* Check RCC reset status register for IWDG reset flag */
    if (RCC->CTRLSTS & RCC_CTRLSTS_IWDGRSTF)
    {
        /* Clear the reset flag by setting RMRSTF bit */
        RCC->CTRLSTS |= RCC_CTRLSTS_RMRSTF;
        return 1;
    }
    
    return 0;
}

/**
 * @brief Check if IWDG is currently enabled
 */
uint8_t IWDG_IsEnabled(void)
{
    return g_iwdg_enabled;
}

/**
 * @brief Extend watchdog timeout temporarily for critical sections
 */
void IWDG_ExtendTimeout(uint32_t extended_timeout_ms)
{
    if (!g_iwdg_enabled)
    {
        return;  /* Watchdog not running */
    }
    
    IWDG_Prescaler_Type prescaler;
    uint16_t reload;
    
    /* Calculate new timing values */
    IWDG_CalculateTimings(extended_timeout_ms, &prescaler, &reload);
    
    /* Enable write access */
    IWDG->KEY = IWDG_KEY_ACCESS;
    
    /* Update prescaler and reload */
    IWDG->PREDIV = (uint32_t)prescaler;
    IWDG->RELV = reload;
    
    /* Wait for update */
    IWDG_WaitForUpdate();
    
    /* Reload counter immediately */
    IWDG->KEY = IWDG_KEY_RELOAD;
}

/**
 * @brief Enter critical section - feed watchdog and prepare for longer operation
 */
void IWDG_EnterCriticalSection(uint32_t critical_section_duration_ms)
{
    if (!g_iwdg_enabled)
    {
        return;  /* Watchdog not running */
    }
    
    /* Feed watchdog first to get maximum time */
    IWDG_Feed();
    
    /* If critical section is longer than current timeout, extend it */
    /* Add 20% safety margin */
    uint32_t required_timeout = (critical_section_duration_ms * 120) / 100;
    
    if (required_timeout > 500)  /* Only extend if needed */
    {
        IWDG_ExtendTimeout(required_timeout);
    }
}

/**
 * @brief Exit critical section - feed watchdog immediately
 */
void IWDG_ExitCriticalSection(void)
{
    if (g_iwdg_enabled)
    {
        IWDG_Feed();
    }
}

/**
 * @brief Suspend watchdog functionality (prevent initialization)
 */
void IWDG_Suspend(void)
{
    g_iwdg_suspended = 1;
}

/**
 * @brief Resume watchdog functionality (allow initialization)
 */
void IWDG_Resume(void)
{
    g_iwdg_suspended = 0;
}
