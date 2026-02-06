
#define USE_GRAPHICAL_STATUS 0
#define DEBUG_SLEEP 1
#define BUFFER_SIZE 4096
#define MODEL XD0011




#define TV1_VOLTAGE_SENSE_PIN GPIOA,GPIO_PIN_1
#define TV2_VOLTAGE_SENSE_PIN GPIOA,GPIO_PIN_2
#define MIC_PIN GPIOA,GPIO_PIN_3
#define GPIO_PA4 GPIOA,GPIO_PIN_4
#define TV1_PIN GPIOA,GPIO_PIN_5
#define LCD_BACKLIGHT_PIN GPIOA,GPIO_PIN_6
#define BUTTON_PIN GPIOA,GPIO_PIN_7
#define FLASH_CS_PIN GPIOA,GPIO_PIN_8
#define FLASH_SCLK_PIN GPIOA,GPIO_PIN_9
#define FLASH_MISO_PIN GPIOA,GPIO_PIN_10
#define FLASH_MOSI_PIN GPIOA,GPIO_PIN_11
#define lV_CUTOFF_EN GPIOA,GPIO_PIN_12
#define LCD_SPI_CS_PIN GPIOA,GPIO_PIN_15

#define LP4086_ISET_PIN GPIOB,GPIO_PIN_0
#define LP4086_CHRG_PIN GPIOB,GPIO_PIN_1
#define LV_CUTOFF_FEEDBACK GPIOB,GPIO_PIN_2
#define LCD_SCLK_PIN GPIOB,GPIO_PIN_3
#define LCD_FLASH_PWR_EN_PIN GPIOB,GPIO_PIN_4
#define LCD_MOSI_PIN GPIOB,GPIO_PIN_5
#define LCD_RST_PIN GPIOB,GPIO_PIN_6
#define LCD_DC_PIN GPIOB,GPIO_PIN_7
#define TV2_PIN GPIOB,GPIO_PIN_8

#define FSLP_PIN GPIOC,GPIO_PIN_14
#define VPSW_PIN GPIOC,GPIO_PIN_15	



#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include "n32g031_rcc.h"
#include "n32g031_rtc.h"
#include "nv3029.h"
#include "spi_flash.h"
#include "dma.h"
#include "pwm.h"
#include "sleep_wake.h"
#include "voltage_monitor.h"
#include "button.h"
#include "spi_flash_sfdp.h"

enum StatusValues {
	STATUS_IDLE = 0,
	STATUS_READY = 1,
	STATUS_STARTING = 2,
	STATUS_LEVEL_RESET = 3,
	STATUS_START_WRITE = 4,
	STATUS_WRITE_DONE = 5,
	STATUS_START_READ = 6,
	STATUS_READ_DONE = 7,
	STATUS_OPERATION_COMPLETE = 8,
    STATUS_ERROR = 9
};


 uint8_t mainran = 0;

/* Global UTC timestamp - updated from RTC */
uint32_t current_utc = 0;

volatile int page[1];
volatile int lvl_reset_flag[1];
volatile int continue_flag[1];
volatile int write_flag[1];
volatile int status[1];
volatile bool FlashID_error = false;
uint8_t lvl_buffer[5];
uint8_t lvl_buffer_read[5];
uint8_t buffer[BUFFER_SIZE] = {0};
uint32_t FlashID = 0;
uint32_t status_register;

SFDP_Params_t sfdp;

/*
GPIO

PORT & PIN      MODE	SETTINGS	FUNCTION
GPIOA    
0 			    INPUT               A1 NC ?
1 			    ANALOG  HSR         TV1_VOLTAGE_SENSE        
2               ANALOG  HSR         TV2_VOLTAGE_SENSE
3			    INPUT   PULLDOWN?   MIC ?
4			    OUTPUT  PP,Low?     A4 NC ?
5			    OUTPUT  PP,LOW      ELEMENT TV2
6				OUTPUT  PP,LOW      LCD_BACKLIGHT
7			    INPUT   PULLUP      BUTTON
8               OUTPUT  PP,HIGH     FLASH_CS
9               OUTPUT  PP,LOW      SPI2_SCLK, FLASH_SPI_SCLK
10 			    OUTPUT  PP,LOW      SPI2_MISO, FLASH_SPI_MISO
11 			    INPUT   PULLUP      SPI2_MOSI, FLASH_SPI_MOSI
12              OUTPUT  PP,HIGH     Low Voltage Cutoff Enable
13				INPUT   PULLUP      SWCLK
14              INPUT   PULLDOWN    SWDIO
15              OUTPUT  PP,HIGH     LCD_SPI_CS

GPIOB
0	            OUTPUT  PP,LOW      LP4086_ISET
1               INPUT   PULLUP      LP4086_CHRG (PULLED LOW ACTIVE) ext1 interupt
2			    INPUT   PULLUP      LV_CUTOFF_FEEDBACK
3               OUTPUT  PP,LOW      SPI1_SCLK, LCD_SPI_SCLK
4               OUTPUT  OD,LOW      LCD_FLASH_PWR_EN 
5			    OUTPUT  PP,LOW      SPI1_MOSI, LCD_SPI_MOSI
6 			    OUTPUT  PP,HIGH     LCD_RST
7  		        OUTPUT  PP,HIGH     LCD_DC
8				OUTPUT  PP,LOW      ELEMENT TV1

GPIOC
13              ANALOG  HSR         UNKNOWN_ANALOG
14 			    INPUT 			    FSLP     
15              INPUT               VPSW

GPIOF
0              ANALOG  HSR         UNKOWN_ANALOG
1              ANALOG  HSR         UNKOWN_ANALOG
2              ANALOG  HSR         UNKOWN_ANALOG
6              ANALOG  HSR         UNKOWN_ANALOG
7              ANALOG  HSR         UNKOWN_ANALOG
*/

