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
    uint16_t pa1_tv1_sense;      /* PA1 - TV1 voltage sense */
    uint16_t pa2_tv2_sense;      /* PA2 - TV2 voltage sense */
    uint16_t pc13_unknown;       /* PC13 - Unknown analog */
    uint16_t pc14_fslp;          /* PC14 - FSLP */
    uint16_t pc15_vpsw;          /* PC15 - VPSW */
} ADC_MeasurementsType;

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
