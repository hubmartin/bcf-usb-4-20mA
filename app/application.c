#include <application.h>

// LED instance
bc_led_t led;

// Button instance
bc_button_t button;

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    if (event == BC_BUTTON_EVENT_PRESS)
    {
        bc_led_set_mode(&led, BC_LED_MODE_TOGGLE);
    }

    // Logging in action
    bc_log_info("Button event handler - event: %i", event);
}

void application_init(void)
{
    // Initialize logging
    bc_log_init(BC_LOG_LEVEL_DUMP, BC_LOG_TIMESTAMP_ABS);

    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_set_mode(&led, BC_LED_MODE_ON);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    bc_module_sensor_init();

    bc_module_sensor_set_pull(BC_MODULE_SENSOR_CHANNEL_A, BC_MODULE_SENSOR_PULL_UP_56R);

    /*
    56R pull-up
     mA   - ADC raw data
    20 mA - 43232
    18 mA - 45520
    15 mA - 48736
    10 mA - 54100
     5 mA - 60000
     4 mA - 61104
     2 mA - 63312
     1 mA - 64448
     0 mA - 65504

     value is 16 bit but ADC i only 12 bits so the ADC step is 16

     4 - 20 mA range => 61104 - 43232 = 17872, divide by 16 (12bit resolution) 17872 / 16 = 1117 values
     so for sensor -30 to +80°C which is 110°C range you get resolution 110°C / 1117 = 0.098 °C
     The precision is hard to say.

     0 - 20mA range => 65504 - 43232 = 22272, divide by 16 (12bit resolution) 22272 / 16 = 1392 values
     so for sensor -30 to +80°C which is 110°C range you get resolution 110°C / 1392 = 0.079 °C

    */

    bc_adc_init();
}

static void _adc_event_handler(bc_adc_channel_t channel, bc_adc_event_t event, void *param)
{
    (void) channel;
    (void) param;

    if (event == BC_ADC_EVENT_DONE)
    {
        uint16_t adc;
        bc_adc_async_get_value(BC_ADC_CHANNEL_A4, &adc);
        bc_log_debug("%d", adc);

       //float voltage;
       //bc_adc_get_result_voltage(BC_ADC_CHANNEL_A2, &voltage);
       //bc_log_debug("%f", voltage);
    }
}

void application_task(void)
{
    bc_adc_set_event_handler(BC_ADC_CHANNEL_A4, _adc_event_handler, NULL);
    bc_adc_async_measure(BC_ADC_CHANNEL_A4);
    bc_adc_resolution_set(BC_ADC_CHANNEL_A4, BC_ADC_RESOLUTION_12_BIT);
    bc_adc_oversampling_set(BC_ADC_CHANNEL_A4, BC_ADC_OVERSAMPLING_256);

    // Plan next run this function after 1000 ms
    bc_scheduler_plan_current_from_now(200);
}