/**
 * @brief Initialize all hardware peripherals and subsystems
 * Configures GPIOs, DMA, LCD, SPI flash, PWM backlight, voltage monitoring,
 * button timing, and sleep/wake system.
 */
/**
 * @brief Initialize RTC peripheral
 * Checks if RTC is already initialized, configures clock source (LSE), and sets up RTC prescalers.
 * After initialization, reads current UTC time from RTC.
 */
void rtcSetup(void) {
	RTC_InitType RTC_InitStructure;
	RTC_TimeType RTC_TimeStructure;
	RTC_DateType RTC_DateStructure;
	uint32_t timeout = 0;
	uint32_t rtcClockSource;

	/* Check if RTC clock is already configured (don't touch it if already running) */
	/* Reference: N32G031 RCC driver - RCC_GetRTCClkSrc() reads RCC->LSCTRL bits [9:8] */
	rtcClockSource = RCC_GetRTCClkSrc();
	
	/* CRITICAL: Shadow bypass MUST be enabled BEFORE RTC_ConfigCalendar() to avoid DMA interference */
	/* Reference: n32g031_rtc.c line 663 - RTC_ConfigCalendar() internally calls RTC_WaitForSynchro() */
	/* Reference: n32g031_rtc.c line 323 - RTC_WaitForSynchro() blocks up to 5.5 seconds with delay_ms() */
	/* Reference: n32g031_rtc.c line 294 - delay_ms() is a tight busy-wait loop that blocks ALL interrupts/DMA */
	/* Solution: Enable bypass shadow BEFORE any RTC register writes to skip sync waits entirely */
	RTC_EnableBypassShadow(ENABLE);
	
	if (rtcClockSource == RCC_RTCCLK_SRC_NONE) {
		/* RTC not initialized - configure it */
		/* This only happens on first boot or after power loss */
		
		/* Temporarily enable PWR clock to access backup domain */
		/* Reference: N32G031 datasheet - PWR peripheral on APB1 bus controls backup domain access */
		/* IMPORTANT: Must disable this clock after init to avoid APB1/AHB bus arbitration conflicts with DMA */
		RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);

		
		/* Reference: N32G031 datasheet - LSI is less accurate but always available */
		RCC_EnableLsi(ENABLE);
		while (RCC_GetFlagStatus(RCC_LSCTRL_FLAG_LSIRD) == RESET);  /* Wait for LSI ready */
		RCC_ConfigRtcClk(RCC_RTCCLK_SRC_LSI);
			
	

		/* Enable RTC clock */
		/* Reference: N32G031 RCC driver - RCC_EnableRtcClk() sets LSCTRL bit 15 (RTC clock enable) */
		RCC_EnableRtcClk(ENABLE);

		/* Brief delay for RTC peripheral to stabilize after clock enable */
		/* ~2ms delay at 48MHz (100000 iterations) - enough for hardware to settle */
		for (volatile uint32_t i = 0; i < 100000; i++);

		/* Disable write protection to allow RTC register modifications */
		/* Reference: n32g031_rtc.c - RTC registers are write-protected by default for safety */
		/* Must write magic values 0xCA, 0x53 to RTC->WRP to unlock (done inside function) */
		RTC_EnableWriteProtection(DISABLE);

		/* Enter initialization mode (required to write calendar/prescaler registers) */
		/* Reference: n32g031_rtc.c line 241 - RTC_EnterInitMode() implementation */
		/* WARNING: Contains unbounded while(RTC->SUBS <=3) loop at line 252! */
		/* SUBS register must be >3 to ensure safe register access (hardware requirement) */
		if (RTC_EnterInitMode() == ERROR) {
			/* Failed to enter init mode - clean up and abort */
			RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, DISABLE);
			return;
		}

		/* Configure RTC prescaler to generate 1Hz tick from 32.768kHz LSE */
		/* Reference: N32G031 RTC documentation - ck_spre(1Hz) = RTCCLK / ((AsynchPrediv + 1) * (SynchPrediv + 1)) */
		/* Calculation: 32768 Hz / (128 * 256) = 32768 / 32768 = 1 Hz */
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;  /* 127 + 1 = 128 */
		RTC_InitStructure.RTC_SynchPrediv = 0xFF;   /* 255 + 1 = 256 */
		RTC_InitStructure.RTC_HourFormat = RTC_24HOUR_FORMAT;

		/* Set default date and time */
		/* Default: 2026-01-01 00:00:00 (Wednesday) */
		/* Reference: RTC stores year as 0-99 (last 2 digits), month 1-12, date 1-31, weekday 1-7 */
		RTC_TimeStructure.Hours = 0;
		RTC_TimeStructure.Minutes = 0;
		RTC_TimeStructure.Seconds = 0;
		RTC_TimeStructure.H12 = RTC_AM_H12;  /* Ignored in 24-hour format */

		RTC_DateStructure.Year = 26;   /* 2026 (stored as 26 in RTC) */
		RTC_DateStructure.Month = 1;   /* January */
		RTC_DateStructure.Date = 1;    /* 1st */
		RTC_DateStructure.WeekDay = 3; /* Wednesday (1=Monday, 7=Sunday) */

		/* Configure calendar with date and time */
		/* Reference: n32g031_rtc.c line 474 - RTC_ConfigCalendar() implementation */
		/* CRITICAL ISSUE FOUND: This function calls RTC_ExitInitMode() at line 660 */
		/* Then at line 663 it checks if shadow bypass is enabled (RTC_CTRL_BYPS bit) */
		/* If bypass NOT enabled, it calls RTC_WaitForSynchro() which blocks 5.5 seconds! */
		/* SOLUTION: Shadow bypass was enabled at line 163 BEFORE entering this block */
		/* This ensures RTC_ConfigCalendar skips the sync wait at line 663-669 */
		/* DISABLE parameter = no 1.1s pre-delay (RTC_Delay_Flag wait at line 623) */
		RTC_ConfigCalendar(RTC_FORMAT_BIN, &RTC_InitStructure, &RTC_DateStructure, 
		                   &RTC_TimeStructure, DISABLE);

		/* NOTE: RTC_ConfigCalendar() already called RTC_ExitInitMode() internally at line 660 */
		/* So calendar is now counting. No need to call RTC_ExitInitMode() again. */

		/* Enable write protection to prevent accidental RTC register modifications */
		/* Reference: n32g031_rtc.c - Write protection is a safety feature */
		RTC_EnableWriteProtection(ENABLE);
		
		/* Disable PWR clock immediately after initialization complete */
		/* Reference: dma.c line 12 - DMA is on AHB bus, PWR is on APB1 bus */
		/* Having PWR clock enabled causes bus arbitration conflicts with DMA transfers */
		/* This was the root cause of LCD DMA transfer failures! */
		RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, DISABLE);
	}

	/* Shadow bypass remains enabled for all subsequent RTC reads */
	/* This allows rtcGetUTC() to read RTC registers directly without sync delays */
	/* Reference: n32g031_rtc.h - RTC_CTRL_BYPS bit bypasses shadow register synchronization */

	/* Initial time read (no sync wait needed with bypass enabled) */
	/* Reference: n32g031_rtc.c - RTC_GetTime/GetDate touch registers to lock values atomically */
	RTC_GetTime(RTC_FORMAT_BIN, &RTC_TimeStructure);
	RTC_GetDate(RTC_FORMAT_BIN, &RTC_DateStructure);

	/* Optionally log the current time (if you have UART or other logging) */
	/* printf("RTC Initialized - Date: %02d/%02d/20%02d Time: %02d:%02d:%02d\n",
	        RTC_DateStructure.Date, RTC_DateStructure.Month, RTC_DateStructure.Year,
	        RTC_TimeStructure.Hours, RTC_TimeStructure.Minutes, RTC_TimeStructure.Seconds); */

	/* Uncomment below function to set RTC from UTC timestamp */
	// rtcSetFromUTC(1737936000);  /* Example: January 27, 2026 00:00:00 UTC */
}

