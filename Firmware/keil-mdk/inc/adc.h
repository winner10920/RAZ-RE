/**
 * @file adc.h
 * @brief ADC voltage measurement for N32G031K8
 * 
 * Measures voltages on analog input pins and stores results in a struct
 * accessible by debugger.
 */

#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ADC voltage measurements structure
 * Stores raw ADC values (0-4095 for 12-bit ADC)
 * Voltage can be calculated as: V = (ADC_value / 4095) * 3.3V
 */
typedef struct {
    uint16_t ch1_batt;      /* ch1 - Battery Monitor */
    uint16_t ch2_tv2_sense;      /* ch - TV2 voltage sense */
    uint16_t ch3_unknown;       /*ch3 - Unknown analog */
    uint16_t ch4_tempVref;          /* ch4- ? */
    uint16_t ch12_intTemp;          /* Internal Temperature analog */
} ADC_MeasurementsType;

/*
ADC1: The primary Analog-to-Digital Converter.
​Pin mapping:
​Channel 1 (PA1): Battery Voltage Monitor. (Inferred from usage in FUN_08007908 checking threshold 500).
​Channel 2 (PA2): Unknown Sensor (Joystick X?).
​Channel 3 (PA3): Unknown Sensor (Joystick Y?).
​Channel 4 (PA4): VREF / Temperature?*/

/**
 * @brief Global volatile ADC measurements (accessible by debugger)
 */
extern volatile ADC_MeasurementsType g_adc_measurements;

/**
 * @brief Initialize ADC for voltage measurements
 * Sets up ADC with continuous conversion mode
 */
void ADC_MeasureInit(void);

/**
 * @brief Read all ADC channels and update measurements
 * Call this periodically to update the g_adc_measurements struct
 */
void ADC_MeasureReadChannels(void);

/**
 * @brief Read a single ADC channel
 * @param channel ADC channel number (0-9 for N32G031)
 * @return Raw 12-bit ADC value (0-4095)
 */
uint16_t ADC_MeasureReadChannel(uint8_t channel);

/**
 * @brief Convert raw ADC value to voltage
 * @param adc_value Raw 12-bit ADC value (0-4095)
 * @return Voltage in millivolts (0-3300mV)
 */
uint16_t ADC_MeasureConvertToVoltage(uint16_t adc_value);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
