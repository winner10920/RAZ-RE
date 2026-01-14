/**
 * @file adc.c
 * @brief ADC implementation for N32G031K8
 */

#include "adc.h"
#include "n32g031.h"
#include "n32g031_adc.h"
#include "n32g031_rcc.h"
#include "n32g031_gpio.h"

/* Global volatile measurements struct - accessible from debugger */
volatile ADC_MeasurementsType g_adc_measurements = {0};

/* ADC channel definitions for the N32G031 */
#define ADC_CHANNEL_PA1   1    /* PA1 - TV1 voltage sense */
#define ADC_CHANNEL_PA2   2    /* PA2 - TV2 voltage sense */
#define ADC_CHANNEL_PC13  13   /* PC13 - Unknown analog */
#define ADC_CHANNEL_PC14  14   /* PC14 - FSLP */
#define ADC_CHANNEL_PC15  15   /* PC15 - VPSW */

/**
 * @brief Initialize ADC for voltage measurements
 */
void ADC_MeasureInit(void)
{
    ADC_InitType adc_init_struct;
    GPIO_InitType gpio_init_struct;

    /* Enable ADC clock */
    RCC_EnableAHBPeriphClk(RCC_AHBPCLKEN_ADCEN, ENABLE);
    
    /* Enable GPIO clocks for analog pins */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);

    /* Configure PA1 as analog input */
    GPIO_InitStruct(&gpio_init_struct);
    gpio_init_struct.Pin = GPIO_PIN_1;
    gpio_init_struct.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitPeripheral(GPIOA, &gpio_init_struct);

    /* Configure PA2 as analog input */
    gpio_init_struct.Pin = GPIO_PIN_2;
    GPIO_InitPeripheral(GPIOA, &gpio_init_struct);

    /* Configure PC13 as analog input */
    gpio_init_struct.Pin = GPIO_PIN_13;
    GPIO_InitPeripheral(GPIOC, &gpio_init_struct);

    /* Configure PC14 as analog input */
    gpio_init_struct.Pin = GPIO_PIN_14;
    GPIO_InitPeripheral(GPIOC, &gpio_init_struct);

    /* Configure PC15 as analog input */
    gpio_init_struct.Pin = GPIO_PIN_15;
    GPIO_InitPeripheral(GPIOC, &gpio_init_struct);

    /* Configure ADC */
    ADC_InitStruct(&adc_init_struct);
    adc_init_struct.ClkDiv = ADC_CLK_DIV_8;           /* ADC clock = APB1 / 8 */
    adc_init_struct.ScanConvMode = DISABLE;           /* Single channel mode for simplicity */
    adc_init_struct.ContinConvMode = DISABLE;         /* Single conversion mode */
    adc_init_struct.ExtTrigSelect = ADC_EXT_TRIGCONV_NONE;  /* Software trigger */
    adc_init_struct.DataAlign = ADC_DAT_ALIGN_R;      /* Right aligned */
    adc_init_struct.ChsNumber = 1;                    /* Single channel */
    ADC_Init(ADC, &adc_init_struct);

    /* Enable ADC */
    ADC_Enable(ADC, ENABLE);

    /* Wait for ADC to stabilize */
    uint32_t wait_count = 0;
    while ((ADC->CTRL2 & 0x40) == 0 && wait_count < 1000000)
        wait_count++;
}

/**
 * @brief Read a single ADC channel with software trigger
 * @param channel ADC channel number
 * @return Raw 12-bit ADC value (0-4095)
 */
uint16_t ADC_MeasureReadChannel(uint8_t channel)
{
    /* Configure the channel for conversion */
    ADC->SAMPT1 = 0;  /* Sampling time = 1.5 cycles (minimum) */
    
    /* Set the channel in the regular group */
    ADC->RSEQ1 = channel;
    
    /* Start software conversion */
    ADC->CTRL2 |= 0x00000001;  /* SWSTART bit */
    
    /* Wait for conversion to complete */
    uint32_t timeout = 0;
    while ((ADC->STAT & 0x0002) == 0 && timeout < 1000000)  /* EOC flag */
        timeout++;
    
    /* Read the converted value */
    uint16_t adc_value = ADC->DAT & 0xFFF;  /* 12-bit value */
    
    return adc_value;
}

/**
 * @brief Read all ADC channels and update measurements struct
 */
void ADC_MeasureReadChannels(void)
{
    g_adc_measurements.pa1_tv1_sense = ADC_MeasureReadChannel(ADC_CHANNEL_PA1);
    g_adc_measurements.pa2_tv2_sense = ADC_MeasureReadChannel(ADC_CHANNEL_PA2);
    g_adc_measurements.pc13_unknown = ADC_MeasureReadChannel(ADC_CHANNEL_PC13);
    g_adc_measurements.pc14_fslp = ADC_MeasureReadChannel(ADC_CHANNEL_PC14);
    g_adc_measurements.pc15_vpsw = ADC_MeasureReadChannel(ADC_CHANNEL_PC15);
}

/**
 * @brief Convert raw ADC value to voltage in millivolts
 * @param adc_value Raw 12-bit ADC value (0-4095)
 * @return Voltage in millivolts (0-3300mV for 3.3V reference)
 */
uint16_t ADC_MeasureConvertToVoltage(uint16_t adc_value)
{
    /* Voltage = (ADC_value / 4095) * 3300mV */
    /* Using fixed point math to avoid floating point */
    return (uint16_t)((uint32_t)adc_value * 3300 / 4095);
}