/**
 * @brief Get current UTC timestamp from RTC
 * Reads RTC registers and converts date/time to Unix timestamp.
 * Updates the global current_utc variable.
 * @return Current UTC timestamp (seconds since January 1, 1970 00:00:00 UTC)
 */
uint32_t rtcGetUTC(void) {
	RTC_TimeType RTC_TimeStructure;
	RTC_DateType RTC_DateStructure;
	uint32_t year, days = 0;
	uint32_t i;
	const uint32_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	/* Read current time and date from RTC */
	RTC_GetTime(RTC_FORMAT_BIN, &RTC_TimeStructure);
	RTC_GetDate(RTC_FORMAT_BIN, &RTC_DateStructure);

	/* Calculate full year (RTC stores only last 2 digits) */
	year = 2000 + RTC_DateStructure.Year;

	/* Calculate days since Unix epoch (1970-01-01) */
	/* Count leap years and regular years */
	for (i = 1970; i < year; i++) {
		if ((i % 4 == 0 && i % 100 != 0) || (i % 400 == 0)) {
			days += 366;  /* Leap year */
		} else {
			days += 365;  /* Regular year */
		}
	}

	/* Add days for months in current year */
	for (i = 1; i < RTC_DateStructure.Month; i++) {
		days += daysInMonth[i - 1];
		/* Add leap day if February and current year is leap year */
		if (i == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
			days += 1;
		}
	}

	/* Add days in current month */
	days += RTC_DateStructure.Date - 1;

	/* Convert to timestamp */
	current_utc = (days * 86400) + 
	              (RTC_TimeStructure.Hours * 3600) + 
	              (RTC_TimeStructure.Minutes * 60) + 
	              RTC_TimeStructure.Seconds;

	return current_utc;
}

