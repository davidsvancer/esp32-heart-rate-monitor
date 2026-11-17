/** 
 * $Author: David Švancer <xsvancd00@stud.fit.vutbr.cz>
 *
 * @file oled_display.h
 * @author David Švancer
 *
 * @brief Header file for oled display implementation.
*/

#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include "sdkconfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include <string.h>
#include "esp_mac.h"
#include <stdint.h>
 
#define OLED_ADDR                   0x3C    /* OLED Adress */
#define I2C_MASTER_SCL_IO           22      /* Gpio pin number (SCL) */ 
#define I2C_MASTER_SDA_IO           21      /* Gpio pin number (SDA) */  
#define I2C_MASTER_NUM              0       /* I2C port number for communication */  
#define I2C_MASTER_FREQ_HZ          400000  /* I2C communication frequency (400 kHz) */ 
#define I2C_MASTER_TX_BUF_DISABLE   0       /* Disable I2C TX buffer */  
#define I2C_MASTER_RX_BUF_DISABLE   0       /* Disable I2C RX buffer */  
#define I2C_MASTER_TIMEOUT_MS       1000    /* I2C communication timeout in milliseconds */ 

#define OLED_PIXEL_WIDTH 128
#define GRAPH_HEIGHT_PX 32
#define OLED_PAGES 8
#define FRAME_BUFFER_LENGTH OLED_PIXEL_WIDTH * OLED_PAGES

/**
 * @brief Function initializes the OLED display configuration.
 * @return ESP_OK on success, otherwisw an error code on failure.
 */
esp_err_t oled_config_init(void);

/**
 * @brief Function writes a byte to the OLED display.
 *
 * @param reg The register to write to.
 * @param data The data byte to be writen.
 */
void oled_write_byte(uint8_t reg, uint8_t data);

/**
 * @brief Function sends a command to the OLED display.
 * @param command The command byte to be send.
 */
void oled_commmand(uint8_t command);

/**
 * @brief Function writes data to the OLED display.
 * @param data The data byte to be writen.
 */
void oled_write_data(uint8_t data);

/**
 * @brief Function initializes the OLED display.
 */
void oled_init(void);

/**
 * @brief Function writes a pixel to the OLED display.
 * @param x The x coordinate of the pixel.
 * @param y The y coordinate of the pixel.
 */
void oled_write_pixel(int x, int y);

/**
 * @brief Function updates the OLED display with the current frame buffer data.
 */
void oled_update_data_values(void);

/**
 * @brief Function clears the OLED display.
 */
void oled_clear(void);

/**
 * @brief Function draws a scaled character to the OLED display.
 * @param x The x coordinate where the character starts.
 * @param page The page where the character starts.
 * @param font_index The index of the character in the font array.
 * @param scale The scaling factor for the character.
 */
void oled_draw_char_scaled(int x, int page, int font_index, int scale);

/**
 * @brief Function draws a scaled string on the OLED display.
 * @param page The page where the string starts.
 * @param x The x coordinate where the string starts.
 * @param text The string to draw.
 * @param scale The scaling factor for the string.
 */
void oled_draw_string_scaled(int page, int x, char *text, int scale);

/**
 * @brief Function draws a graph on the OLED display.
 * @param start_page The starting page for the graph.
 * @param data The data array representing the graph values.
 * @param length The length of the data array.
 * @param max_height The maximum height of the graph in pixels.
 */
void oled_draw_graph(int start_page, uint8_t *data, int length, int max_height);

/**
 * @brief Function displays the start screen on the OLED display.
 */
void oled_print_start(void);

/**
 * @brief Function resets the graph data.
 */
void oled_reset_graph(void);

/**
 * @brief Function updates the graph with a new sensor value.
 *
 * @param raw_senzor_value The new raw sensor value to be added to the graph.
 */
void oled_update_graph(int raw_senzor_value);

#endif