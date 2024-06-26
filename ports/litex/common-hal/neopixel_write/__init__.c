// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 hathach for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/mphal.h"
#include "shared-bindings/neopixel_write/__init__.h"
#include "csr.h"

// ICE40 LED Driver hard macro.
// See http://www.latticesemi.com/-/media/LatticeSemi/Documents/ApplicationNotes/IK/ICE40LEDDriverUsageGuide.ashx?document_id=50668
enum led_registers {
    LEDDCR0 = 8,
    LEDDBR = 9,
    LEDDONR = 10,
    LEDDOFR = 11,
    LEDDBCRR = 5,
    LEDDBCFR = 6,
    LEDDPWRR = 1,
    LEDDPWRG = 2,
    LEDDPWRB = 3,
};

// Control register definitions
#define LEDDCR0_LEDDEN (1 << 7)
#define LEDDCR0_FR250 (1 << 6)
#define LEDDCR0_OUTPOL (1 << 5)
#define LEDDCR0_OUTSKEW (1 << 4)
#define LEDDCR0_QUICKSTOP (1 << 3)
#define LEDDCR0_PWM_MODE (1 << 2)
#define LEDDCR0_BRMSBEXT (1 << 0)

// Write a value into the LEDDA_IP register.
static void ledda_write(uint8_t value, uint8_t addr) {
    rgb_addr_write(addr);
    rgb_dat_write(value);
}

static int ledda_init_done;

static void ledda_init(void) {
    if (ledda_init_done) {
        return;
    }

    // Enable the driver
    rgb_ctrl_write((1 << CSR_RGB_CTRL_EXE_OFFSET) | (1 << CSR_RGB_CTRL_CURREN_OFFSET) | (1 << CSR_RGB_CTRL_RGBLEDEN_OFFSET));

    ledda_write(LEDDCR0_LEDDEN | LEDDCR0_FR250 | LEDDCR0_QUICKSTOP, LEDDCR0);

    // Set clock register to 12 MHz / 64 kHz - 1
    ledda_write((12000000 / 64000) - 1, LEDDBR);

    // Ensure LED "breathe" effect is disabled
    ledda_write(0, LEDDBCRR);
    ledda_write(0, LEDDBCFR);

    // Also disable the LED blink time
    ledda_write(0, LEDDONR);
    ledda_write(0, LEDDOFR);

    ledda_init_done = 1;
}

void common_hal_neopixel_write(const digitalio_digitalinout_obj_t *digitalinout, uint8_t *pixels, uint32_t numBytes) {
    (void)digitalinout;
    (void)numBytes;
    ledda_init();

    ledda_write(pixels[0], LEDDPWRR); // Red
    ledda_write(pixels[1], LEDDPWRG); // Green
    ledda_write(pixels[2], LEDDPWRB); // Blue
}
