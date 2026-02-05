#include "pico/stdlib.h"
#include "hardware/structs/systick.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stack.h"
#include "chip8.h"
#include "display.h"
#include "input.h"
#include "buzzer.h"

uint8_t BACK = 0;

const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

uint8_t display[64 * 32];

uint8_t ram[4096];
struct Stack* stack = NULL;   // returns stack with stack pointer

uint8_t v[16];    // general-purpose register
uint16_t vi;       // register for memory addresses

uint16_t pc;       // program counter

uint8_t timer_delay;
uint8_t timer_sound;

const uint16_t START_ADDRESS = 0x200;

void init() {
    printf("[+] Initializing emulator...\n");

    printf("[+] Initializing display...\n");
    display_init();

    printf("[+] Initializing inputs...\n");
    init_input();

    printf("[+] Initializing stack...\n");
    // Init stack
    stack = init_stack();

    printf("[+] Loading buzzer...\n");
    init_buzzer();

    printf("[+] Loading font...\n");
    // Load the font
    for(int i = 0; i < sizeof(fontset); i++) {
        ram[i + 0x050] = fontset[i];
    }

    printf("[+] DONE!\n");
}

void start(uint8_t* rom) {
    memcpy(ram + 0x200, rom, 4096 - 0x200);

    // reset registers, timers, and display for every new ROM
    memset(display, 0, sizeof(display));
    memset(v, 0, sizeof(v));
    vi = 0;
    timer_delay = 0;
    timer_sound = 0;
    pc = 0x200;

    // Timer to mimic the timing of CHIP8
    // 700hz for instructions; 60hz for timers
    systick_hw->csr = 0x0;
    systick_hw->rvr = 0x21D2DA;
    systick_hw->csr = 0x5;

    while(1) {
        if(BACK)
            return;

        if(systick_hw->csr & 0x10000) {
            if (timer_delay > 0) timer_delay--;
            if (timer_sound > 0) {
                buzzer(1);
                timer_sound--;
            } else {
                buzzer(0);
            }

            // 700 / 60 = ~11
            // runs 11 opcodes everytime timers are decremented at 60hz
            for(int i = 0; i < 11; i++) {
                uint16_t instruction = (ram[pc] << 8) | ram[pc+1];

                pc += 2;
                decode(instruction);
            }
        }
    }
}

