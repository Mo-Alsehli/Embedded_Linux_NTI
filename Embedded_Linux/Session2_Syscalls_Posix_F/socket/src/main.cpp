#include "led.h"

int main() {
    std::fstream file("/sys/class/leds/input4::capslock/brightness");
    Led led(file);

    led.turn_capslock_off();
}