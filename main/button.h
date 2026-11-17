/** 
 * $Author: David Švancer <xsvancd00@stud.fit.vutbr.cz>
 *
 * @file button.h
 * @author David Švancer
 *
 * @brief Header file for button implementation.
 */

#ifndef BUTTON_H
#define BUTTON_H

#include "driver/gpio.h"
#include <stdbool.h>

/* Button connected gpio pin to ESP32 */
#define BUTTON_PIN GPIO_NUM_18 

/**
 * @brief Function to inicializate button.
 */
void button_init(void);

/**
 * @brief Function handles button pressing logic.
 * @return Returns true if button is pressed, otherwise false.
 */
bool is_button_pressed(void);

#endif

