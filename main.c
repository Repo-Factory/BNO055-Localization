#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/i2c.h"
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
void setup_i2C(i2c_inst_t* i2c);
void fixedToDecimal(char *str, int16_t fp);
void printOrientation(const Orientation* orientation);
void processOrientation(const Orientation* orientation);

///////////////////// MAIN //////////////////////////////
int main() 
{                 
    setupBNO055();
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
    fixedToDecimal("Yaw", orientation->yaw);
    fixedToDecimal("Pitch", orientation->pitch);
    fixedToDecimal("Roll", orientation->roll);
    printf("\n");
}

void processOrientation(const Orientation* orientation)
{
    // int status, valread, client_fd;
    // struct sockaddr_in serv_addr;
    // char buffer[1024] = { 0 };

    // if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    //     printf("\n Socket creation error \n");
    //     return -1;
    // }
 
    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_port = htons(PORT);
 
    // // Convert IPv4 and IPv6 addresses from text to binary
    // // form
    // if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    //     printf(
    //         "\nInvalid address/ Address not supported \n");
    //     return -1;
    // }
 
    // if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
    //     printf("\nConnection Failed \n");
    //     return -1;
    // }

    // send(client_fd, orientation, sizeof(orientation), 0);
    // printf("Orientation message sent\n");
    // valread = read(client_fd, buffer, 1024 - 1); // subtract 1 for the null terminator at the end
    // printf("%s\n", buffer);
 
    // // closing the connected socket
    // close(client_fd);
    // return 0;
}

void fixedToDecimal(char* str, int16_t fp)
{
    uint16_t ft = 0;
    for (uint8_t i = 0; i < 4; ++i)
    {
        ft += ((fp >> i) & 0x1) * (625 << i);
    }
    printf("%s\t%d.%d\t", str, fp >> 4, ft);
}