#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

#include <string>
#include "string.h"

class SerialDevice
{
public:
    SerialDevice(void);
    virtual ~SerialDevice(void);

    virtual int Read(char *data);
    virtual int Write(char *data);

    char device_name_[256] = {'\0'};

};


#endif // SERIALDEVICE_H
