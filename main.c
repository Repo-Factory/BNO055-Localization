#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"

#define DEBUG_MODE
#define LED_PIN         25ul    // On board LED
#define FAST_I2C_FREQ   400000  // Fast i2C @ 400kHz
#define I2C_ADDR        0x29    // maybe BNO055
#define I2C_SCL         21
#define I2C_SDA         20

#define OPMODE 0x3D
#define FUSION_MODE 0x0C
#define EULER_YAW 0x1A

void fixedToDecimal(char *str, int16_t fp);
void setup_i2C(i2c_inst_t* i2c);
const uint8_t data_2_ask_for = 0x1A;


///////////////////// MAIN //////////////////////////////
int main(){                 // Main for Core 0, Comms and processing Done here
#ifdef DEBUG_MODE
    stdio_init_all();
#endif
    busy_wait_ms(2000);
    setup_i2C(i2c0);
    uint8_t NDOF_SET[2] = {0x3D, 0x0C};


    // printf("Hello, i2c test time!\n");


    printf("NDOF SET %s\n", (i2c_write_blocking(i2c0, I2C_ADDR, NDOF_SET, 2, 0) > 0) ? "SUCCESS" : "FAILURE");

    while(1){
        if(i2c_get_write_available(i2c0)){
            //printf("Starting Access\n");
        
            uint8_t dst_arr[32] = {0x00};

            int happy = i2c_write_blocking(i2c0, I2C_ADDR, &data_2_ask_for, 1, 1);
            //printf("Write Blocking Return: %d\n", happy);
            if(happy > 0){
                int yeehaw = i2c_read_timeout_us(i2c0, I2C_ADDR, dst_arr, 6, 1, 1000000);
                //printf("Read Timeout Return: %d\n", yeehaw);
            
                int16_t yarr = (int16_t)((((uint16_t)dst_arr[1]) << 8) | dst_arr[0]);
                int16_t roll = (int16_t)((((uint16_t)dst_arr[3]) << 8) | dst_arr[2]);
                int16_t pitch = (int16_t)((((uint16_t)dst_arr[5]) << 8) | dst_arr[4]);

                // printf("Pitch = %d\tRoll = %d\tYarr = %d\n", pitch >> 4, roll >> 4, yarr >> 4);
                fixedToDecimal("Pitch", pitch);
                fixedToDecimal("Roll", roll);
                fixedToDecimal("Yarr", yarr);
                printf("\n");
            }
            busy_wait_ms(15);
        }
    }

}




////////////////////// FUNctions //////////////////////////
void setup_i2C(i2c_inst_t* i2c){
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL);
    gpio_pull_up(I2C_SDA);
    i2c_init(i2c, FAST_I2C_FREQ);
}

void fixedToDecimal(char *str, int16_t fp){
    uint16_t ft = 0;

    for (uint8_t i = 0; i < 4; ++i)
    {
        ft += ((fp >>i) & 0x1) * (625 << i);
    }

    printf("%s\t%d.%d\t", str, fp >> 4, ft);
}