/* 
 * @brief Set RTC time and date from Unix UTC timestamp
 * @param utcTimestamp Unix timestamp (seconds since January 1, 1970 00:00:00 UTC)
 * 
 * Example usage:
 *   rtcSetFromUTC(1737936000);  // Sets RTC to January 27, 2026 00:00:00 UTC
 */
/*
void rtcSetFromUTC(uint32_t utcTimestamp) {
	RTC_TimeType RTC_TimeStructure;
	RTC_DateType RTC_DateStructure;
	uint32_t days, seconds;
	uint32_t year = 1970, month = 1, date = 1;
	uint32_t hours, minutes, secs;
	const uint32_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	uint32_t weekday;

	// Calculate days and remaining seconds
	days = utcTimestamp / 86400;
	seconds = utcTimestamp % 86400;

	// Calculate time components
	hours = seconds / 3600;
	minutes = (seconds % 3600) / 60;
	secs = seconds % 60;

	// Calculate date components
	while (1) {
		uint32_t daysInYear = 365;
		if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
			daysInYear = 366;  // Leap year
		}

		if (days >= daysInYear) {
			days -= daysInYear;
			year++;
		} else {
			break;
		}
	}

	// Calculate month and date
	for (month = 1; month <= 12; month++) {
		uint32_t daysThisMonth = daysInMonth[month - 1];
		if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
			daysThisMonth = 29;  // February in leap year
		}

		if (days >= daysThisMonth) {
			days -= daysThisMonth;
		} else {
			break;
		}
	}
	date = days + 1;

	// Calculate day of week (Zeller's congruence simplified)
	// January 1, 1970 was Thursday (4)
	weekday = ((utcTimestamp / 86400) + 4) % 7;
	if (weekday == 0) weekday = 7;  // RTC uses 1=Monday...7=Sunday

	// Disable write protection
	RTC_EnableWriteProtection(DISABLE);
	
	// Enter initialization mode
	if (RTC_EnterInitMode() == ERROR) {
		RTC_EnableWriteProtection(ENABLE);
		return;  // Failed to enter init mode
	}

	// Set time
	RTC_TimeStructure.Hours = hours;
	RTC_TimeStructure.Minutes = minutes;
	RTC_TimeStructure.Seconds = secs;
	RTC_TimeStructure.H12 = RTC_AM_H12;

	// Set date (RTC year is 0-99, storing last 2 digits)
	RTC_DateStructure.Year = year % 100;
	RTC_DateStructure.Month = month;
	RTC_DateStructure.Date = date;
	RTC_DateStructure.WeekDay = weekday;
	
	// Configure calendar
	RTC_InitType RTC_InitStructure;
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	RTC_InitStructure.RTC_SynchPrediv = 0xFF;
	RTC_InitStructure.RTC_HourFormat = RTC_24HOUR_FORMAT;
	RTC_ConfigCalendar(RTC_FORMAT_BIN, &RTC_InitStructure, &RTC_DateStructure, 
	                   &RTC_TimeStructure, DISABLE);
	
	// Exit initialization mode
	RTC_ExitInitMode();
	
	// Enable write protection
	RTC_EnableWriteProtection(ENABLE);
}
*/

void setup(void){

 RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_GPIOC, ENABLE);
    RCC_ConfigAdcHclk(RCC_ADCHCLK_DIV6);
	RCC_EnableAHBPeriphClk(RCC_AHBPCLKEN_ADCEN, ENABLE);

	/* Initialize voltage monitoring on analog inputs */
	VoltageMonitor_Init();
    

	

