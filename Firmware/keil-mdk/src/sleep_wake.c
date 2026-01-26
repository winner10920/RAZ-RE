/**
 * @file sleep_wake.c
 * @brief Sleep/Wake implementation for N32G031K8
 */

#include "sleep_wake.h"
#include "pwm.h"
#include "n32g031_rcc.h"
#include "n32g031_gpio.h"
#include "n32g031_exti.h"
#include "n32g031_pwr.h"
#include "n32g031_lptim.h"
#include "core_cm0.h"
#include "nv3029.h"
#include "dma.h"

/* Sleep/Wake state machine */
static bool g_is_sleeping = false;
static bool g_is_ultra_low_power = false;
static uint32_t g_timeout_sec = 0;
static uint32_t g_inactivity_counter = 0;
static uint32_t g_timeout_counter = 0;
static volatile bool g_wake_interrupt_triggered = false;
static volatile bool g_lptim_wake_triggered = false;
static bool g_debug_mode = false;
static uint32_t g_lptim_wake_interval = 30;

/* Saved peripheral state for restoration */
static uint8_t g_saved_backlight_brightness = 0;
static bool g_saved_tv1_state = false;
static bool g_saved_tv2_state = false;
static bool g_saved_lcd_flash_pwr = false;
static bool g_saved_lv_cutoff = false;
static bool g_saved_lp4086_iset = false;

/* GPIO Pin definitions */
#define BUTTON_PORT GPIOA
#define BUTTON_PIN GPIO_PIN_7
#define BUTTON_EXTI_LINE EXTI_LINE7

#define MIC_PORT GPIOA
#define MIC_PIN GPIO_PIN_3
#define MIC_EXTI_LINE EXTI_LINE3



#define LV_CUTOFF_EN_PORT GPIOA
#define LV_CUTOFF_EN_PIN GPIO_PIN_12

#define LP4086_ISET_PORT GPIOB
#define LP4086_ISET_PIN GPIO_PIN_0

#define TV1_PORT GPIOA
#define TV1_PIN GPIO_PIN_5

#define TV2_PORT GPIOB
#define TV2_PIN GPIO_PIN_8

#define LCD_BACKLIGHT_PIN GPIOA,GPIO_PIN_6

/* Timer tick counter (assuming 100ms per call to SleepWake_CheckTimeout) */
#define TICKS_PER_SECOND 3

/**
 * @brief Restore system clock after waking from STOP mode
 * After STOP mode, the system runs on HSI (8MHz). This restores the original clock config.
 */
static void SleepWake_RestoreSystemClock(void)
{
    /* After wake from STOP, system is running on HSI (8MHz) */
    /* Need to restore the original clock configuration */
    
    /* Call SystemInit to restore clock to configured state (48MHz HSI PLL) */
    extern void SystemInit(void);
    SystemInit();
    
    /* Alternative manual method if SystemInit doesn't work:
     * Enable HSI and wait for it to stabilize
     * Configure PLL if needed
     * Switch system clock back to PLL or HSE
     */
}

/**
 * @brief Initialize sleep/wake system
 */
void SleepWake_Init(uint32_t timeout_sec)
{
    g_timeout_sec = timeout_sec;
    g_inactivity_counter = 0;
    g_timeout_counter = 0;
    g_is_sleeping = false;
    g_is_ultra_low_power = false;
    g_wake_interrupt_triggered = false;
    g_lptim_wake_triggered = false;
    g_debug_mode = false;
    g_saved_backlight_brightness = 50;  /* Default brightness */
    
    /* Initialize external interrupts for wake */
    SleepWake_InitExternalInterrupts();
    
    /* Initialize LPTIM for 30-second wake intervals */
    SleepWake_InitLPTIM(30);
}

/**
 * @brief Initialize external interrupts for BUTTON and MIC pins
 * Configured for falling edge (button press, activity detection)
 */
