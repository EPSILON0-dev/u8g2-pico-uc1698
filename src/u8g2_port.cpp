#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "u8g2_port.hpp"

#define SDA_PIN 2
#define SCK_PIN 3
#define CD_PIN 4
#define CS_PIN 5
#define RESET_PIN 6


// ---------------------------------------------------------------------------------
// CALLBACKS

// Due to my stupidity this has to be done in software
static void spiSend(uint8_t data)
{
    // Just look at these 32 nops, truly marvelous
    for (uint8_t i = 0; i < 8; i++) {
        gpio_put(SDA_PIN, (data >> (7-i)) & 1);
        gpio_put(SCK_PIN, 0);
        __asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
        __asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
        gpio_put(SCK_PIN, 1);
        __asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
        __asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
    }
}

uint8_t u8x8_gpio_delay_callback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch(msg)
    {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:	// called once during init phase of u8g2/u8x8
        gpio_init(SDA_PIN);
        gpio_set_dir(SDA_PIN, GPIO_OUT);
        gpio_put(SDA_PIN, 1);
        gpio_init(SCK_PIN);
        gpio_set_dir(SCK_PIN, GPIO_OUT);
        gpio_put(SCK_PIN, 1);
        gpio_init(CS_PIN);
        gpio_set_dir(CS_PIN, GPIO_OUT);
        gpio_put(CS_PIN, 1);
        gpio_init(CD_PIN);
        gpio_set_dir(CD_PIN, GPIO_OUT);
        gpio_put(CS_PIN, 1);
        gpio_init(RESET_PIN);
        gpio_set_dir(RESET_PIN, GPIO_OUT);
        gpio_put(RESET_PIN, 1);
        break;

        case U8X8_MSG_DELAY_NANO:  // delay arg_int * 1 nano second
        break;    

        case U8X8_MSG_DELAY_100NANO:  // delay arg_int * 100 nano seconds
        sleep_us(arg_int / 10);
        break;

        case U8X8_MSG_DELAY_10MICRO:  // delay arg_int * 10 micro seconds
        sleep_us(arg_int * 10);
        break;
        
        case U8X8_MSG_DELAY_MILLI:  // delay arg_int * 1 milli second
        sleep_ms(arg_int);
        break;

        case U8X8_MSG_GPIO_CS:  // CS (chip select) pin: Output level in arg_int
        gpio_put(CS_PIN, arg_int);
        break;

        case U8X8_MSG_GPIO_DC:  // DC (data/cmd, A0, register select) pin: Output level in arg_int
        gpio_put(CD_PIN, arg_int);
        break;

        case U8X8_MSG_GPIO_RESET:  // Reset pin: Output level in arg_int
        gpio_put(RESET_PIN, arg_int);
        break;

        default:
        u8x8_SetGPIOResult(u8x8, 1);  // default return value
        break;
    }

    return 1;
}

uint8_t u8x8_spi_callback(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) 
{
    uint8_t *data;
    uint8_t internal_spi_mode; 

    switch(msg) {

        case U8X8_MSG_BYTE_SEND:
        data = (uint8_t*)arg_ptr;
        while (arg_int > 0) {
            // TODO: Use the hardware SPI once the new board is made :/
            spiSend((uint8_t)*data); // It's the one above
            data++;
            arg_int--;
        }  
        break;

        case U8X8_MSG_BYTE_INIT:
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
        break;

        case U8X8_MSG_BYTE_SET_DC:
        u8x8_gpio_SetDC(u8x8, arg_int);
        break;

        case U8X8_MSG_BYTE_START_TRANSFER:
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);  
        u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, NULL);
        break;

        case U8X8_MSG_BYTE_END_TRANSFER:      
        u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->pre_chip_disable_wait_ns, NULL);
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
        break;

        default:
        return 0;
    }  
    return 1;
}

void u8g2_Setup_uc1698_240x64_f(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb, u8x8_msg_cb gpio_and_delay_cb)
{
    uint8_t tile_buf_height;
    uint8_t *buf;
    u8g2_SetupDisplay(u8g2, u8x8_d_pcd8544_84x48, u8x8_cad_001, byte_cb, gpio_and_delay_cb);
    buf = u8g2_m_30_8_f(&tile_buf_height);
    u8g2_SetupBuffer(u8g2, buf, tile_buf_height, u8g2_ll_hvline_vertical_top_lsb, rotation);
}

static const uint8_t u8x8_d_uc1698_240x64_init_seq[] = {
    U8X8_START_TRANSFER(),
    
    U8X8_C(0x2B),  // Power control
    U8X8_C(0x25),  // Temperature compensation
    U8X8_C(0xAD),  // LCD Enable
    U8X8_C(0xD8),  // Scan function
    U8X8_C(0xD5),  // Color mode
    U8X8_C(0xF8),  // Window program mode
    U8X8_C(0x89),  // Address control
    U8X8_C(0xC0),  // LCD mapping
    U8X8_C(0x50),  // Scroll high
    U8X8_C(0x40),  // Scroll low
    U8X8_C(0x81),  // Set contrast
    U8X8_C(0x60),  // Contrast value
        
    U8X8_END_TRANSFER(),             	/* disable chip */
    U8X8_END()             			/* end of sequence */
};

