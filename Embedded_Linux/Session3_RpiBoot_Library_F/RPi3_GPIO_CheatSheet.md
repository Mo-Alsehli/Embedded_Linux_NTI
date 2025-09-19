
# Raspberry Pi 3 GPIO Register Cheat Sheet (BCM2837)

**Base Address (ARM Physical):**  
`GPIO_BASE = 0x3F200000` (for Pi 2/3)  
(`0x20200000` on Pi 1 / Zero)

---

## 📌 Function Select Registers (GPFSELx)

Each pin uses **3 bits**.  
`000=input, 001=output, 100-111=ALT0–ALT5`

| GPIO  | Register | Bit Field (3 bits) |
|------|----------|------------------|
| 0–9  | GPFSEL0  | GPIO0 = bits 0–2, GPIO9 = bits 27–29 |
| 10–19| GPFSEL1  | GPIO10 = bits 0–2 … GPIO19 = bits 27–29 |
| 20–29| GPFSEL2  | GPIO20 = bits 0–2 … GPIO29 = bits 27–29 |
| 30–39| GPFSEL3  | GPIO30 = bits 0–2 … GPIO39 = bits 27–29 |
| 40–49| GPFSEL4  | GPIO40 = bits 0–2 … GPIO49 = bits 27–29 |
| 50–53| GPFSEL5  | GPIO50 = bits 0–2 … GPIO53 = bits 12–14 |

**Bit position formula:**

```c
register = GPFSEL_BASE + ((gpio / 10) * 4);
bitfield = (gpio % 10) * 3;
```

---

## 📌 Output Set / Clear Registers

**Write 1 to set/clear a pin.**  
(Write 0 → no effect)

| Register | Covers GPIO | Bit Position |
|---------|-------------|-------------|
| GPSET0 / GPCLR0 | 0–31  | Bit = GPIO number |
| GPSET1 / GPCLR1 | 32–53 | Bit = (GPIO − 32) |

Example:

```c
GPSET0 |= (1 << 17);   // Set GPIO17 HIGH
GPCLR0 |= (1 << 17);   // Set GPIO17 LOW
```

---

## 📌 Level Registers (Read-Only)

Read current logic state (0/1).

| Register | Covers GPIO | Bit Position |
|---------|-------------|-------------|
| GPLEV0 | 0–31  | Bit = GPIO number |
| GPLEV1 | 32–53 | Bit = (GPIO − 32) |

---

## 📌 Pull-Up / Pull-Down Control

Sequence:

1. Write mode to `GPPUD` (`00=off, 01=pull-down, 10=pull-up`)
2. Wait 150 cycles
3. Write `1 << gpio` to `GPPUDCLK0/1`
4. Wait 150 cycles
5. Clear `GPPUD` and `GPPUDCLKx`

| Register | Covers GPIO |
|---------|-------------|
| GPPUDCLK0 | GPIO 0–31 |
| GPPUDCLK1 | GPIO 32–53 |

---

## 🔧 Quick Example (GPIO17 Output)

```c
#define GPIO_BASE 0x3F200000
#define GPFSEL1   ((volatile unsigned int*)(GPIO_BASE + 0x04))
#define GPSET0    ((volatile unsigned int*)(GPIO_BASE + 0x1C))
#define GPCLR0    ((volatile unsigned int*)(GPIO_BASE + 0x28))

void gpio17_on() {
    unsigned int r = *GPFSEL1;
    r &= ~(7 << 21);  // clear bits for GPIO17
    r |= (1 << 21);   // set as output
    *GPFSEL1 = r;

    *GPSET0 = (1 << 17); // set high
}
```

---

## 🗺 GPIO-to-Register Map (Summary)

| GPIO | Function Select Reg / Bits | SET/CLR Reg / Bit | LEVEL Reg / Bit |
|-----|----------------------------|------------------|----------------|
| 0–9 | GPFSEL0, (gpio*3)          | GPSET0/GPCLR0, bit=gpio | GPLEV0, bit=gpio |
| 10–19| GPFSEL1, ((gpio-10)*3)    | GPSET0/GPCLR0    | GPLEV0 |
| 20–29| GPFSEL2                   | GPSET0/GPCLR0    | GPLEV0 |
| 30–31| GPFSEL3 (only first 2 pins)| GPSET0/GPCLR0   | GPLEV0 |
| 32–39| GPFSEL3 (rest)            | GPSET1/GPCLR1, bit=(gpio-32) | GPLEV1 |
| 40–49| GPFSEL4                   | GPSET1/GPCLR1    | GPLEV1 |
| 50–53| GPFSEL5                   | GPSET1/GPCLR1    | GPLEV1 |
