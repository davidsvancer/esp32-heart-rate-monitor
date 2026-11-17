/** 
 * $Author: David Švancer <xsvancd00@stud.fit.vutbr.cz>
 *
 * @file senzor.c
 * @author David Švancer
 *
 * @brief Senzor implementation file.
*/

#include "senzor.h"

/* private local variables */
static adc_oneshot_unit_handle_t adc1_handle;
static float threshold = 0;
static float smooth_value = 0;
static int raw_senzor_value = 0;
static int current_value = 0;
static int64_t last_beat_time = 0;
static bool pulse_detected_flag = false;
static int BPM_buffer[BPM_BUFFER_SIZE] = {0};   /* buffer for BPM median value calculation */
static int BPM_buffer_index = 0;
static int BPM_median_value = 0; 
static int beat_count = 0;

void senzor_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = { .unit_id = ADC_UNIT_1 };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));

    adc_oneshot_chan_cfg_t channel_config = 
    {
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &channel_config));
    
    senzor_reset();
}

void senzor_reset(void) 
{
    /* get raw unfiltered value and set is as temporary threshold */
    adc_oneshot_read(adc1_handle, ADC_CHANNEL, &raw_senzor_value);
    threshold = raw_senzor_value;
    smooth_value = raw_senzor_value;
    current_value = raw_senzor_value;
    
    /* reset all values for next BPM measurement */
    last_beat_time = 0;
    pulse_detected_flag = false;
    beat_count = 0;
    BPM_buffer_index = 0;
    BPM_median_value = 0;
    for(int i=0; i<BPM_BUFFER_SIZE; i++) 
    {
        BPM_buffer[i] = 0;
    }
    
    /* Debug print for console, plot */
    printf("Senzor values reseted, ready for next measurement\n");
}

void senzor_measure_step(void) 
{
    /* raw unfiltered value read from analog senzor */
    adc_oneshot_read(adc1_handle, ADC_CHANNEL, &raw_senzor_value);

    /* filtering, using LOW PASS filter */
    smooth_value = (0.8 * smooth_value) + (0.2 * raw_senzor_value);
    current_value = (int)smooth_value;

    /* dynamic threshold adjustment */
    if (current_value > threshold)
    {
        threshold = 0.95 * threshold + 0.05 * current_value;
    } 
    else
    {
        threshold = 0.995 * threshold + 0.005 * current_value;
    }
    
    /* hystereze */
    int trigger_level = (int)threshold + 10; 

    /* current filtered value is bigger than our trigger level and pulse was not detected means new pulse */
    if (current_value > trigger_level && !pulse_detected_flag)
    {
        pulse_detected_flag = true;
        int64_t now = esp_timer_get_time();

        /* we skip first pulse, because we need two detected pulses for BPM calculation */ 
        if (last_beat_time != 0)   
        {
            int64_t time_difference_between_pulses = now - last_beat_time;

            if (time_difference_between_pulses > 250000)
            {
                int BPM_current_value = 60000000 / time_difference_between_pulses;      /* BPM formula */

                if (BPM_current_value > 40 && BPM_current_value < 220) {
                    
                    /* save calculated value to circular buffer */
                    BPM_buffer[BPM_buffer_index] = BPM_current_value;
                    BPM_buffer_index = (BPM_buffer_index + 1) % BPM_BUFFER_SIZE;
                    if (beat_count < BPM_BUFFER_SIZE)
                    {
                        beat_count++;
                    } 

                    /* copy data from circular buffer and sort them */
                    int sorted_buffer[BPM_BUFFER_SIZE];
                    for(int i = 0; i < beat_count; i++) {
                        sorted_buffer[i] = BPM_buffer[i];
                    }
                    bubble_sort(sorted_buffer, beat_count);

                    BPM_median_value = calculate_median(sorted_buffer, beat_count);  /* calculate median value */

                    /* Debug print for console */
                    printf("Pulse BPM, median: %d actual: %d\n", BPM_median_value, BPM_current_value);
                }
            }
        }
        last_beat_time = now;   /* save current time as last for next measurement */
    }
    else if (current_value < threshold && pulse_detected_flag)  /* off between detected pulses */
    {
        pulse_detected_flag = false;
    }
}

int senzor_get_bpm(void) 
{
    return BPM_median_value;
}

int senzor_get_raw_value(void) 
{
    return current_value;
}

bool senzor_is_pulse_active(void) 
{
    return pulse_detected_flag;
}