static const uint8_t u8x8_d_uc1698_240x64_powersave0_seq[] = {
    U8X8_START_TRANSFER(),
    // TODO: Add powerup sequence
    U8X8_END_TRANSFER(),
    U8X8_END()
};

static const uint8_t u8x8_d_uc1698_240x64_powersave1_seq[] = {
    U8X8_START_TRANSFER(),
    // TODO: Add powerdown sequence
    U8X8_END_TRANSFER(),
    U8X8_END()
};



static const u8x8_display_info_t u8x8_uc1698_240x64_display_info =
{
    /* chip_enable_level = */ 0,
    /* chip_disable_level = */ 1,
    
    /* post_chip_enable_wait_ns = */ 5,
    /* pre_chip_disable_wait_ns = */ 5,
    /* reset_pulse_width_ms = */ 10, 
    /* post_reset_wait_ms = */ 50, 
    /* sda_setup_time_ns = */ 12,  // Doesn't matter
    /* sck_pulse_width_ns = */ 75,  // Doesn't matter
    /* sck_clock_hz = */ 4000000UL,  // Doesn't matter
    /* spi_mode = */ 0,  // Doesn't matter
    /* i2c_bus_clock_100kHz = */ 4,  // Doesn't matter
    /* data_setup_time_ns = */ 30,  // Doesn't matter
    /* write_pulse_width_ns = */ 40,  // Doesn't matter
    /* tile_width = */ 30,
    /* tile_height = */ 8,
    /* default_x_offset = */ 0,
    /* flipmode_x_offset = */ 0,
    /* pixel_width = */ 240,
    /* pixel_height = */ 64
};

uint8_t u8x8_d_pcd8544_84x48(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    uint8_t *ptr;
    uint8_t r, y, d[3];
    int8_t x;
    switch(msg)
    {
        case U8X8_MSG_DISPLAY_SETUP_MEMORY:
        u8x8_d_helper_display_setup_memory(u8x8, &u8x8_uc1698_240x64_display_info);
        break;

        case U8X8_MSG_DISPLAY_INIT:
        u8x8_d_helper_display_init(u8x8);
        u8x8_cad_SendSequence(u8x8, u8x8_d_uc1698_240x64_init_seq);
        break;

        case U8X8_MSG_DISPLAY_SET_POWER_SAVE:
        if (arg_int == 0)
            u8x8_cad_SendSequence(u8x8, u8x8_d_uc1698_240x64_powersave0_seq);
        else
            u8x8_cad_SendSequence(u8x8, u8x8_d_uc1698_240x64_powersave1_seq);
        break;
        
        case U8X8_MSG_DISPLAY_SET_CONTRAST:
        u8x8_cad_StartTransfer(u8x8);
        u8x8_cad_SendCmd(u8x8, 0x81);
        u8x8_cad_SendCmd(u8x8, arg_int);
        u8x8_cad_EndTransfer(u8x8);
        break;

        // The call format is known so we can do some trickery to make it work with our weird-ass
        //  controller that takes 6 pixels every 3 transfers
        case U8X8_MSG_DISPLAY_DRAW_TILE:
        u8x8_cad_StartTransfer(u8x8);
        ptr = ((u8x8_tile_t*)arg_ptr)->tile_ptr;

        for (y = 0; y < 8; y++) {
            r = ((u8x8_tile_t*)arg_ptr)->y_pos << 3 | y;
            u8x8_cad_SendCmd(u8x8, 0x10);  // X = 0
            u8x8_cad_SendCmd(u8x8, 0x00);
            u8x8_cad_SendCmd(u8x8, 0x70 | (r >> 4));  // Y = tile * 8 + r
            u8x8_cad_SendCmd(u8x8, 0x60 | (r & 0xf));
            
            for (x = 39; x >= 0; x--) {
                d[0] = ((ptr[x * 6 + 3] >> y) & 1) * 240 |
                    ((ptr[x * 6 + 4] >> y) & 1) * 15;
                d[1] = ((ptr[x * 6 + 5] >> y) & 1) * 240 |
                    ((ptr[x * 6 + 0] >> y) & 1) * 15;
                d[2] = ((ptr[x * 6 + 1] >> y) & 1) * 240 |
                    ((ptr[x * 6 + 2] >> y) & 1) * 15;
                u8x8_cad_SendData(u8x8, 3, d);
            }
        }

        u8x8_cad_EndTransfer(u8x8);
        break;

        default:
        return 0;
    }
    return 1;
}


