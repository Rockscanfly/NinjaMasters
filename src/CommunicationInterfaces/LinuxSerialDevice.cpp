// Code provided by
// https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
// accessed 09/04/2020
#include "LinuxSerialDevice.hpp"

#ifdef __linux__
LinuxSerialDevice::LinuxSerialDevice(char* device, int baud)
{

    #ifdef DEBUG
        printf("Opening LinuxSerialDevice\n")
        printf("Device: %s\n", device);
        printf("Baud: %i\n", baud);
    #endif

    // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
    serial_port_ = open(device, O_RDWR);

    if(serial_port_ == -1) {
        printf("Error %i from opening %s: %s\n", errno, device, strerror(errno));
    }

    // Create new termios struc, we call it 'tty' for convention
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    // Read in existing settings, and handle any error
    if(tcgetattr(serial_port_, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate
    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port_, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        exit(1);
    }

    #ifdef DEBUG
        printf("Successfully opened LinuxSerialDevice\n")
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
    // Allocate memory for read buffer, set size according to your needs
    int num_bytes = read(serial_port_, &data, 256);

        if (num_bytes < 0) {
            printf("Error reading: %s\n", strerror(errno));
        }
    #ifdef DEBUG
        else
        {
            printf("Read %i bytes. Received message: %s\n", num_bytes, data);
        }
    #endif
    return num_bytes;
}

int LinuxSerialDevice::Write(char *data)
{
    #ifdef DEBUG
        printf("Writing Data: %s\n", data);
    #endif
    write(serial_port_, data, 256);
	return sizeof(data);
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

