#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

class SerialDevice
{
public:
    SerialDevice(void);
    virtual ~SerialDevice(void);

    virtual int Read(char *data) = 0;
    virtual int Write(char *data) = 0;

    char device_name_[256] = {'\0'};

};


#endif // SERIALDEVICE_H
