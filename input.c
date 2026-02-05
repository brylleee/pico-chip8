#include "pico/stdlib.h"
#include <stdio.h>

#define DIMENSION   4

const int ROWS[DIMENSION] = {9, 8, 7, 6};
const int COLS[DIMENSION] = {5, 4, 3, 2};

const int BUTTONS[16] = {
  1,   2,  3,   0xC,
  4,   5,  6,   0xD,
  7,   8,  9,   0xE,
  0xA, 0,  0xB, 0xF
};

void init_input() {
    for(int i = 0; i < DIMENSION; i++) {
        gpio_init(ROWS[i]);
        gpio_set_dir(ROWS[i], GPIO_OUT);

        gpio_init(COLS[i]);
        gpio_set_dir(COLS[i], GPIO_IN);
        gpio_pull_down(COLS[i]);
    }
}

int check_press() {
    for(int i = 0; i < DIMENSION; i++) {
        gpio_put(ROWS[i], 1);
        sleep_us(20);

        for(int j = 0; j < DIMENSION; j++) {
            if(gpio_get(COLS[j])) {
                gpio_put(ROWS[i], 0);
                return BUTTONS[j + (i * DIMENSION)];
            }
        }
        gpio_put(ROWS[i], 0);
        sleep_us(20);
    }

    return -1;
}

int wait_press() {
    while(1) {
        int value = check_press();
        if(value >= 0) {
            return value;
        }
    }
}