/** 
 * $Author: David Švancer <xsvancd00@stud.fit.vutbr.cz>
 *
 * @file oled_display.c
 * @author David Švancer
 *
 * @brief Oled display implementation file.
*/

#include "oled_display.h"

/* local private variables */
static uint8_t graph_buffer[OLED_PIXEL_WIDTH]; 
static uint8_t frame_buffer[FRAME_BUFFER_LENGTH];
static int signal_min = 4095;    /* 4095 is analog senzor max value on mine selected configuration */
static int signal_max = 0;

/* font alphabet */
static const unsigned char font8x8[][8] = 
{
    {0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x00, 0x00},   // char '0' index 0
    {0x00, 0x42, 0x7F, 0x40, 0x00, 0x00, 0x00, 0x00},   // char '1' index 1
    {0x42, 0x61, 0x51, 0x49, 0x46, 0x00, 0x00, 0x00},   // 2 char '2' index 2
    {0x21, 0x41, 0x45, 0x4B, 0x31, 0x00, 0x00, 0x00},   // 3 char '3' index 3
    {0x18, 0x14, 0x12, 0x7F, 0x10, 0x00, 0x00, 0x00},   // 4 char '4' index 4
    {0x27, 0x45, 0x45, 0x45, 0x39, 0x00, 0x00, 0x00},   // 5 char '5' index 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00, 0x00, 0x00},   // 6 char '6' index 6
    {0x01, 0x71, 0x09, 0x05, 0x03, 0x00, 0x00, 0x00},   // 7 char '7' index 7
    {0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00, 0x00},   // 8 char '8' index 8
    {0x06, 0x49, 0x49, 0x29, 0x1E, 0x00, 0x00, 0x00},   // 9 char '9' index 9
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // Space char ' ' index 10
    {0x7F, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00, 0x00},   // B char 'B' index 11
    {0x7F, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00},   // P char 'P' index 12
    {0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x00, 0x00, 0x00},   // M char 'M' index 13
    {0x0C, 0x1E, 0x3E, 0x7C, 0x3E, 0x1E, 0x0C, 0x00},   // Heart char 'heart' index 14
    {0x7F, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00},   // P char 'P' index 15
    {0x7F, 0x09, 0x19, 0x29, 0x46, 0x00, 0x00, 0x00},   // R char 'R' index 16
    {0x7F, 0x49, 0x49, 0x49, 0x41, 0x00, 0x00, 0x00},   // E char 'E' index 17
    {0x46, 0x49, 0x49, 0x49, 0x31, 0x00, 0x00, 0x00},   // S char 'S' index 18
    {0x7F, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00, 0x00},   // B char 'B' index 19
    {0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00, 0x00, 0x00},   // U char 'U' index 20
    {0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, 0x00, 0x00},   // T char 'T' index 21
    {0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00, 0x00, 0x00},   // O char 'O' index 22
    {0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00, 0x00, 0x00},   // N char 'N' index 23
    {0x7E, 0x09, 0x09, 0x09, 0x7E, 0x00, 0x00, 0x00}    // A char 'A' index 24
};

