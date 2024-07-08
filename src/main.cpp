#include "pico/stdlib.h"
#include <U8g2lib.h>
#include <stdio.h>
#include "u8g2_port.hpp"

#define LED_PIN 18

int main() {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    stdio_init_all();

    sleep_ms(2000);  // TODO: Delete

    U8G2_CUSTOM u8g2(U8G2_R0);
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0,10,"Hello World!");
    u8g2.sendBuffer();

    while (1);
}