void decode(uint16_t instruction) {
    //printf("[[%04X]]", instruction);
    switch((instruction >> 12) & 0x0F) {
        case 0x0:
            if((instruction & 0xfff) == 0x0E0) {
                memset(display, 0, 64*32);
                cls();
                break;
            }

            if((instruction & 0xfff) == 0X0EE) {
                pc = pop_stack(stack);
            }
        break;

        case 0x1:
            pc = (instruction & 0xfff);  // jmp
        break;

        case 0x2:
            push_stack(stack, pc);
            pc = (instruction & 0xfff);
        break;

        case 0x3:
            if((v[(instruction & 0xf00) >> 8]) == (instruction & 0xff)) {
                pc += 2;
            }
        break;

        case 0x4:
            if((v[(instruction & 0xf00) >> 8]) != (instruction & 0xff)) {
                pc += 2;
            }
        break;

        case 0x5:
            if(v[(instruction >> 8 & 0xf)] == v[(instruction >> 4) & 0xf]) {
                pc += 2;
            }
        break;

        case 0x6:
            // LD Vx, KK
            v[(instruction >> 8) & 0xf] = instruction & 0xff;
        break;

        case 0x7:
            // ADD Vx, KK
            v[(instruction >> 8) & 0xf] += (instruction & 0xff);
        break;

        case 0x8:
            // LD Vx, Vy
            if((instruction & 0xf)== 0) {
                v[(instruction >> 8) & 0xf] = v[(instruction >> 4) & 0xf];
                break;
            }
            
            // OR Vx, Vy
            if((instruction & 0xf) == 1) {
                v[(instruction >> 8) & 0xf] = v[(instruction >> 8) & 0xf] | v[(instruction >> 4) & 0xf];
                break;
            }

            // AND Vx, Vy
            if((instruction & 0xf) == 2) {
                v[(instruction >> 8) & 0xf] = v[(instruction >> 8) & 0xf] & v[(instruction >> 4) & 0xf];
                break;
            }

            // XOR Vx, Vy
            if((instruction & 0xf) == 3) {
                v[(instruction >> 8) & 0xf] = v[(instruction >> 8) & 0xf] ^ v[(instruction >> 4) & 0xf];
                break;
            }

            // ADD Vx, Vy
            if((instruction & 0xf) == 4) { // ADD Vx, Vy
                uint8_t x = (instruction >> 8) & 0x0F;
                uint8_t y = (instruction >> 4) & 0x0F;
                uint16_t sum = v[x] + v[y];

                v[15] = (sum > 255) ? 1 : 0;
                v[x] = (uint8_t)(sum & 0xFF);
                break;
            }

            // SUB Vx, Vy
            if((instruction & 0xf) == 5) {
                uint8_t carry = 0;
                uint8_t diff = v[(instruction >> 8) & 0xf] - v[(instruction >> 4) & 0xf];
                if(v[(instruction >> 8) & 0xf] >= v[(instruction >> 4) & 0xf]) {
                    carry = 0x1;
                }

                v[(instruction >> 8) & 0xf] = diff;
                v[15] = carry;
                break;
            }

            // SHR Vx
            if((instruction & 0xf) == 6) {
                uint8_t carry = 0;
                
                if((v[(instruction >> 8) & 0xf] & 0x1)) {
                    carry = 0x1;
                }

                v[(instruction >> 8) & 0xf] = v[(instruction >> 8) & 0xf] / 2;
                v[15] = carry;
                break;
            }

            // SUBN Vx, Vy
            if((instruction & 0xf) == 7) {
                uint8_t carry = 0;
                uint8_t diff = v[(instruction >> 4) & 0xf] - v[(instruction >> 8) & 0xf];
                if(v[(instruction >> 4) & 0xf] >= v[(instruction >> 8) & 0xf]) {
                    carry = 0x1;
                }

                v[(instruction >> 8) & 0xf] = diff;
                v[15] = carry;
                break;
            }

            // SHL Vx
            if((instruction & 0xf) == 0x000E) {
                uint8_t carry = 0;
                
                if((v[(instruction >> 8) & 0xf] & 0x80)) {
                    carry = 0x1;
                }

                v[(instruction >> 8) & 0xf] = v[(instruction >> 8) & 0xf] * 2;
                v[15] = carry;
            }
        break;

        case 0x9:
            // SNE Vx, Vy
            if(v[(instruction >> 8 & 0xf)] != v[(instruction >> 4) & 0xf]) {
                pc += 2;
            }
        break;

        case 0xA:
            vi = (instruction & 0x0FFF);
        break;

        case 0xB:
            pc = (v[0] + (instruction & 0x0FFF));
        break;

        case 0xC:
            v[(instruction >> 8) & 0x0F] = rand() & (instruction & 0x00FF);
        break;

        case 0xD: {
            uint8_t n = (instruction & 0x000F);
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t y = (instruction >> 4) & 0xF;

            v[15] = 0;

            // fill `display` bitmap with `n` bytes starting from `Vi` register
            for(int i = 0; i < n; i++) {
                for(int j = 0; j < 8; j++) {
                    uint8_t pixel = ((ram[vi + i] >> (7 - j)) & 1);

                    if(pixel) {
                      int offset_x = ((v[x] + j) % 64);
                      int offset_y = ((v[y] + i) % 32);

                      if(display[offset_x + (offset_y << 6)]) {
                          v[15] = 1;
                      }

                      display[offset_x + (offset_y << 6)] ^= pixel;  
                    }
                }
            }

            draw_screen(display);
            break;
        }
        break;

        case 0xE:
            // SKP Vx
            if((instruction & 0x00FF) == 0x9E) {
                if(v[(instruction >> 8) & 0x0F] == (uint8_t)check_press())
                    pc += 2;
                break;
            }

            // SKNP Vx
            if((instruction & 0x00FF) == 0xA1) {
                if(v[(instruction >> 8) & 0x0F] != (uint8_t)check_press())
                    pc += 2;
                break;
            }
        break;

        case 0xF:
            // LD Vx, DT
            if((instruction & 0x00FF) == 0x07) {
                v[(instruction >> 8) & 0x0F] = timer_delay;
                break;
            }

            // LD Vx, K
            if((instruction & 0x00FF) == 0x0A) {
                v[(instruction >> 8) & 0x0F] = wait_press();
                break;
            }

            // LD DT, Vx
            if((instruction & 0x00FF) == 0x15) {
                timer_delay = v[(instruction >> 8) & 0x0F];
                break;
            }

            // LD ST, Vx
            if((instruction & 0x00FF) == 0x18) {
                timer_sound = v[(instruction >> 8) & 0x0F];
                break;
            }

            // ADD I, Vx
            if((instruction & 0x00FF) == 0x1E) {
                vi = vi + v[(instruction >> 8) & 0x0F];
                break;
            }

            // LD F, Vx
            if((instruction & 0x00FF) == 0x29) {
                vi = 0x050 + (v[(instruction >> 8) & 0x0F] * 5);
                break;
            }

            // LD B, Vx
            if((instruction & 0x00FF) == 0x33) {
                uint8_t value = v[(instruction >> 8) & 0x0F];
                ram[vi]     = value / 100;          // Hundreds digit
                ram[vi + 1] = (value / 10) % 10;    // Tens digit
                ram[vi + 2] = value % 10;           // Ones digit
                break;
            }

            // LD [I], Vx
            if((instruction & 0x00FF) == 0x55) {
                for(int i = 0; i <= ((instruction >> 8) & 0x0F); i++) {
                    ram[vi + i] = v[i];
                }
            }

            // LD Vx, [I]
            if((instruction & 0x00FF) == 0x65) {
                for(int i = 0; i <= ((instruction >> 8) & 0x0F); i++) {
                    v[i] = ram[vi + i];
                }
                break;
            }
        break;
    }
}