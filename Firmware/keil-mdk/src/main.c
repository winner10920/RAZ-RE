

#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include "n32g031_rcc.h"
#include "nv3029.h"
#include "spi_flash.h"
#include "dma.h"
#include "pwm.h"
#include "sleep_wake.h"
#include "voltage_monitor.h"


#define TV1_VOLTAGE_SENSE_PIN GPIOA,GPIO_PIN_1

#define TV2_VOLTAGE_SENSE_PIN GPIOA,GPIO_PIN_2

#define MIC_PIN GPIOA,GPIO_PIN_3

#define GPIO_PA4 GPIOA,GPIO_PIN_4

#define TV1_PIN GPIOA,GPIO_PIN_5

#define LCD_BACKLIGHT_PIN GPIOA,GPIO_PIN_6

#define BUTTON_PIN GPIOA,GPIO_PIN_7

#define UNKNOWN_OUTPUT_PIN GPIOA,GPIO_PIN_12

#define LCD_SPI_CS_PIN GPIOA,GPIO_PIN_15





#define LP4086_ISET_PIN GPIOB,GPIO_PIN_0

#define LP4086_CHRG_PIN GPIOB,GPIO_PIN_1

#define UNKNOWN_INPUT_PIN GPIOB,GPIO_PIN_2

#define LCD_SCLK_PIN GPIOB,GPIO_PIN_3

#define LCD_FLASH_PWR_EN_PIN GPIOB,GPIO_PIN_4

#define LCD_MOSI_PIN GPIOB,GPIO_PIN_5

#define LCD_RST_PIN GPIOB,GPIO_PIN_6

#define LCD_DC_PIN GPIOB,GPIO_PIN_7

#define TV2_PIN GPIOB,GPIO_PIN_8



/*
GPIO

PORT & PIN      MODE	SETTINGS	FUNCTION
GPIOA    
0 			    INPUT               ?
1 			    ANALOG  HSR         TV1_VOLTAGE_SENSE        
2               ANALOG  HSR         TV2_VOLTAGE_SENSE
3			    INPUT               MIC ?
4			    OUTPUT  PP,LOW      NC
5			    OUTPUT  PP,LOW      ELEMENT TV2
6				OUTPUT  PP,LOW      LCD_BACKLIGHT
7			    INPUT   PULLUP      BUTTON
8               OUTPUT  PP,HIGH     FLASH_CS
9               OUTPUT  PP,LOW      SPI2_SCLK, FLASH_SPI_SCLK
10 			    OUTPUT  PP,LOW      SPI2_MISO, FLASH_SPI_MISO
11 			    INPUT   PULLUP      SPI2_MOSI, FLASH_SPI_MOSI
12              OUTPUT  PP,HIGH     UNKNOWN_OUTPUT
13				INPUT   PULLUP      SWCLK
14              INPUT   PULLDOWN    SWDIO
15              OUTPUT  PP,HIGH     LCD_SPI_CS

GPIOB
0	            OUTPUT  PP,LOW      LP4086_ISET
1               INPUT   PULLUP      LP4086_CHRG (PULLED LOW ACTIVE) ext1 interupt
2			    INPUT   PULLUP      UNKOWN_INPUT maybe batt v?
3               OUTPUT  PP,LOW      SPI1_SCLK, LCD_SPI_SCLK
4               OUTPUT  OD,LOW      LCD_FLASH_PWR_EN 
5			    OUTPUT  PP,LOW      SPI1_MOSI, LCD_SPI_MOSI
6 			    OUTPUT  PP,HIGH     LCD_RST
7  		        OUTPUT  PP,HIGH     LCD_DC
8				OUTPUT  PP,LOW      ELEMENT TV1

GPIOC
13              ANALOG  HSR         UNKNOWN_ANALOG
14 			    ? 					FSLP     
15              ?                   VPSW

GPIOF
0              ANALOG  HSR         UNKOWN_ANALOG
1              ANALOG  HSR         UNKOWN_ANALOG
2              ANALOG  HSR         UNKOWN_ANALOG
6              ANALOG  HSR         UNKOWN_ANALOG
7              ANALOG  HSR         UNKOWN_ANALOG
*/

