# Pico Chip-8

An emulator for the original CHIP8 using Raspberry Pi Pico.

![Pico Chip8](https://github.com/user-attachments/assets/ff935a32-c4b7-4386-bd60-df6e3a26a527)

Example video [here](https://github.com/user-attachments/assets/ef6c6fa8-6ed7-4de0-aa72-15863626f58f).
---

### Simulation

You can try it out, although in a slower simulation speed mode [here](https://wokwi.com/projects/453325769567688705) in Wokwi.

### Componets
- Raspberry Pi Pico
- SSD1306 (128x64 display)
- Passive Buzzer
- Tactile pushbutton
- 4x4 keypad

### Pinout
| Component    | Pin | Connect to |
| ---          | --- | ---        |
| **SSD1306**  | VCC | Pico 3.3V  |
|              | GND | Pico GND   |
|              | SCK | GP17       |
|              | SDA | GP16       |
| **Buzzer**   | (+) | GP13       |
|              | (-) | GND        |
| Pushbutton   |  A  | GP2        |
|              |  B  | GND        |
| Keypad       | 1-4 | GP6 - GP9  |
|              | 5-8 | GP2 - GP5  |


### Building and flashing

Clone the repository:

`git clone https://github.com/brylleee/pico-chip8/`

Build:

`cd pico-chip8/build && cmake .. && make -j`

Make sure the Pico is in download mode by holding the **BOOTSEL** button and plugging the USB into your PC.
You can then drag and drop the `CHIP8.uf2` file into the Pico.

---

References:

[Cowgod's Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
