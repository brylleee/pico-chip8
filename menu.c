#include "display.h"
#include "roms.h"
#include "input.h"

#include "buzzer.h"

#include "chip8.h"

void menu() {
    cls();

    char *text[] = {
        "             ",
        "             ",
        "    CHIP 8   ",
        "   EMULATOR  ",
        "  BY  BRYLLE ",
    };

    int y = 0;
    for (uint i = 0; i < count_of(text); i++) {
        WriteString(SSD1306_BUF, 5, y, text[i]);
        y+=8;
    }

    render(SSD1306_BUF, &frame_area);

    sleep_ms(2000);
    
    int page = 0;

    list:
    cls();

    y = 0;
    for(uint i = 0; i < count_of(names[page]); i++) {
        WriteString(SSD1306_BUF, 5, y, names[page][i]);
        y+=8;
    }

    render(SSD1306_BUF, &frame_area);

    int game = wait_press();
    while(game < 0 || game > 8) {
        buzzer(1); sleep_ms(500); buzzer(0);
        game = wait_press();
    }

    if(game == 0) {
        if(page < count_of(roms)-1)
            page++;
        else
            page = 0;

        goto list;
    }

    cls();
    start(roms[page][game-1]);
    BACK = 0;

    goto list;
}