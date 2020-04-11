#ifndef LINUXSERIALDEVICE_H
#define LINUXSERIALDEVICE_H

#include "SerialDevice.hpp"

class LinuxSerialDevice : public SerialDevice
{
public:
    LinuxSerialDevice(char* device, int baud);

    ~LinuxSerialDevice(void);

    int Read(char *data);
    int Write(char *data);

private:
    int serial_port_ = 0;

    char rx_buffer_[256] = {'\0'};
    char tx_buffer_[256] = {'\0'};
};


#endif // LINUXSERIALDEVICE_H
