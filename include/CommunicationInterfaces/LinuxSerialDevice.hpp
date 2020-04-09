#ifndef LINUXSERIALDEVICE_H
#define LINUXSERIALDEVICE_H

#include "SerialDevice.hpp"

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


class LinuxSerialDevice : public SerialDevice
{
public:
    LinuxSerialDevice(char* device, int baud);

    ~LinuxSerialDevice(void);

    int Read(char *data);
    int Write(char *data);

private:
    int serial_port_ = 0;
    unsigned char msg_[256] = {'\0'};

};


#endif // LINUXSERIALDEVICE_H
