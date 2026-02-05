#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define BUZZER_PIN   13
uint slice;

void init_buzzer() {
    gpio_init(BUZZER_PIN);
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);

    slice = pwm_gpio_to_slice_num(BUZZER_PIN);

    pwm_set_clkdiv(slice, 125.0f);
    pwm_set_wrap(slice, 2272);
    pwm_set_chan_level(slice, PWM_CHAN_B, 1136);

    pwm_set_enabled(slice, false);
}

void buzzer(int value) {
    pwm_set_enabled(slice, value);
}