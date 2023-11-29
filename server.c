#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <string.h>
#include <stdint.h>

#define SERIAL_PORT "/dev/ttyACM0"  // Change this to match your serial port
#define MAX_BUFFER_SIZE 256
#define MAX_VALUE_SIZE 16

typedef struct Orientation
{
    const int16_t yaw; 
    const int16_t pitch;
    const int16_t roll;
} Orientation;

int openSerialPort(const char *serialPort) {
    int serial_fd = open(serialPort, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serial_fd == -1) {
        perror("Error opening serial port");
        exit(1);
    }
    return serial_fd;
}

ssize_t readSerialData(int serial_fd, char *buffer, size_t bufferSize) {
    ssize_t bytesRead = read(serial_fd, buffer, bufferSize - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
    }
    return bytesRead;
}

void extractValue(char* destination, char** source)
{
    while (**source != ' ') {
        *destination++ = *(*source)++;
    }
    *destination = '\0';
    (*source)++; // Move to the next character after the space
}

void fixedToDecimal(int16_t fp)
{
    uint16_t ft = 0;
    for (uint8_t i = 0; i < 4; ++i)
    {
        ft += ((fp >> i) & 0x1) * (625 << i);
    }
    printf("%d.%d ", fp >> 4, ft);
}

void printOrientation(const Orientation* orientation)
{
    fixedToDecimal(orientation->yaw);
    fixedToDecimal(orientation->pitch);
    fixedToDecimal(orientation->roll);
    printf("\n");
    fflush(stdout);
}

void extractValues(const char* buffer, char* values[], size_t numValues)
{
    size_t i = 0, j = 0, k = 0;

    for (i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == ' ' || buffer[i] == '\0')
        {
            values[k][j] = '\0';
            j=0; k++;
        } 
        else
        {
            values[k][j++] = buffer[i];
        }
    }
}

int main() 
{
    char buffer[MAX_BUFFER_SIZE];
    int serial_fd = openSerialPort(SERIAL_PORT);

    for(;;)
    {
        ssize_t bytesRead = read(serial_fd, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) 
        {
            buffer[bytesRead] = '\0';
            
            char  yaw[MAX_VALUE_SIZE], pitch[MAX_VALUE_SIZE], roll[MAX_VALUE_SIZE];
            char* values[] = {yaw, pitch, roll};
            extractValues(buffer, values, sizeof(values) / sizeof(values[0]));

            const Orientation orientation = 
            {
                (int16_t)strtol(yaw, NULL, 10), 
                (int16_t)strtol(pitch, NULL, 10), 
                (int16_t)strtol(roll, NULL, 10)
            };

            printOrientation(&orientation);
        }
    }

    close(serial_fd);
    return 0;
}