void SleepWake_InitExternalInterrupts(void)
{
    GPIO_InitType gpio_init;
    EXTI_InitType exti_init;


    /* Configure EXTI for BUTTON_PIN (PA7 -> EXTI7) */
    EXTI_InitStruct(&exti_init);
    exti_init.EXTI_Line = BUTTON_EXTI_LINE;
    exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Falling;  /* Button press (low) */
    exti_init.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&exti_init);

    /* Configure EXTI for MIC_PIN (PA3 -> EXTI3) */
    exti_init.EXTI_Line = MIC_EXTI_LINE;
    exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Rising;  /* Activity detection */
    exti_init.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&exti_init);

    /* Enable EXTI interrupt handlers in NVIC */
    NVIC_EnableIRQ(EXTI2_3_IRQn);  /* EXTI3 is handled by EXTI2_3_IRQn */
    NVIC_EnableIRQ(EXTI4_15_IRQn); /* EXTI7 is handled by EXTI4_15_IRQn */
    NVIC_SetPriority(EXTI2_3_IRQn, 2);
    NVIC_SetPriority(EXTI4_15_IRQn, 2);
}

/**
 * @brief Disable external interrupts
 */
void SleepWake_DisableExternalInterrupts(void)
{
    EXTI_InitType exti_init;
    exti_init.EXTI_LineCmd = DISABLE;
    exti_init.EXTI_Line = BUTTON_EXTI_LINE;
    exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitPeripheral(&exti_init);
    
    exti_init.EXTI_Line = MIC_EXTI_LINE;
    EXTI_InitPeripheral(&exti_init);

    NVIC_DisableIRQ(EXTI2_3_IRQn);
    NVIC_DisableIRQ(EXTI4_15_IRQn);
}

/**
 * @brief Enable external interrupts
 */
void SleepWake_EnableExternalInterrupts(void)
{
    EXTI_InitType exti_init;
    exti_init.EXTI_LineCmd = ENABLE;
    exti_init.EXTI_Line = BUTTON_EXTI_LINE;
    exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitPeripheral(&exti_init);
    
    exti_init.EXTI_Line = MIC_EXTI_LINE;
    EXTI_InitPeripheral(&exti_init);

    NVIC_EnableIRQ(EXTI2_3_IRQn);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

/**
 * @brief Check if wake interrupt was triggered
 */
bool SleepWake_IsWakeInterruptTriggered(void)
{
    return g_wake_interrupt_triggered;
}

/**
 * @brief Clear wake interrupt flag
 */
void SleepWake_ClearWakeInterrupt(void)
{
    g_wake_interrupt_triggered = false;
}
void SleepWake_EnableTimeout(uint32_t timeout_sec)
{
    g_timeout_sec = timeout_sec;
    g_inactivity_counter = 0;
    g_timeout_counter = 0;
}

/**
 * @brief Disable timeout
 */
void SleepWake_DisableTimeout(void)
{
    g_timeout_sec = 0;
    g_inactivity_counter = 0;
    g_timeout_counter = 0;
}

/**
 * @brief Reset activity timer
 */
void SleepWake_ResetTimer(void)
{
    if (!g_is_sleeping)
    {
        g_inactivity_counter = 0;
        g_timeout_counter = 0;
    }
}

/**
 * @brief Check if timeout has expired
 * Call this periodically (e.g., every 100ms)
 */
bool SleepWake_CheckTimeout(void)
{
    if (g_timeout_sec == 0 || g_is_sleeping)
    {
        return false;  /* Timeout disabled or already sleeping */
    }

    g_inactivity_counter++;

    /* Check if timeout reached (timeout_sec * 10 ticks per second) */
    if (g_inactivity_counter >= (g_timeout_sec * TICKS_PER_SECOND))
    {
        return true;  /* Timeout expired */
    }

    return false;
}

/**
 * @brief Get remaining time before sleep
 */
uint32_t SleepWake_GetTimeRemaining(void)
{
    if (g_timeout_sec == 0 || g_is_sleeping)
    {
        return 0;
    }

    uint32_t ticks_remaining = (g_timeout_sec * TICKS_PER_SECOND) - g_inactivity_counter;
    return (ticks_remaining + TICKS_PER_SECOND - 1) / TICKS_PER_SECOND;  /* Round up */
}

/**
 * @brief Check if sleeping
 */
bool SleepWake_IsSleeping(void)
{
    return g_is_sleeping;
}

/**
 * @brief Put device to sleep
 * Turns off backlight and non-essential peripherals
 */
void SleepWake_GoToSleep(void)
{
    if (g_is_sleeping)
    {
        return;  /* Already sleeping */
    }

    /* Save current states */
    g_saved_backlight_brightness = PWM_GetDutyCycle();
    g_saved_tv1_state = GPIO_ReadOutputDataBit(GPIOA, GPIO_PIN_5);
    g_saved_tv2_state = GPIO_ReadOutputDataBit(GPIOB, GPIO_PIN_8);

    /* Turn off backlight */
    PWM_SetDutyCycle(0);

    /* Turn off non-essential output loads using GPIO library directly */
    GPIO_ResetBits(GPIOA, GPIO_PIN_5);   /* TV1 off */
    GPIO_ResetBits(GPIOB, GPIO_PIN_8);   /* TV2 off */
    //GPIO_ResetBits(GPIOB, GPIO_PIN_0);   /* LP4086_ISET off */

    /* Optional: Disable non-essential clocks here if needed */
    /* RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, DISABLE); etc */

    g_is_sleeping = true;
}

/**
 * @brief Wake up from sleep
 * Restores backlight and peripherals
 */
void SleepWake_WakeUp(void)
{
    if (!g_is_sleeping)
    {
        return;  /* Not sleeping */
    }
    // dma_init();
	// LCD_flash_dma_init();
    // LCD_init();
    /* Restore backlight */
    PWM_SetDutyCycle(g_saved_backlight_brightness);

    /* Restore output loads to previous state using GPIO library directly */
    if (g_saved_tv1_state)
        GPIO_SetBits(GPIOA, GPIO_PIN_5);
    else
        GPIO_ResetBits(GPIOA, GPIO_PIN_5);

    if (g_saved_tv2_state)
        GPIO_SetBits(GPIOB, GPIO_PIN_8);
    else
        GPIO_ResetBits(GPIOB, GPIO_PIN_8);

    /* Restore charging control if it was enabled */
    //GPIO_SetBits(GPIOB, GPIO_PIN_0);

    g_is_sleeping = false;

    /* Reset inactivity timer */
    g_inactivity_counter = 0;
}

/**
 * @brief EXTI2_3 Interrupt handler (MIC_PIN on EXTI3)
 */
void EXTI2_3_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_LINE3) != RESET)
    {
        g_wake_interrupt_triggered = true;
        EXTI_ClrITPendBit(EXTI_LINE3);
    }
}

