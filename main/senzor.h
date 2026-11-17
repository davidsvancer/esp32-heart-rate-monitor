/** 
 * $Author: David Švancer <xsvancd00@stud.fit.vutbr.cz>
 *
 * @file senzor.h
 * @author David Švancer
 *
 * @brief Header file for senzor implementation.
*/

#ifndef SENZOR_H
#define SENZOR_H

#include <stdbool.h>
#include <stdio.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "util.h"

/* Analog senzor pin connected to ESP32 */
#define ADC_CHANNEL ADC1_CHANNEL_6 
/* Buffer size for BPM array */
#define BPM_BUFFER_SIZE 10

/**
 * @brief Function inicializate senzor.
 */
void senzor_init(void);

/**
 * @brief Function resets and nulls all variables associated with pulse measurement.
 */
void senzor_reset(void);

/**
 * @brief Function reads raw value from senzor input, filters it and contains BPM detection logic.
 */
void senzor_measure_step(void);

/* Getters */

/**
 * @brief Function gets current BPM value.
 * @return Returns the BPM value as median from bpm array for better stability.
 */
int senzor_get_bpm(void);

/**
 * @brief Function gets current raw value read from analog senzor.
 * @return Returns the current read raw filtered value from senzor.
 */
int senzor_get_raw_value(void);

/**
 * @brief Helper function implemented for debug, return bool value based on pulse.
 * @return Returns true if pulse was detected, otherwise false.
 */
bool senzor_is_pulse_active(void); 

#endif