{ // gpio init

	//spare
	GPIO_Init(FSLP_PIN, GPIO_MODE_INPUT);
	GPIO_Init(VPSW_PIN, GPIO_MODE_INPUT);

    // Conditional Hardwired Pause when VPSW is LOW
    if (GPIO_ReadInputDataBit(VPSW_PIN) == RESET) {
		while (GPIO_ReadInputDataBit(VPSW_PIN) == RESET) {
			// Wait here until VPSW goes HIGH
			Delay(100);  // Small delay to avoid busy-waiting
		}
	}


	// outputs

	GPIO_Init(LCD_FLASH_PWR_EN_PIN, GPIO_MODE_OUTPUT_PP);
	GPIO_Off(LCD_FLASH_PWR_EN_PIN);

	GPIO_Init(lV_CUTOFF_EN, GPIO_MODE_OUTPUT_PP);
	GPIO_On( lV_CUTOFF_EN);

    GPIO_Init(TV1_PIN, GPIO_MODE_OUTPUT_PP);
	GPIO_Off(TV1_PIN);

	GPIO_Init(TV2_PIN, GPIO_MODE_OUTPUT_PP);
	GPIO_Off(TV2_PIN);


	GPIO_Init(LP4086_ISET_PIN, GPIO_MODE_OUTPUT_PP);
	GPIO_Off(LP4086_ISET_PIN);
    // leave above on to enable charging


	// inputs

	GPIO_Init(BUTTON_PIN, GPIO_MODE_INPUT);
    GPIO_Init(LP4086_CHRG_PIN, GPIO_MODE_INPUT);
	GPIO_Init(MIC_PIN, GPIO_MODE_INPUT);
    GPIO_Init(LV_CUTOFF_FEEDBACK, GPIO_MODE_INPUT);

    // ANALOG

	GPIO_Init(TV1_VOLTAGE_SENSE_PIN, GPIO_MODE_ANALOG);
	GPIO_Init(TV2_VOLTAGE_SENSE_PIN, GPIO_MODE_ANALOG);
    //GPIO_Init(LV_CUTOFF_FEEDBACK, GPIO_MODE_ANALOG);



    // Not connected?
	GPIO_Init(GPIO_PA4, GPIO_MODE_OUTPUT_PP);
	GPIO_Off(GPIO_PA4);

	//spare
	GPIO_Init(FSLP_PIN, GPIO_MODE_INPUT);
	GPIO_Init(VPSW_PIN, GPIO_MODE_INPUT);

}
	/* Initialize and configure RTC */
	//rtcSetup();
	//rtcGetUTC();
	dma_init();
	LCD_flash_dma_init();
	LCD_init();
	/* Initialize PWM for LCD backlight (PA6) - 500Hz, 85% duty cycle */
	PWM_Init(20000);
    PWM_SetDutyCycle(50);  /* Set initial brightness */

	sFLASH_Init();



	/* Initialize button timing system */
	Button_Init();

	SleepWake_Init(10);

	

}

/**
 * @brief Main application screen loop
 * Handles button input, sleep/wake logic, voltage monitoring, and LCD display updates.
 * Supports short/long/very long button press detection and ultra-low power sleep mode.
 */