/**
 * @brief EXTI4_15 Interrupt handler (BUTTON_PIN on EXTI7)
 */
void EXTI4_15_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_LINE7) != RESET)
    {
        g_wake_interrupt_triggered = true;
        EXTI_ClrITPendBit(EXTI_LINE7);
    }
}

/**
 * @brief Initialize LPTIM for periodic wake-up
 */
void SleepWake_InitLPTIM(uint32_t wake_interval_sec)
{
    LPTIM_InitType lptim_init;
    
    g_lptim_wake_interval = wake_interval_sec;
    
    /* Enable LPTIM clock */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_LPTIM, ENABLE);
    
    /* Use LSI (32kHz internal oscillator) as LPTIM clock source */
    RCC_ConfigLPTIMClk(RCC_LPTIMCLK_SRC_LSI);
    
    /* Enable LSI */
    RCC_EnableLsi(ENABLE);
    /* Wait for LSI to be ready - simple delay */
    for (volatile uint32_t i = 0; i < 10000; i++);
    
    /* Configure LPTIM */
    LPTIM_DeInit(LPTIM);
    lptim_init.Prescaler = LPTIM_PRESCALER_DIV32;  /* 32kHz / 32 = 1kHz */
    lptim_init.ClockSource = LPTIM_CLK_SOURCE_INTERNAL;
    lptim_init.Waveform = LPTIM_OUTPUT_WAVEFORM_PWM;
    lptim_init.Polarity = LPTIM_OUTPUT_POLARITY_REGULAR;
    LPTIM_Init(LPTIM, &lptim_init);
    
    /* Set autoreload value for desired wake interval */
    /* 1kHz clock, so for 30 seconds: 1000 Hz * 30 = 30000 */
    uint32_t arr_value = (wake_interval_sec * 1000) - 1;
    if (arr_value > 0xFFFF) arr_value = 0xFFFF;  /* Max 16-bit value */
    
    /* Enable LPTIM */
    LPTIM_Enable(LPTIM);
    
    /* Set autoreload register */
    LPTIM_SetAutoReload(LPTIM, arr_value);
    
    /* Enable LPTIM ARRM interrupt */
    LPTIM_EnableIT_ARRM(LPTIM);
    NVIC_EnableIRQ(LPTIM_TIM6_IRQn);
    NVIC_SetPriority(LPTIM_TIM6_IRQn, 1);
    
    /* Start LPTIM in continuous mode */
    LPTIM_StartCounter(LPTIM, LPTIM_OPERATING_MODE_CONTINUOUS);
}