extern char __heap_start__;
extern char __heap_end__;
extern char __stack_start__;
extern char __stack_end__;

void check_memory_boundaries(void) {
    volatile char *heap_start = &__heap_start__;
	volatile char *heap_end = &__heap_end__;
	// ... use these pointers for monitoring
	}


volatile uint8_t mainran = 0;

volatile uint32_t* SPI1DATA = (uint32_t*)0x4001200C;

#define BUFFER_SIZE 4096
volatile int page[1];
volatile int lvl_reset_flag[1];
volatile int continue_flag[1];
volatile int write_flag[1];
volatile int status[1];
volatile bool RunError = false;
uint8_t lvl_buffer[5];
uint8_t lvl_buffer_read[5];
uint8_t buffer[BUFFER_SIZE];
uint32_t FlashID = 0;
uint32_t status_register;

uint8_t sFLASH_ReadRegister(uint8_t reg)
{
    uint8_t flashstatus = 0;

    /*!< Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();

    /*!< Send "Read Status Register" instruction */
    sFLASH_SendByte(sFLASH_CMD_RDSR_1);
    
    flashstatus = sFLASH_SendByte(reg);

    /*!< Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();
		
		return flashstatus;
}

int main(void)
{
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_GPIOC, ENABLE);
    RCC_ConfigAdcHclk(RCC_ADCHCLK_DIV6);
	RCC_EnableAHBPeriphClk(RCC_AHBPCLKEN_ADCEN, ENABLE);

	/* Initialize voltage monitoring on analog inputs */
	VoltageMonitor_Init();

	/* Initialize PWM for LCD backlight (PA6) - 500Hz, 85% duty cycle */
	PWM_Init(20000);
	PWM_SetDutyCycle(35);  /* Set initial brightness */


	GPIO_Init(LCD_FLASH_PWR_EN_PIN, GPIO_MODE_OUTPUT_PP);
	GPIO_On(LCD_FLASH_PWR_EN_PIN);

	GPIO_Init(UNKNOWN_OUTPUT_PIN, GPIO_MODE_OUTPUT_PP);
	GPIO_On( UNKNOWN_OUTPUT_PIN);

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
    //GPIO_Init(UNKNOWN_INPUT_PIN, GPIO_MODE_INPUT);

    // ANALOG

	GPIO_Init(TV1_VOLTAGE_SENSE_PIN, GPIO_MODE_ANALOG);
	GPIO_Init(TV2_VOLTAGE_SENSE_PIN, GPIO_MODE_ANALOG);
    GPIO_Init(UNKNOWN_INPUT_PIN, GPIO_MODE_ANALOG);



    // Not connected
	GPIO_Init(GPIO_PA4, GPIO_MODE_OUTPUT_PP);
	GPIO_Off(GPIO_PA4);

    Delay(1200);
	
	LCD_init();
		
	LCD_diag();

	/* Initialize sleep/wake system - 30 second inactivity timeout */
	SleepWake_Init(30);
	
	/* Variables for button debouncing and activity tracking */
	static uint8_t button_prev_state = 0;
	static uint32_t activity_timer = 0;

	
    

	

	/* Simple PA7 test: toggle LED on each rising edge (short startup test) */
	// {
	// 	uint8_t prev = GPIO_ReadInputDataBit(BUTTON_PIN);
	// 	uint8_t led_state = GPIO_ReadOutputDataBit(LCD_BACKLIGHT_PIN);; // currently LED was turned on earlier
	// 	for (int i = 0; i < 500; ++i) {
	// 		uint8_t cur = GPIO_ReadInputDataBit(BUTTON_PIN);
	// 		if (cur && !prev) {
	// 			led_state = !led_state;
	// 			if (led_state) GPIO_On(LCD_BACKLIGHT_PIN); else GPIO_Off(LCD_BACKLIGHT_PIN);
	// 		}
	// 		prev = cur;
	// 		Delay(100);
	// 	}
	// }


	if( mainran == 0){

		 sFLASH_Init();
		
	while(1){
		Delay(100);
		FlashID = sFLASH_ReadID();
		if(FlashID != 0) break;
	}
    mainran++;
	/* Main loop - includes sleep/wake and button handling */
	while(mainran == 1 && !RunError) {
		/* Check button for wake-up or activity */
		uint8_t button_cur_state = GPIO_ReadInputDataBit(BUTTON_PIN);
		if (button_cur_state && !button_prev_state)  /* Button pressed */
		{
			if (SleepWake_IsSleeping())
			{
				/* Wake up from sleep */
				SleepWake_WakeUp();
			}
			/* Reset inactivity timer on button press */
			SleepWake_ResetTimer();
		}
		button_prev_state = button_cur_state;

		/* Check if timeout for sleep has expired */
		if (SleepWake_CheckTimeout())
		{
			/* Go to sleep */
			SleepWake_GoToSleep();
		}

		/* Update voltage readings periodically */
		VoltageMonitor_UpdateReadings();

		/* Normal device operation code here */
		/* (Flash operations, display updates, etc.) */
		
		Delay(100);  /* 100ms delay in main loop */
	}

	/* Original flash check loop */
	// Check Flash ID
	if(FlashID == 8740886){      //sFLASH_PD32S_ID){
		status[0] = 1;
		
		// Wait for continue flag
		while(continue_flag[0] == 0);
		continue_flag[0] = 0;
		
		status[0] = 2;
		continue_flag[0] = 0;
		status[0] = 2;
		
		// If level reset flag is set, execute the level reset
		if(lvl_reset_flag[0] == 1){
			lvl_buffer[4] = 0xBB;
			sFLASH_EraseSector(0xf8000);
			sFLASH_WriteBuffer(lvl_buffer, 0xf8000, 5);
			status[0] = 3;
			
		}
		//for(int i=0; i<4096; i++){
			//buffer[i] = 0xe1;
		//}
		
		//Delay(10);
		sFLASH_ReadBuffer(lvl_buffer_read, 0xf8000, 5);
		
		volatile int tmp_page = -1;
		
		// If the write flag is set, initialize write
		if(write_flag[0] == 1){
			write_flag[0] = 0;
			sFLASH_EraseBulk();
			Delay(10);
			status[0] = 5;
			for(uint32_t i=0; i<256; i++){
				while(status[0] == 5) ;
				uint32_t addr = i * BUFFER_SIZE;
				sFLASH_WriteBuffer(buffer, addr, BUFFER_SIZE);
				status[0] = 5;
			}
			//sFLASH_EraseBulk();
			//Delay(10);
			//while(1){
				//if(tmp_page != page[0]){
					//status[0] = 4;
					//tmp_page = page[0];
					//uint32_t addr = page[0] * BUFFER_SIZE;
					//sFLASH_WriteBuffer(buffer, addr, BUFFER_SIZE);
					//for(int i=0; i<16; i++){
						//int offset = (i * 256);
						//uint32_t sector_addr = addr + offset;
						//pointer = sector_addr;
						//Delay(1000);
						
					//}
					//sFLASH_ReadBuffer(buffer, addr, BUFFER_SIZE);
					//status[0] = 5;
				//}
				
			//}
		}else{
			while(1){
				if(tmp_page != page[0]){
					status[0] = 6;
					tmp_page = page[0];
					uint32_t addr = page[0] * BUFFER_SIZE;
					sFLASH_ReadBuffer(buffer, addr, BUFFER_SIZE);
					status[0] = 7;
				}
			}
		}
	}else{
		// If there is an error reading the Flash ID, pulse the light 3 times for 1 second delay
		PWM_SetDutyCycle(50); 
		Delay(1000);
		PWM_SetDutyCycle(20); 
		Delay(1000);
		PWM_SetDutyCycle(50); 
		Delay(1000);
		PWM_SetDutyCycle(20); 
		Delay(1000);
		PWM_SetDutyCycle(50); 
		Delay(1000);
		PWM_SetDutyCycle(20); 
		Delay(1000);
		PWM_SetDutyCycle(50); 
	    RunError = true;
		//while(1)
		mainran = 1;
	}
	}
	else{



	}

}







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