void mainScreen(void){
/* Variables for button debouncing and activity tracking */
	static uint8_t button_prev_state = 0;
	static uint32_t activity_timer = 0;
	static bool use_ultra_low_power = false;  /* Enable ultra-low power mode */
	bool debugSleep = DEBUG_SLEEP;
	
    bool micInput = false;
	bool lvInput = false;	
	bool isCharging = false;
	bool buttonPressed = false;

	displayPhoto(0x0A000, 0x0BE0);
    displayFullPhotoChunked(0x0);
	Delay(5000);
	LCD_fill_screen(COLOR_BLACK);
	displayPhoto(0x0E740, 0x0BE0);


	if (debugSleep)
	{
		 SleepWake_SetDebugMode(true);
	}
	
	

	while(mainran == 1){

	/* Update button state machine */
	Button_Update();
	
	// /* Check button for wake-up or activity */
	 	uint8_t button_cur_state = GPIO_ReadInputDataBit(BUTTON_PIN);
		if (button_cur_state && !button_prev_state)
			button_prev_state = button_cur_state;

		buttonPressed = !button_cur_state;
		
		/* Check for button press types */
		ButtonState_t button_action = Button_GetLastAction();
		if (button_action != BUTTON_STATE_NONE)
		{
			switch (button_action)
			{
				case BUTTON_STATE_SHORT_PRESS:
					/* Handle short press - example: toggle something */
					// Your code here
					break;
					
				case BUTTON_STATE_LONG_PRESS:
					/* Handle long press - example: enter menu */
					// Your code here
					break;
					
				case BUTTON_STATE_VERY_LONG_PRESS:
					/* Handle very long press - example: factory reset */
					// Your code here
					break;
					
				default:
					break;
			}
			
			/* Clear the action after handling */
			Button_ClearAction();
		}
		
		{																	// sleep-wake logic

		/* Check for external interrupt wake-up (button or mic) */
		if (SleepWake_IsWakeInterruptTriggered())
		{
			if (SleepWake_IsSleeping()){									// Wake up from sleep via interrupt 
				SleepWake_WakeUp();
				
				/* Check if button is still pressed after wakeup */
				if (Button_IsPressed())
				{
					Button_SetPressedOnWakeup();
				}
			}                                                               // end is sleeping
			/* Reset inactivity timer on interrupt wake */
			SleepWake_ResetTimer();
			/* Clear the interrupt flag */
			SleepWake_ClearWakeInterrupt();
			
		}

									
		if (SleepWake_CheckTimeout()){										// Check if timeout for sleep has expired 
			if (use_ultra_low_power){										// Enter ultra-low power mode
				while (1){
				
					SleepWake_EnterUltraLowPower();
					if (SleepWake_HandleUltraLowPowerWake()){				// Handle wake from ultra-low power 
						/* Stay awake - restore display and continue main loop */
						displayFullPhotoChunked(0x0);
						Delay(1000);
						LCD_fill_screen(COLOR_BLACK);
						break;  /* Exit ultra-low power loop */
					}														// end handle wake
				}															// end ultra-low power loop
			}else{
				/* Go to normal sleep */
				SleepWake_GoToSleep();
				}
			}																// end check sleep timeout

		}																	//end sleep-wake logic

		uint32_t press_duration = Button_GetPressDuration();
		
		VoltageMonitor_UpdateReadings();									// Update voltage readings periodically
																			// Read inputs
        micInput = GPIO_ReadInputDataBit(MIC_PIN);
	    lvInput = GPIO_ReadInputDataBit(LV_CUTOFF_FEEDBACK);
		isCharging = GPIO_ReadInputDataBit(LP4086_CHRG_PIN);
		
		LCD_draw_string(0, 120, micInput ? "MIC IN" : "NO MIC", COLOR_YELLOW, COLOR_BLACK, 2);
		LCD_draw_string(0, 140, lvInput ? "LV IN" : "NO LV", COLOR_YELLOW, COLOR_BLACK, 2);
		LCD_draw_string(70, 140, "CHRG" , isCharging ? COLOR_RED : COLOR_GREEN, COLOR_BLACK, 2);
		LCD_FillRect(126, 0, 2, 160, button_cur_state ? COLOR_BLACK : COLOR_BLUE); // clear voltage area

		//SleepWake_GetTimeRemaining
		// draw voltage values at x=0, and y =0, y=20, y=40 etc...
		char voltage_str[20];
		uint16_t tv1_voltage = g_voltage_readings.pa1_tv1_sense ;
		uint16_t tv2_voltage = g_voltage_readings.pa2_tv2_sense ;
		uint16_t pa0_voltage = g_voltage_readings.pa0_analog ;
		uint16_t temp_celsius = g_voltage_readings.temp_sensor ;
		uint16_t pb2_voltage = g_voltage_readings.pb2_analog ;
		uint16_t ch3_voltage = g_voltage_readings.ch3_unused ;
		
		snprintf(voltage_str, sizeof(voltage_str), "TV1: %.4i mV", tv1_voltage);
		LCD_draw_string(0, 0, voltage_str, COLOR_CYAN, COLOR_BLACK, 1);
		snprintf(voltage_str, sizeof(voltage_str), "TV2: %.4i mV", tv2_voltage);
		LCD_draw_string(0, 10, voltage_str, COLOR_CYAN, COLOR_BLACK, 1);
		snprintf(voltage_str, sizeof(voltage_str), "PA0: %.4i mV", pa0_voltage);
		LCD_draw_string(0, 20, voltage_str, COLOR_CYAN, COLOR_BLACK, 1);
		snprintf(voltage_str, sizeof(voltage_str), "TMP: %.4i mC", temp_celsius);
		LCD_draw_string(0, 30, voltage_str, COLOR_CYAN, COLOR_BLACK, 1);
		snprintf(voltage_str, sizeof(voltage_str), "PB2: %.4i mV", pb2_voltage);
		LCD_draw_string(0, 40, voltage_str, COLOR_CYAN, COLOR_BLACK, 1);
		snprintf(voltage_str, sizeof(voltage_str), "CH3: %.4i mV", ch3_voltage);
		LCD_draw_string(0, 500, voltage_str, COLOR_CYAN, COLOR_BLACK, 1);
		displayPhoto(0x0E740, 0x0BE0);
		
		char screen_type_str[20];
		snprintf(screen_type_str, sizeof(screen_type_str), "Scrn id: %s-%s", spi_rx_data_04, spi_rx_data_d3);
		LCD_draw_string(0, 50, screen_type_str, COLOR_YELLOW, COLOR_BLACK, 1);




		char timeRemainingStr[8];
		uint32_t timeRemaining = SleepWake_GetTimeRemaining();
		snprintf(timeRemainingStr, sizeof(timeRemainingStr), "%.2u", timeRemaining);
		LCD_draw_string(80, 120, timeRemainingStr, COLOR_YELLOW, COLOR_BLACK, 2);

		/* Display button timing info */
		char button_str[20];
		snprintf(button_str, sizeof(button_str), "BTN: %u ms", (unsigned int)press_duration);
		LCD_draw_string(0, 60, button_str, COLOR_CYAN, COLOR_BLACK, 1);
		
		if (Button_WasPressedOnWakeup())
		{
			LCD_draw_string(0, 70, "HELD@WAKE", COLOR_RED, COLOR_BLACK, 1);
		}

		


		Delay(200);  /* 100ms delay in main loop */

	}
}