/**
 * @brief LPTIM interrupt handler
 */
void LPTIM_TIM6_IRQHandler(void)
{
    /* Check if autoreload match interrupt */
    if (LPTIM_IsActiveFlag_ARRM(LPTIM) != RESET)
    {
        g_lptim_wake_triggered = true;
        LPTIM_ClearFLAG_ARRM(LPTIM);
    }
    
    /* Check if compare match interrupt */
    if (LPTIM_IsActiveFlag_CMPM(LPTIM) != RESET)
    {
        g_lptim_wake_triggered = true;
        LPTIM_ClearFLAG_CMPM(LPTIM);
    }
}

/**
 * @brief Check if LPTIM triggered wake
 */
bool SleepWake_IsLPTIMWake(void)
{
    return g_lptim_wake_triggered;
}

/**
 * @brief Check if SWD is connected
 */
bool SleepWake_IsSWDConnected(void)
{
    /* Check if debugger is attached by reading DHCSR register */
    /* Multiple reads to ensure stable connection detection */
    volatile uint32_t *dhcsr = (uint32_t *)0xE000EDF0;
    
    /* Check C_DEBUGEN bit multiple times for stability */
    uint32_t connected_count = 0;
    for (int i = 0; i < 5; i++) {
        if ((*dhcsr & 0x00000001) != 0) {
            connected_count++;
        }
        /* Small delay between checks */
        for (volatile uint32_t j = 0; j < 1000; j++);
    }
    
    /* Require at least 3 out of 5 checks to confirm connection */
    return (connected_count >= 3);
}

/**
 * @brief Enter ultra-low power mode (STOP)
 */
void SleepWake_EnterUltraLowPower(void)
{
    if (g_is_ultra_low_power)
    {
        return;  /* Already in ultra-low power */
    }
    
    //turn off lcd 

    LCD_SendCommand_DMA(LCD_CMD_DISPOFF); // 0x28
    Delay(10);
    LCD_SendCommand_DMA(LCD_CMD_SLPIN); // 0x10
    Delay(10);
    /* Save current power pin states */
    g_saved_lcd_flash_pwr = GPIO_ReadOutputDataBit(LCD_FLASH_PWR_EN_PIN);
    g_saved_lv_cutoff = GPIO_ReadOutputDataBit(LV_CUTOFF_EN_PORT, LV_CUTOFF_EN_PIN);
    g_saved_lp4086_iset = GPIO_ReadOutputDataBit(LP4086_ISET_PORT, LP4086_ISET_PIN);
    g_saved_tv1_state = GPIO_ReadOutputDataBit(TV1_PORT, TV1_PIN);
    g_saved_tv2_state = GPIO_ReadOutputDataBit(TV2_PORT, TV2_PIN);
    g_saved_backlight_brightness = PWM_GetDutyCycle();
    
    /* Turn off power-consuming peripherals */
    GPIO_SetBits(LCD_FLASH_PWR_EN_PIN);  /* LCD Flash power off */
    GPIO_ResetBits(LV_CUTOFF_EN_PORT, LV_CUTOFF_EN_PIN);          /* LV cutoff disable */
    GPIO_ResetBits(LP4086_ISET_PORT, LP4086_ISET_PIN);            /* Keep LP4086 ISET low */
    GPIO_ResetBits(TV1_PORT, TV1_PIN);                            /* TV1 off */
    GPIO_ResetBits(TV2_PORT, TV2_PIN);                            /* TV2 off */
    PWM_SetDutyCycle(0);     
    GPIO_SetBits(LCD_BACKLIGHT_PIN);                                      /* Backlight off */
    
    g_is_ultra_low_power = true;
    g_is_sleeping = true;
    g_lptim_wake_triggered = false;
    
    /* Clear pending wake flags */
    g_wake_interrupt_triggered = false;
    
    /* Enter STOP mode - CPU clock stopped, peripherals can run on LSI */
    /* Wake sources: EXTI (button/mic) and LPTIM */

    Delay(100);
    PWR_EnterSTOPMode(PWR_PDEntry_WFI);
    
    /* ===== EXECUTION RESUMES HERE AFTER WAKE ===== */
    /* System clock is now HSI (8MHz) - must restore before using peripherals */
    SleepWake_RestoreSystemClock();
    
    /* Allow extra time for system to fully stabilize after clock restoration */
    for (volatile uint32_t i = 0; i < 100000; i++);
}

