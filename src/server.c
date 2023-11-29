#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERIAL_PORT "/dev/ttyACM0"
#define MAX_BUFFER_SIZE 256
#define MAX_VALUE_SIZE 128

typedef struct Orientation
{
    const int16_t yaw; 
    const int16_t pitch;
    const int16_t roll;
} Orientation;

int openSerialPort(const char* serialPort) {
    int serial_fd = open(serialPort, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serial_fd == -1) {
        perror("Error opening serial port");
        exit(1);
    }
    return serial_fd;
}

void extractValues(const char* buffer, char* values[], size_t bytesRead)
{
    size_t i = 0, j = 0, k = 0;

    for (i = 0; i < bytesRead; i++) 
    {
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

float intToFloat(const int num)
{
    if (num == 0) return 0;
    return (float)(num) / pow(10, floor(log10(abs(num)) + 1));
}

float fixedToDecimal(int16_t fp)
{
    uint16_t ft = 0;
    for (uint8_t i = 0; i < 4; ++i)
    {
        ft += ((fp >> i) & 0x1) * (625 << i);
    }
    float decimalValue = 0;
    decimalValue += ((fp >> 4) + intToFloat(ft));
    return decimalValue;
}

void printOrientation(const Orientation* orientation)
{
    printf("%f ", fixedToDecimal(orientation->yaw));
    printf("%f ", fixedToDecimal(orientation->pitch));
    printf("%f ", fixedToDecimal(orientation->roll));
    printf("\n");
    fflush(stdout);
}

int processOrientation(const Orientation* orientation)
{
    static int setup = 0;
    static int status, valread, client_fd;
    static struct sockaddr_in serv_addr;
    static char buffer[1024] = { 0 };
    static const int PORT = 12121;

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
 
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
 
    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    send(client_fd, orientation, sizeof(orientation), 0);
    printf("Orientation message sent\n");
    valread = read(client_fd, buffer, 1024 - 1);
    printf("%s\n", buffer);
 
    close(client_fd);
    return 0;
}

int main() 
{
    char buffer[MAX_BUFFER_SIZE];
    int serial_fd = openSerialPort(SERIAL_PORT);

    for(;;)
    {
        ssize_t bytesRead = read(serial_fd, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0 && bytesRead < 20) 
        {
            buffer[bytesRead] = '\0';
            
            char  yaw[MAX_VALUE_SIZE], pitch[MAX_VALUE_SIZE], roll[MAX_VALUE_SIZE];
            char* values[] = {yaw, pitch, roll};
            extractValues(buffer, values, bytesRead);

            const Orientation orientation = 
            {
                (int16_t)strtol(yaw, NULL, 10), 
                (int16_t)strtol(pitch, NULL, 10), 
                (int16_t)strtol(roll, NULL, 10)
            };

            printOrientation(&orientation);
            processOrientation(&orientation);
        }
    }

    close(serial_fd);
    return 0;
}