/**
 * @brief Flash memory programming and verification screen
 * Handles bulk erase, page-by-page write, and page-by-page read operations.
 * Displays progress on LCD if USE_GRAPHICAL_STATUS is enabled.
 */
void flashScreen(void){

		// Check if SFDP is supported
if (SFDP_IsSupported())
{
    // Read and parse SFDP
    if (SFDP_ReadAndParse(&sfdp))
    {
        // Get flash size
        uint32_t size_mb = SFDP_GetDensityMegabits(&sfdp);
        
        // Get optimal parameters
        uint16_t page_size = SFDP_GetPageSize(&sfdp);
        uint32_t sector_size = SFDP_GetSectorSize(&sfdp);
        uint8_t erase_cmd = SFDP_GetSectorEraseOpcode(&sfdp);
        
        // Check capabilities
        if (SFDP_SupportsFastRead(&sfdp))
        {
            // Enable fast read mode
        }
    }
}
	
		status[0] = 1;
	bool use_graphical_status = USE_GRAPHICAL_STATUS;
	/* Original flash check loop */
	// Check Flash ID
	if(FlashID == sFLASH_W25Q128_ID || FlashID == sFLASH_M25P64_ID || FlashID == sFLASH_GD25Q80_ID || FlashID == sFLASH_PD32S_ID || FlashID == sFlash_unk_ID){     
		
		// Wait for continue flag
		while(continue_flag[0] == 0);
		continue_flag[0] = 0;
		
		status[0] = STATUS_READY;
		continue_flag[0] = 0;
		status[0] = STATUS_READY;
		
		// If level reset flag is set, execute the level reset
		if(lvl_reset_flag[0] == 1){
			lvl_buffer[4] = 0xBB;
			sFLASH_EraseSector(0xf8000);
			sFLASH_WriteBuffer(lvl_buffer, 0xf8000, 5);
			status[0] = STATUS_LEVEL_RESET;
			
		}

		sFLASH_ReadBuffer(lvl_buffer_read, 0xf8000, 5);
		
		volatile int tmp_page = -1;
		
		// If the write flag is set, initialize write
		if(write_flag[0] == 1){
			write_flag[0] = 0;
			sFLASH_EraseBulk();
			Delay(10);

			status[0] = STATUS_WRITE_DONE;
			if(use_graphical_status){
				LCD_fill_screen(COLOR_BLACK);
				LCD_draw_string(0, 0, "Writing- ", COLOR_WHITE, COLOR_BLACK, 2);
				LCD_draw_string(0, 20, "Block#: ", COLOR_WHITE, COLOR_BLACK, 2);
				Delay(10);
			}

			for(uint32_t i=0; i<256; i++){
				while(status[0] == 5) ;
				uint32_t addr = i * BUFFER_SIZE;
				LCD_flash_write_async(addr, BUFFER_SIZE, buffer);
				
				if(use_graphical_status){
					
					char block_str[10];
					snprintf(block_str, sizeof(block_str), "%u", i);
					LCD_draw_string(72, 20, block_str, COLOR_WHITE, COLOR_BLACK, 2);
				}
				while(!LCD_is_flash_write_complete());

				//sFLASH_WriteBuffer(buffer, addr, BUFFER_SIZE);
				status[0] = STATUS_WRITE_DONE;
			}
			
		}else{ //Reaxing Flash page by page
			if(use_graphical_status){	
				LCD_fill_screen(COLOR_BLACK);
				LCD_draw_string(0, 0, "Reading- ", COLOR_WHITE, COLOR_BLACK, 2);
				LCD_draw_string(0, 20, "Block#: ", COLOR_WHITE, COLOR_BLACK, 2);
				Delay(10);
			}

			while(status[0] != STATUS_OPERATION_COMPLETE){
				if(tmp_page != page[0]){
					status[0] = STATUS_START_READ;
					tmp_page = page[0];
					uint32_t addr = page[0] * BUFFER_SIZE;
					if(use_graphical_status){	
						char block_str[10];
						snprintf(block_str, sizeof(block_str), "%d", page[0]);
						LCD_draw_string(72, 20, block_str, COLOR_WHITE, COLOR_BLACK, 2);
					}

					LCD_flash_read_async(addr, BUFFER_SIZE, buffer, BUFFER_SIZE);
					/* Wait for DMA read to complete */
					while(!LCD_is_flash_read_complete());
		
					//sFLASH_ReadBuffer(buffer, addr, BUFFER_SIZE);
					status[0] = STATUS_READ_DONE;
				}
			}
		}
		if(use_graphical_status){	
			LCD_fill_screen(COLOR_BLACK);
			LCD_draw_string(0, 0, "Done !", COLOR_WHITE, COLOR_BLACK, 2);
		}
		status[0] = STATUS_OPERATION_COMPLETE; //operation complete
	}else{
		// If there is an error reading the Flash ID, pulse the light 3 times for 1 second delay
		PWM_SetDutyCycle(50); 
		Delay(1000);
		PWM_SetDutyCycle(0); 
		Delay(1000);
		PWM_SetDutyCycle(50); 
		Delay(1000);
		PWM_SetDutyCycle(0); 
		Delay(1000);
		PWM_SetDutyCycle(50); 
		Delay(1000);
		PWM_SetDutyCycle(0); 
		Delay(1000);
		PWM_SetDutyCycle(50); 
	    FlashID_error = true;
        status[0] = STATUS_ERROR;
		//while(1)
		mainran = 1;
	}

	/* Infinite loop after flash operations */
	while (1)
	{
		/* Wait indefinitely */
	}
	


}