/**
 * @brief Handle wake from ultra-low power mode
 */
bool SleepWake_HandleUltraLowPowerWake(void)
{
    if (!g_is_ultra_low_power)
    {
        return true;  /* Not in ultra-low power, stay awake */
    }
    
    /* Check if woken by external interrupt (button or mic) */
    if (g_wake_interrupt_triggered)
    {
        /* External interrupt - restore full power and stay awake */
        SleepWake_RestoreFullPower();
        g_wake_interrupt_triggered = false;
        g_lptim_wake_triggered = false;
        return true;  /* Stay fully awake */
    }
    
    /* Woken by LPTIM - return to sleep immediately */
    if (g_lptim_wake_triggered)
    {
        g_lptim_wake_triggered = false;
        return false;  /* Return to ultra-low power */
    }
    
    /* Should not reach here, but return to ultra-low power to be safe */
    return false;
}

/**
 * @brief Restore full power mode
 */
void SleepWake_RestoreFullPower(void)
{
    if (!g_is_ultra_low_power)
    {
        return;  /* Not in ultra-low power */
    }
    
    /* Small delay to allow clock to stabilize after restoration */
    for (volatile uint32_t i = 0; i < 10000; i++);
    
    /* Power cycle LCD to fully reset controller state */
    GPIO_ResetBits(GPIOB, GPIO_PIN_4);  /* LCD_FLASH_PWR_EN off */
    for (volatile uint32_t i = 0; i < 100000; i++);  /* Wait for power down */
    GPIO_SetBits(GPIOB, GPIO_PIN_4);    /* LCD_FLASH_PWR_EN on */
    for (volatile uint32_t i = 0; i < 100000; i++);  /* Wait for power up */
    
    /* Force LCD hardware reset before reinit to clear controller state */
    /* LCD may have retained orientation/config registers during sleep */
    GPIO_ResetBits(GPIOB, GPIO_PIN_6);  /* LCD_RST low */
    for (volatile uint32_t i = 0; i < 50000; i++);  /* Hold reset longer */
    GPIO_SetBits(GPIOB, GPIO_PIN_6);    /* LCD_RST high */
    for (volatile uint32_t i = 0; i < 50000; i++);  /* Wait for LCD to stabilize */
    
    /* Restore power pins and reinitialize peripherals */
    setup();
    
    
    

    
    if (g_saved_tv1_state)
        GPIO_SetBits(TV1_PORT, TV1_PIN);
    else
        GPIO_ResetBits(TV1_PORT, TV1_PIN);
    
    if (g_saved_tv2_state)
        GPIO_SetBits(TV2_PORT, TV2_PIN);
    else
        GPIO_ResetBits(TV2_PORT, TV2_PIN);
    


    /* Restore backlight */
    PWM_SetDutyCycle(g_saved_backlight_brightness);
    
    g_is_ultra_low_power = false;
    g_is_sleeping = false;
    
    /* Reset inactivity timer */
    g_inactivity_counter = 0;
}

/**
 * @brief Set debug mode
 */
void SleepWake_SetDebugMode(bool enable)
{
    g_debug_mode = enable;
}

/**
 * @brief Get debug mode status
 */
bool SleepWake_IsDebugMode(void)
{
    return g_debug_mode;
}
