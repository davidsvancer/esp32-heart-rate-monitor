/** 
 * $Author: David Švancer <xsvancd00@stud.fit.vutbr.cz>
 *
 * @file button.c
 * @author David Švancer
 *
 * @brief Button implementation file.
*/

#include "button.h"

void button_init(void)
{
    gpio_config_t io_conf = {};
        io_conf.pin_bit_mask = (1ULL << BUTTON_PIN);
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = 1;                 /* pull-up resistor */
        io_conf.pull_down_en = 0;
        io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
}

/* Since we have button configured with pull up resistor, button have 1 as passive value
   and 0 as pressed */
bool is_button_pressed(void)
{
    return gpio_get_level(BUTTON_PIN) == 0;
}