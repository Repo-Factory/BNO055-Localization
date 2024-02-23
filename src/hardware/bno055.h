#include <stdint.h>

#define ZERO_BYTES  0
#define ONE_BYTE    1
#define TWO_BYTES   2
#define THREE_BYTES 3
#define FOUR_BYTES  4
#define FIVE_BYTES  5
#define SIX_BYTES   6 
#define SEVEN_BYTES 7
#define EIGHT_BYTES 8
#define INIT_BYTE   0

#define DEBUG_MODE
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