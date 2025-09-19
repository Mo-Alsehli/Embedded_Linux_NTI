// #include "RPI_GPIO_Registers.h"
#include "uart.h"

// Registers for setting pin mode.
#define BCM2837_GPIO_BASE 0x3F200000
#define BCM2837_GPFSEL1 ((volatile unsigned int*) (BCM2837_GPIO_BASE + 0x04))

// Registers for writing and clearing pins
#define BCM2837_GPSET0 ((volatile unsigned int*) (BCM2837_GPIO_BASE + 0x1C))
#define BCM2837_GPCLR0 ((volatile unsigned int*) (BCM2837_GPIO_BASE + 0x28))

// Registers for reading the pin
#define BCM2837_GPLEV0 ((volatile unsigned int*) (BCM2837_GPIO_BASE + 0x34))

void set_gpio17_output() {
    unsigned int r = *BCM2837_GPFSEL1;

    r &= ~(7 << 21);
    r |= (1 << 21);

    *BCM2837_GPFSEL1 = r;
}

void set_gpio17() { *BCM2837_GPSET0 = (1 << 17); }

void clear_gpio17() { *BCM2837_GPCLR0 = (1 << 17); }

int read_gpio17() {
    unsigned int val = *BCM2837_GPLEV0;

    return (val & (1 << 17)) ? 1 : 0;
}

int main() {
    // set up serial console
    uart_init();

    // say hello
    uart_puts("Simple UART App\n");

    set_gpio17_output();
    // set_gpio17();

    while (1) {
        char c = uart_getc();
        if (c == 'y') {
            set_gpio17();
        } else if (c == 'n') {
            clear_gpio17();
        }

        int state = read_gpio17();

        if (state) {
            uart_puts("GPIO 17 is HIGH\n");
        } else {
            uart_puts("GPIO 17 is LOW\n");
        }
    }
}