esp_err_t oled_config_init(void) 
{
    i2c_config_t conf = 
    {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(I2C_MASTER_NUM, &conf);

    return i2c_driver_install
    (
        I2C_MASTER_NUM, conf.mode,
        I2C_MASTER_RX_BUF_DISABLE,
        I2C_MASTER_TX_BUF_DISABLE,
        0
    );
}

void oled_write_byte(uint8_t reg, uint8_t data)
{
    uint8_t write_buf[2] = {reg, data};

    /* write and show data on oled display */
    i2c_master_write_to_device
    (
        I2C_MASTER_NUM, 
        OLED_ADDR,
        write_buf,
        sizeof(write_buf),
        I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS
    );
}

void oled_commmand(uint8_t command)
{
    oled_write_byte(0x00, command);     /* 0x00 is data mode*/
}

void oled_write_data(uint8_t data)
{
    oled_write_byte(0x40, data);    /* 0x40 is data mode*/
}

void oled_init(void)
{
    /* basic oled display configuration inicialization */
    oled_config_init();

    oled_commmand(0xAE);    /* display off */
    oled_commmand(0xD5);    /* clock divide ratio */
    oled_commmand(0x80);    /* default clock + frequency ratios  */
    oled_commmand(0xA8);    /* mutliplex ratio */
    oled_commmand(0x3F);    /* height 64 pixels */ 
    oled_commmand(0x8D);    /* charge pump setting */ 
    oled_commmand(0x14);    /* enable charge pump */ 
    oled_commmand(0x20);    /* memory addresssing mode */ 
    oled_commmand(0x02);    /* page addressing mode */ 
    oled_commmand(0xA1);    /* segment remap */ 
    oled_commmand(0xC8);    /* COM output scan direction */ 
    oled_commmand(0x81);    /* set contrast */ 
    oled_commmand(0xCF);    /* contrast level high (brighter display) */
    oled_commmand(0xAF);    /* display on */ 

    /* clear and show data on display */
    oled_clear();
    oled_update_data_values();
}

void oled_clear(void) 
{
    for (size_t i = 0; i < FRAME_BUFFER_LENGTH; i++)
    {
        frame_buffer[i] = 0x00;     /* pixel with value 0 is turned off */
    }
}

void oled_update_data_values(void) 
{
    /* Update entire oled display, go through all pages and its pixels */
    for (size_t page = 0; page < OLED_PAGES; page++)
    {
        /* set correct page and its config */
        oled_commmand(0xB0 + page);
        oled_commmand(0x00);
        oled_commmand(0x10);

        /* (+1) because command byte needs to be add */
        uint8_t oled_buffer[OLED_PIXEL_WIDTH + 1];
        oled_buffer[0] = 0x40; /* Data mode */

        for (size_t column = 0; column < OLED_PIXEL_WIDTH; column++)
        {
            oled_buffer[column + 1] = frame_buffer[page * OLED_PIXEL_WIDTH + column];
        }
        
        /* write and show data on oled display */
        i2c_master_write_to_device
        (
            I2C_MASTER_NUM,
            OLED_ADDR,
            oled_buffer,
            OLED_PIXEL_WIDTH + 1,
            I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS
        );
    }
}

void oled_write_pixel(int x, int y) 
{
    int page = y / 8;
    int bit = y % 8;
    frame_buffer[page * OLED_PIXEL_WIDTH + x] |= (1 << bit);        /* pixel with value 1 is shown */
}

void oled_draw_char_scaled(int x, int page, int font_index, int scale)
{
    /* go through each column of the font */
    for (int col = 0; col < 8; col++)
    {
        uint8_t col_data = font8x8[font_index][col]; /* get the column data */ 
        
        /* go through each row of the column */
        for (int row = 0; row < 8; row++)
        {
            /* check if the bit at the current row is set */
            if (col_data & (1 << row))
            {
                /*  scale the pixel horizontally and vertically */
                for (int dx = 0; dx < scale; dx++)
                {
                    for (int dy = 0; dy < scale; dy++)
                    {
                        int px = x + col * scale + dx; /* calculate the x coordinate of the scaled pixel */
                        int py = page * 8 + row * scale + dy; /* calculate the y coordinate of the scaled pixel */

                        /*  check if the pixel is within the display boundaries */
                        if (px < OLED_PIXEL_WIDTH && py < OLED_PAGES * 8)
                        {
                            oled_write_pixel(px, py); 
                        }
                    }
                }
            }
        }
    }
}

void oled_draw_string_scaled(int page, int x, char * text, int scale)
{
    int i = 0;
    while (text[i] != '\0')
    {
        char c = text[i];
        int font_index = 10; 
        if (c >= '0' && c <= '9') font_index = c - '0'; /* char - '0' is the int value of the char */
        else if (c == ' ') font_index = 10;           
        else if (c == 'B') font_index = 11;
        else if (c == 'P') font_index = 12;
        else if (c == 'M') font_index = 13;
        else if (c == '+') font_index = 14;             /* mapped heart symbol */
        else if (c == 'R') font_index = 16;
        else if (c == 'E') font_index = 17;
        else if (c == 'S') font_index = 18;
        else if (c == 'U') font_index = 20;
        else if (c == 'T') font_index = 21;
        else if (c == 'O') font_index = 22;
        else if (c == 'N') font_index = 23;
        else if (c == 'A') font_index = 24;

        oled_draw_char_scaled(x, page, font_index, scale);
        x += 8 * scale;
        i++;
    }
}

void oled_draw_graph(int start_page, uint8_t *data, int length, int max_height)
{
    /* go through each column */
    for(int x = 0; x < length; x++)
    {
        int height = data[x]; /* get the height value of the current column */

        /* check if the height is within limits */
        if(height > max_height)
        {
            height = max_height;
        }

        /* go through each row */
        for(int y = 0; y < max_height; y++)
        {
            int page = start_page + (y / 8); 
            int bit = y % 8; 

            /* if the current row is within the height, set the pixel, otherwise clear it */
            if(y < height)
            {
                frame_buffer[page * OLED_PIXEL_WIDTH + x] |= (1 << bit); 
            } 
            else 
            {
                frame_buffer[page * OLED_PIXEL_WIDTH + x] &= ~(1 << bit); 
            }
        }
    }
}

void oled_print_start(void)
{
    oled_clear();   /* clear oled display */
    int scale = 2;
    oled_draw_string_scaled(1, 24, "PRESS", scale); 
    oled_draw_string_scaled(3, 12, "BUTTON", scale);
    oled_draw_string_scaled(5, 0, "TO START", scale);
    oled_update_data_values();  /* call update to display changed values */
}

void oled_reset_graph(void)
{
    /* reset all graph related values */
    for (int i = 0; i < OLED_PIXEL_WIDTH; i++)
    {
        graph_buffer[i] = 0;
    }
    signal_min = 4095;
    signal_max = 0;
}

void oled_update_graph(int raw_senzor_value)
{
    /* auto scaling */
    if (raw_senzor_value < signal_min)
    {
        signal_min = raw_senzor_value;
    }
    if (raw_senzor_value > signal_max)
    {
        signal_max = raw_senzor_value;
    } 

    /* dynamic slow adjustment */
    signal_min += 2; 
    signal_max -= 2; 
    
    if (signal_min > signal_max - 20)
    {
        signal_min = signal_max - 20;
    } 

    /* convert to pixels */
    int graphHeight = 0;
    int range = signal_max - signal_min;

    if (range > 0)
    {
        graphHeight = (raw_senzor_value - signal_min) * GRAPH_HEIGHT_PX / range;
    }

    /* check if the height is within limits */
    if (graphHeight < 0)
    {
        graphHeight = 0;
    } 
    if (graphHeight > GRAPH_HEIGHT_PX)
    {
        graphHeight = GRAPH_HEIGHT_PX;
    } 

    /* Shift values and store the new one */
    for (int i = 0; i < OLED_PIXEL_WIDTH - 1; i++)
    {
        graph_buffer[i] = graph_buffer[i + 1];
    }
    graph_buffer[OLED_PIXEL_WIDTH - 1] = (uint8_t)graphHeight;

    /* write the data to display by calling this function, grapf is on page 3 under the BPM text */
    oled_draw_graph(3, graph_buffer, OLED_PIXEL_WIDTH, GRAPH_HEIGHT_PX);
}
