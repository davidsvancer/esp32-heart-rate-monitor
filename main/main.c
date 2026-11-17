/** 
 * $Author: David Švancer <xsvancd00@stud.fit.vutbr.cz>
 *
 * @file main.c
 * @author David Švancer
 *
 * @brief Main aplication file, calling functions and handling program logic.
*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "oled_display.h"
#include "button.h"
#include "senzor.h"

void app_main(void)
{
    /* hardware inicialization */
    button_init();
    oled_init();
    senzor_init();

    /* initialization text display on OLED display */
    oled_print_start();

    bool is_measuring_flag = false;         /* true if measuring is on, otherwise false */
    bool last_button_state = false;         /* flag for button state */
    char text_buffer[16];                   /* for print */

    /* debug console print */
    printf("System ready, waiting for button press\n");

    while (1)
    {
        bool current_button_state = is_button_pressed();

        /* button was pressed */
        if (current_button_state && !last_button_state)
        {
            /* we switch measuing flag value to true */            
            is_measuring_flag = !is_measuring_flag;
            if (is_measuring_flag)
            {
                /* new measuring, reseting all values */
                senzor_reset();
                oled_reset_graph();
                oled_clear();
            }
            else
            {
                oled_print_start(); /* measuring stop, show init text on OLED display */
            }

            /* button debounce */
            vTaskDelay(pdMS_TO_TICKS(200)); 
        }

        last_button_state = current_button_state;

        if (is_measuring_flag)
        {
            /* measure value and get values from getters */
            senzor_measure_step();
            int bpm = senzor_get_bpm();
            int rawValue = senzor_get_raw_value();

            /* display values on OLED, text and graph */
            oled_clear();
            sprintf(text_buffer, "BPM %d+", bpm);
            oled_draw_string_scaled(0, 0, text_buffer, 2);
            oled_update_graph(rawValue);
            oled_update_data_values(); 
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
