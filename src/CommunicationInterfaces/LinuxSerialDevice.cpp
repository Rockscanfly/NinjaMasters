// Code provided by
//https://github.com/xanthium-enterprises/Serial-Port-Programming-on-Linux/blob/master/USB2SERIAL_Read/Reciever%20(PC%20Side)/SerialPort_read.c
// accessed 11/04/2020
// https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
// accessed 09/04/2020

// C library headers
#include <stdio.h>
#include <string.h>
#include <cstdlib>

#ifdef __linux__
// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#endif //__linux__

#include "LinuxSerialDevice.hpp"

#ifdef __linux__
LinuxSerialDevice::LinuxSerialDevice(char* device, int baud)
{

    #ifdef DEBUG
        printf("Opening LinuxSerialDevice\n");
        printf("Device: %s\n", device);
        printf("Baud: %i\n", baud);
    #endif

    // Open the serial port
    serial_port_ = open(device, O_RDWR | O_NOCTTY);
                                    /* O_RDWR   - Read/Write access to serial port       */
                                    /* O_NOCTTY - No terminal will control the process   */
                                    /* Open in blocking mode,read will wait              */


    if(serial_port_ == -1) {
        printf("Error %i from opening %s: %s\n", errno, device, strerror(errno));
        exit(1);
    }else{
        printf("Successfully opened serial port\n");
    }

    struct termios serial_port_settings;  /* Create the structure                          */

    tcgetattr(serial_port_, &serial_port_settings); /* Get the current attributes of the Serial port */

    /* Setting the Baud rate */
    cfsetispeed(&serial_port_settings, baud); /* Set Read  Speed as 9600                       */
    cfsetospeed(&serial_port_settings, baud); /* Set Write Speed as 9600                       */

    /* 8N1 Mode */
    serial_port_settings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
    serial_port_settings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    serial_port_settings.c_cflag &= ~CSIZE;    /* Clears the mask for setting the data size             */
    serial_port_settings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */

    serial_port_settings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
    serial_port_settings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */


    serial_port_settings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
    serial_port_settings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

    serial_port_settings.c_oflag &= ~OPOST;/*No Output Processing*/

    /* Setting Time outs */
    serial_port_settings.c_cc[VMIN] = 1; /* Read at least 1 characters */
    serial_port_settings.c_cc[VTIME] = 1; /* Wait up to 10 miliseconds from first character recieved */


    // Save tty settings, also checking for error
    if (tcsetattr(serial_port_, TCSANOW, &serial_port_settings) != 0)
    {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        exit(1);
    }
    else
    {
        printf("  BaudRate = %i \n  StopBits = 1 \n  Parity   = none\n", baud);
    }

    tcflush(serial_port_, TCIFLUSH);   /* Discards old data in the rx buffer */

    #ifdef DEBUG
        printf("Successfully opened LinuxSerialDevice\n");
    #endif
}

LinuxSerialDevice::~LinuxSerialDevice(void)
{
    #ifdef DEBUG
        printf("Closing LinuxSerialDevice\n");
    #endif

    if (serial_port_ != -1)
    {
        close(serial_port_);
    }
}

int LinuxSerialDevice::Read(char *data)
{
    #ifdef DEBUG
        printf("LinuxSerialDevice Read()\n");
    #endif

    memset(&rx_buffer_, '\0', 256); // clear data buffer
    int new_char = 0;
    int total_bytes = 0;
    int new_bytes = 0;

    do
    {
        new_bytes = read(serial_port_, &rx_buffer_[total_bytes], 256);
        total_bytes += new_bytes;

        if (new_bytes < 0) {
            printf("Error reading: %s\n", strerror(errno));
        }

        if (new_bytes > 255)
        {
            printf("Error: Read too many bytes: %i\n", new_bytes);
            break;
        }
    } while (rx_buffer_[total_bytes-1] != '\n');

    snprintf(data, total_bytes, "%s", rx_buffer_);
    #ifdef DEBUG
    printf("Read %i bytes. Received message: %s\n", total_bytes, data);
    #endif


// int num_bytes = read(serial_port_, &rx_buffer_, 256);

// else if (num_bytes > 256) {
//     printf("Error read too many bytes: %i\n", num_bytes);
// }
// else
// {
//     snprintf(data, num_bytes+1, "%s\n", rx_buffer_);
//     #ifdef DEBUG
//     printf("Read %i bytes. Received message: %s\n", num_bytes, data);
//     #endif
// }
    return total_bytes;
}

int LinuxSerialDevice::Write(char *data)
{
    #ifdef DEBUG
        printf("LinuxSerialDevice Write()\n");
    #endif
    memset(&tx_buffer_, '\0', 256); // clear data buffer
    int num_bytes_to_send = 0;
    num_bytes_to_send = snprintf(tx_buffer_, 256, "%s\r\n", data);

    #ifdef DEBUG
        printf("Writing %i bytes: %s\n", num_bytes_to_send, tx_buffer_);
    #endif

    int num_bytes_sent = write(serial_port_, tx_buffer_, num_bytes_to_send);

        if (num_bytes_sent == 0)
        {
            printf("Error no bytes written\n");
        }
    #ifdef DEBUG
        else
        {
            printf("Data written: %i bytes\n", num_bytes_sent);
        }
    #endif
	return num_bytes_sent;
}
#endif // __linux__

#ifndef __linux__
LinuxSerialDevice::LinuxSerialDevice(char* device, int baud)
{
    printf("Error: cannot access a linux device on a non-linux operating system\n");
    exit(1);
}

LinuxSerialDevice::~LinuxSerialDevice(void) {}

LinuxSerialDevice::Read(char *data) { return 0; }

LinuxSerialDevice::Write(char *data) { return 0; }
#endif // __linux__


// // Write to serial port
// unsigned char msg[] = { 'H', 'e', 'l', 'l', 'o', '\r' };
// write(serial_port_, "Hello, world!", sizeof(msg));

// // Allocate memory for read buffer, set size according to your needs
// char read_buf [256];
// memset(&read_buf, '\0', sizeof(read_buf);

// // Read bytes. The behaviour of read() (e.g. does it block?,
// // how long does it block for?) depends on the configuration
// // settings above, specifically VMIN and VTIME
// int num_bytes = read(serial_port_, &read_buf, sizeof(read_buf));

// // n is the number of bytes read. n may be 0 if no bytes were received, and can also be -1 to signal an error.
// if (num_bytes < 0) {
//     printf("Error reading: %s", strerror(errno));
// }

// // Here we assume we received ASCII data, but you might be sending raw bytes (in that case, don't try and
// // print it to the screen like this!)
// printf("Read %i bytes. Received message: %s", num_bytes, read_buf);

