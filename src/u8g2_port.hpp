#ifndef U8G2_PORT_HPP
#define U8G2_PORT_HPP

uint8_t u8x8_gpio_delay_callback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_i2c_callback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
void u8g2_Setup_custom(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb, u8x8_msg_cb gpio_and_delay_cb);

class U8G2_CUSTOM : public U8G2 {
  public: U8G2_CUSTOM(const u8g2_cb_t *rotation) : U8G2() {
    u8g2_Setup_custom(&u8g2, rotation, u8x8_i2c_callback, u8x8_gpio_delay_callback);
  }
};

#endif