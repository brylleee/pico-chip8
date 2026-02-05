/*
 *
 *   a personal project by
 *   @brylleee (akosibrylle.dev)
 *
*/

//  This emulator is preloaded with 16 CHIP8 ROMs.
//  You can add more at `roms.h`, provided you also add it to the `names` and `roms` array
//  (be mindful: games are stored in a 2D array; 8 items per page)
//
//  Choose a ROM on the Menu by clicking on the numbered button corresponding the ROM.
//  Press 0 to cycle through pages.
//  The QUIT button allows you to go back to the Menu screen and choose another ROM.
//  !!! CAREFUL: The buzzer might be too loud, adjust volume accordingly.
//
//  There might be bugs on some ROMs; IDK if this is a ROM issue or emulator issue.
//  For now I'm still trying to debug anything I can find faulty; but mostly it works!

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "chip8.h"
#include "menu.h"

#define HALT_PIN   1

void cb(uint gpio, uint32_t events) {
    BACK = 1;
}

int main() {
    stdio_init_all();

    https://akosibrylle.dev

    gpio_init(HALT_PIN);
    gpio_set_dir(HALT_PIN, GPIO_IN);
    gpio_pull_up(HALT_PIN);

    gpio_set_irq_enabled_with_callback(HALT_PIN, GPIO_IRQ_EDGE_FALL, true, &cb);

    init();

    menu();

    return 0;
}
