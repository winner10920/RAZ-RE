/**
 * @file voltage_monitor.h
 * @brief Voltage monitoring using ADC on N32G031K8
 * 
 * Measures voltages on analog input pins and stores results in a struct
 * accessible by debugger.
 */

#ifndef __VOLTAGE_MONITOR_H__
#define __VOLTAGE_MONITOR_H__

#include <stdint.h>
#include "n32g031.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TV1_PIN GPIOA,GPIO_PIN_5

#define TV2_PIN GPIOB,GPIO_PIN_8
/**
 * @brief ADC voltage measurements structure
 * Stores raw ADC values (0-4095 for 12-bit ADC)
 * Voltage can be calculated as: V = (ADC_value / 4095) * 3.3V
 */
typedef struct {
    uint16_t pa0_analog;         /* PA0 - Analog input */
    uint16_t pa1_tv1_sense;      /* PA1 - TV1 voltage sense */
    uint16_t pa2_tv2_sense;      /* PA2 - TV2 voltage sense */
    uint16_t temp_sensor;        /* Internal temperature sensor */
} VoltageReadings;

/**
 * @brief Global volatile voltage readings (accessible by debugger)
 */
extern volatile VoltageReadings g_voltage_readings;

/**
 * @brief Initialize ADC for voltage measurements
 * Sets up ADC with single channel conversion mode and GPIO pins
 */
void VoltageMonitor_Init(void);

/**
 * @brief Read all configured analog channels and update measurements
 * Call this periodically to update the g_voltage_readings struct
 */
void VoltageMonitor_UpdateReadings(void);

/**
 * @brief Read a single ADC channel
 * @param channel ADC channel number (0-9 for N32G031)
 * @return Raw ADC value (0-4095 for 12-bit)
 */
uint16_t VoltageMonitor_ReadChannel(uint8_t channel);

/**
 * @brief Convert raw ADC value to millivolts
 * @param adc_value Raw 12-bit ADC value (0-4095)
 * @return Millivolts (0-3300 for 3.3V reference)
 */
uint16_t VoltageMonitor_ConvertToMillivolts(uint16_t adc_value);

extern void GPIO_Off(GPIO_Module *GPIOx, uint16_t Pin);
extern void GPIO_On(GPIO_Module* GPIOx, uint16_t Pin);
extern void Delay(volatile uint32_t count);

#ifdef __cplusplus
}
#endif

#endif /* __VOLTAGE_MONITOR_H__ */