/**
 * @brief Main program entry point
 * Initializes hardware, verifies flash ID, and enters mainScreen() loop.
 * @return Never returns (infinite loop)
 */
int main(void)
{
	mainran = 3;
	setup();

	while(1){
		Delay(100);
		FlashID = sFLASH_ReadID();
		if(FlashID_error == true) break;
		if(FlashID != 0) break; else FlashID_error = true;
		}

	if( mainran == 0){
		mainran = 1;
		mainScreen();
		}

	while(mainran == 3) {
		{
		uint32_t anim_addr = 0x124710;
		LCD_Window lcdAnimWindow = {0, 0, 128 - 1, 160 - 1};
		LCD_animation(lcdAnimWindow, 52, anim_addr);
		}
		
		//Delay(1000);
		}//end while mainran 1

			

	
	while(mainran == 2 && !FlashID_error) {
		flashScreen();
		}//end while mainran 2

	while(1)
	{
		/* Main infinite loop */
	}

} // end main

/**
 * @brief Simple delay function
 * @param count Delay count (actual delay is count * 0x500 iterations)
 * Not precise - for accurate timing use hardware timers
 */
void Delay(volatile uint32_t count)
{
	volatile uint32_t t_delay = count * 0x500;
	for (; t_delay >0; t_delay--);
}

/**
 * @brief Turn off a GPIO pin (set to LOW)
 * @param GPIOx GPIO port (GPIOA, GPIOB, etc.)
 * @param Pin GPIO pin number
 */
void GPIO_Off(GPIO_Module *GPIOx, uint16_t Pin) {  GPIO_ResetBits(GPIOx, Pin); }

/**
 * @brief Turn on a GPIO pin (set to HIGH)
 * @param GPIOx GPIO port (GPIOA, GPIOB, etc.)
 * @param Pin GPIO pin number
 */
void GPIO_On(GPIO_Module* GPIOx, uint16_t Pin) { GPIO_SetBits(GPIOx, Pin); }

/**
 * @brief Initialize a GPIO pin with specified mode
 * Enables the GPIO clock and configures the pin. Input pins are set with pull-up by default.
 * @param GPIOx GPIO port (GPIOA, GPIOB, GPIOC, or GPIOF)
 * @param Pin GPIO pin number (GPIO_PIN_0 through GPIO_PIN_15)
 * @param GpioMode GPIO mode (GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP, GPIO_MODE_ANALOG, etc.)
 */
void GPIO_Init(GPIO_Module* GPIOx, uint16_t Pin, uint32_t GpioMode) {
    GPIO_InitType GPIO_InitStructure;

    /* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

    /* Enable the GPIO Clock */
    if (GPIOx == GPIOA)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    }
    else if (GPIOx == GPIOB)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
    }
    else if (GPIOx == GPIOC)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
    }
    else if (GPIOx == GPIOF)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOF, ENABLE);
    }
    else
    {
        return;
    }

    /* Configure the GPIO pin */
    if (Pin <= GPIO_PIN_ALL)
    {
        GPIO_InitStruct(&GPIO_InitStructure);
        GPIO_InitStructure.Pin = Pin;
        GPIO_InitStructure.GPIO_Mode = GpioMode;
		if(GpioMode == GPIO_MODE_INPUT){
			GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
		}
        GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
    }
}


/**
 * @brief Assert failed handler
 * Called when an assertion fails in debug builds (USE_FULL_ASSERT defined).
 * Enters an infinite loop to halt execution for debugging.
 * @param expr The expression that failed
 * @param file The source file name where assertion failed
 * @param line The line number where assertion failed
 */
#ifdef USE_FULL_ASSERT
void assert_failed(const uint8_t* expr, const uint8_t* file, uint32_t line)
{
    while (1)
    {
    }
}
#endif // USE_FULL_ASSERT

/**
 * @}
 */
