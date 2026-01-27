
#define USE_GRAPHICAL_STATUS 0
#define DEBUG_SLEEP 1
#define BUFFER_SIZE 4096




#define TV1_VOLTAGE_SENSE_PIN GPIOA,GPIO_PIN_1

#define TV2_VOLTAGE_SENSE_PIN GPIOA,GPIO_PIN_2

#define MIC_PIN GPIOA,GPIO_PIN_3

#define GPIO_PA4 GPIOA,GPIO_PIN_4

#define TV1_PIN GPIOA,GPIO_PIN_5

#define LCD_BACKLIGHT_PIN GPIOA,GPIO_PIN_6

#define BUTTON_PIN GPIOA,GPIO_PIN_7

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



#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include "n32g031_rcc.h"
#include "nv3029.h"
#include "spi_flash.h"
#include "dma.h"
#include "pwm.h"
#include "sleep_wake.h"
#include "voltage_monitor.h"
#include "iwdg.h"

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



/*
GPIO

PORT & PIN      MODE	SETTINGS	FUNCTION
GPIOA    
0 			    INPUT               ?
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
12              OUTPUT  PP,HIGH     Low Voltage Cutoff Enable?
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









void setup(void){

 RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_GPIOC, ENABLE);
    RCC_ConfigAdcHclk(RCC_ADCHCLK_DIV6);
	RCC_EnableAHBPeriphClk(RCC_AHBPCLKEN_ADCEN, ENABLE);

	/* Initialize voltage monitoring on analog inputs */
	VoltageMonitor_Init();

	

{ // gpio init
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

	//swd

}
	dma_init();
	LCD_flash_dma_init();
	LCD_init();
	/* Initialize PWM for LCD backlight (PA6) - 500Hz, 85% duty cycle */
	PWM_Init(20000);
    PWM_SetDutyCycle(50);  /* Set initial brightness */

	sFLASH_Init();

	SleepWake_Init(10);

}

void mainScreen(void){
/* Variables for button debouncing and activity tracking */
	static uint8_t button_prev_state = 0;
	static uint32_t activity_timer = 0;
	static bool use_ultra_low_power = true;  /* Enable ultra-low power mode */
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

	// /* Check button for wake-up or activity */
	 	uint8_t button_cur_state = GPIO_ReadInputDataBit(BUTTON_PIN);
		if (button_cur_state && !button_prev_state)
			button_prev_state = button_cur_state;

		buttonPressed = !button_cur_state;
		
		{																	// sleep-wake logic

		/* Check for external interrupt wake-up (button or mic) */
		if (SleepWake_IsWakeInterruptTriggered())
		{
			if (SleepWake_IsSleeping()){									// Wake up from sleep via interrupt 
																			
				SleepWake_WakeUp();
				
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

		


		Delay(200);  /* 100ms delay in main loop */

	}
}

void flashScreen(void){
	
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

int main(void)
{
	mainran = 0;
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

			

	
	while(mainran == 2 && !FlashID_error) {
		flashScreen();
		}//end while mainran 2

	while(1)
	{
		/* Main infinite loop */
	}

} // end main







void Delay(volatile uint32_t count)
{
	volatile uint32_t t_delay = count * 0x500;
	for (; t_delay >0; t_delay--);
}
void GPIO_Off(GPIO_Module *GPIOx, uint16_t Pin) {  GPIO_ResetBits(GPIOx, Pin); }
void GPIO_On(GPIO_Module* GPIOx, uint16_t Pin) { GPIO_SetBits(GPIOx, Pin); }
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


/* Assert failed function by user.
 * @param file The name of the call that failed.
 * @param line The source line number of the call that failed.
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
