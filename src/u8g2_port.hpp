#ifndef U8G2_PORT_HPP
#define U8G2_PORT_HPP

#include <U8g2lib.h>

void u8g2_Setup_uc1698_240x64_f(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb, u8x8_msg_cb gpio_and_delay_cb);
uint8_t u8x8_gpio_delay_callback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_spi_callback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

class U8G2_UC1698_240X64_F_4W_HW_SPI : public U8G2 {
  public: U8G2_UC1698_240X64_F_4W_HW_SPI(const u8g2_cb_t *rotation) : U8G2() {
    u8g2_Setup_uc1698_240x64_f(&u8g2, rotation, u8x8_spi_callback, u8x8_gpio_delay_callback);
  }
};

#endif