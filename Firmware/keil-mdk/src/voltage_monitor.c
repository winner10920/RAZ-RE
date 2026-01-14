/**
 * @file voltage_monitor.c
 * @brief Voltage monitoring using ADC on N32G031K8
 * 
 * Uses the N32G031 standard peripheral driver for ADC
 */

#include "voltage_monitor.h"
#include "n32g031.h"
#include "n32g031_adc.h"
#include "n32g031_rcc.h"
#include "n32g031_gpio.h"

/* ADC channel definitions */
#define MONITOR_CHANNEL_PA0      0   /* PA0 - Analog input */
#define MONITOR_CHANNEL_PA1      1   /* PA1 - TV1 voltage sense */
#define MONITOR_CHANNEL_PA2      2   /* PA2 - TV2 voltage sense */
#define MONITOR_CHANNEL_CH3      3   /* Unused */
#define MONITOR_CHANNEL_CH4      10   /* Unused */
#define MONITOR_CHANNEL_TEMP    12   /* Internal temperature sensor */

/*
ADC1: The primary Analog-to-Digital Converter.
​Pin mapping:
​Channel 1 (PA1): Battery Voltage Monitor. (Inferred from usage in FUN_08007908 checking threshold 500).
​Channel 2 (PA2): Unknown Sensor (Joystick X?).
​Channel 3 (PA3): Unknown Sensor (Joystick Y?).
​Channel 4 (PA4): VREF / Temperature?*/
/* Global volatile voltage readings */
volatile VoltageReadings g_voltage_readings = {0};

/**
 * @brief Initialize ADC for voltage measurements
 */
void VoltageMonitor_Init(void)
{
    GPIO_InitType gpio_init_struct;
    ADC_InitType adc_init_struct;

    /* Enable GPIO clocks for analog input pins */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);

    /* Configure PA0, PA1, PA2,  as analog inputs */
    GPIO_InitStruct(&gpio_init_struct);
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    gpio_init_struct.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitPeripheral(GPIOA, &gpio_init_struct);

    GPIO_InitStruct(&gpio_init_struct);
    gpio_init_struct.Pin = GPIO_PIN_2;
    gpio_init_struct.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitPeripheral(GPIOB, &gpio_init_struct);

    GPIO_InitStruct(&gpio_init_struct);
    gpio_init_struct.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    gpio_init_struct.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitPeripheral(GPIOC, &gpio_init_struct);

    /* Enable ADC clock */
    RCC_EnableAHBPeriphClk(RCC_AHBPCLKEN_ADCEN, ENABLE);

    /* Configure ADC */
    ADC_InitStruct(&adc_init_struct);
    adc_init_struct.MultiChEn = DISABLE;          /* Single channel mode */
    adc_init_struct.ContinueConvEn = DISABLE;     /* Single conversion mode */
    adc_init_struct.ExtTrigSelect = ADC_EXT_TRIGCONV_NONE; /* Software trigger */
    adc_init_struct.DatAlign = ADC_DAT_ALIGN_R;   /* Right aligned */
    adc_init_struct.ChsNumber = 1;                /* 1 channel */

    /* Initialize ADC */
    ADC_Init(ADC, &adc_init_struct);

    /* Enable ADC */
    ADC_Enable(ADC, ENABLE);
    
    /* Enable temperature sensor (required before reading) */
    ADC_EnableTempSensor(ENABLE);
}

/**
 * @brief Read a single ADC channel with software trigger
 * @param channel ADC channel number
 * @return Raw 12-bit ADC value (0-4095)
 */
uint16_t VoltageMonitor_ReadChannel(uint8_t channel)
{
    uint32_t timeout = 1000000;

    /* Configure the channel for conversion (rank 1, 240.5 cycle sampling) */
    ADC_ConfigRegularChannel(ADC, channel, 1, ADC_SAMP_TIME_240CYCLES5);

    /* Start ADC conversion with software trigger */
    ADC_EnableSoftwareStartConv(ADC, ENABLE);

    /* Wait for end of conversion (EOC flag) */
    while ((ADC_GetFlagStatus(ADC, ADC_FLAG_ENDC) == RESET) && (timeout > 0))
    {
        timeout--;
    }

    /* Clear EOC flag */
    ADC_ClearFlag(ADC, ADC_FLAG_ENDC);

    /* Return converted value */
    return ADC_GetDat(ADC);
}

/**
 * @brief Read all configured analog channels and update measurements struct
 */
void VoltageMonitor_UpdateReadings(void)
{
    /* Read all 4 channels and update the struct */
    g_voltage_readings.pa0_analog = VoltageMonitor_ConvertToMillivolts(VoltageMonitor_ReadChannel(MONITOR_CHANNEL_PA0));
    
    GPIO_On(TV1_PIN);  /* Power on voltage sense circuitry if needed */
    g_voltage_readings.pa1_tv1_sense = VoltageMonitor_ConvertToMillivolts(VoltageMonitor_ReadChannel(MONITOR_CHANNEL_PA1));
    GPIO_Off(TV1_PIN); /* Power off to save energy */
    
    GPIO_On(TV2_PIN);  /* Power on voltage sense circuitry if needed */
    g_voltage_readings.pa2_tv2_sense = VoltageMonitor_ConvertToMillivolts(VoltageMonitor_ReadChannel(MONITOR_CHANNEL_PA2));
    GPIO_Off(TV2_PIN); /* Power off to save energy */
    
    
    g_voltage_readings.temp_sensor = ((1300-VoltageMonitor_ConvertToMillivolts(VoltageMonitor_ReadChannel(MONITOR_CHANNEL_TEMP)))/39)+2500; /* Convert to °C using typical formula */
    //g_voltage_readings.temp_sensor = VoltageMonitor_ConvertToMillivolts(VoltageMonitor_ReadChannel(MONITOR_CHANNEL_TEMP)); /* Convert to °C using typical formula */

    g_voltage_readings.ch3_unused = VoltageMonitor_ConvertToMillivolts(VoltageMonitor_ReadChannel(MONITOR_CHANNEL_CH3)); /* Unused channel placeholder */
    g_voltage_readings.pb2_analog = VoltageMonitor_ConvertToMillivolts(VoltageMonitor_ReadChannel(MONITOR_CHANNEL_CH4)); /* Unused channel placeholder */
}

/**
 * @brief Convert raw ADC value to voltage in millivolts
 * @param adc_value Raw 12-bit ADC value (0-4095)
 * @return Voltage in millivolts (0-3300mV for 3.3V reference)
 */
uint16_t VoltageMonitor_ConvertToMillivolts(uint16_t adc_value)
{
    /* Voltage = (ADC_value / 4095) * 3300mV */
    /* Using fixed point math: (adc_value * 3300) / 4095 */
    /* To avoid overflow with 16-bit: use uint32_t for intermediate calculation */
    uint32_t mv = ((uint32_t)adc_value * 3300) / 4095;
    return (uint16_t)mv;
}
