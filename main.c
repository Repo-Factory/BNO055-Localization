#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"
#include "bytes.h"

#define DEBUG_MODE
#define LED_PIN         25ul    // On board LED
#define FAST_I2C_FREQ   400000  // Fast i2C @ 400kHz
#define I2C_ADDR        0x29    // BNO055
#define I2C_SCL         21
#define I2C_SDA         20

#define OPMODE          0x3D
#define NDOF            0x0C
#define EULER_YAW       0x1A

#define SUCCESS         0
#define NOSTOP_TRUE     1
#define NOSTOP_FALSE    0
#define TIMEOUT_MICROSECONDS 1000000

const uint8_t data_2_ask_for = EULER_YAW;

typedef struct Orientation
{
    const int16_t yaw; 
    const int16_t pitch;
    const int16_t roll;
} Orientation;

void setupBNO055();
void setup_UART();
void setup_i2C(i2c_inst_t* i2c);
void fixedToDecimal(char *str, int16_t fp);
void printOrientation(const Orientation* orientation);
void processOrientation(const Orientation* orientation);

///////////////////// MAIN //////////////////////////////
int main() 
{                 
    setupBNO055();
    setup_UART();

    for(;;)
    {
        if(i2c_get_write_available(i2c0)) 
        {        
            uint8_t dst_arr[32] = {INIT_BYTE};
            if(i2c_write_blocking(i2c0, I2C_ADDR, &data_2_ask_for, ONE_BYTE, NOSTOP_TRUE) > SUCCESS) 
            {
                i2c_read_timeout_us(i2c0, I2C_ADDR, dst_arr, SIX_BYTES, NOSTOP_TRUE, TIMEOUT_MICROSECONDS);

                const int16_t yaw   = (int16_t)((((uint16_t)dst_arr[1]) << 8) | dst_arr[0]);
                const int16_t roll  = (int16_t)((((uint16_t)dst_arr[3]) << 8) | dst_arr[2]);
                const int16_t pitch = (int16_t)((((uint16_t)dst_arr[5]) << 8) | dst_arr[4]);
                const Orientation orientation = {yaw, pitch, roll};

                printOrientation(&orientation);
                processOrientation(&orientation);
            }
            busy_wait_ms(15);
        }
    }
}

////////////////////// Functions //////////////////////////
void setupBNO055()
{
    #ifdef DEBUG_MODE
        stdio_init_all();
    #endif
    setup_i2C(i2c0);
    uint8_t NDOF_SET[TWO_BYTES] = {OPMODE, NDOF};
    i2c_write_blocking(i2c0, I2C_ADDR, NDOF_SET, TWO_BYTES, NOSTOP_FALSE);
}

void setup_UART()
{
    const uint BAUD_RATE = 115200;
    const uint TX_PIN = 0;
    uart_init(uart0, BAUD_RATE);
    gpio_set_function(TX_PIN, GPIO_FUNC_UART);
}

void setup_i2C(i2c_inst_t* i2c)
{
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL);
    gpio_pull_up(I2C_SDA);
    i2c_init(i2c, FAST_I2C_FREQ);
}

void printOrientation(const Orientation* orientation)
{
    printf("%d %d %d ", orientation->yaw, orientation->pitch, orientation->roll);
}

void processOrientation(const Orientation* orientation)
{
    uint8_t orientation_data[6] = 
    {
        (orientation->yaw && 0xFF00) >> 8, orientation->yaw && 0x00FF,
        (orientation->pitch && 0xFF00) >> 8, orientation->pitch && 0x00FF,
        (orientation->roll && 0xFF00) >> 8, orientation->roll && 0x00FF,
    };

    for (int i = 0; i < 6; i++)
    {
        uart_putc_raw(uart0, orientation_data[i]);
    }
}

void fixedToDecimal(char* str, int16_t fp)
{
    uint16_t ft = 0;
    for (uint8_t i = 0; i < 4; ++i)
    {
        ft += ((fp >> i) & 0x1) * (625 << i);
    }
    printf("%d.%d", fp >> 4, ft);
}