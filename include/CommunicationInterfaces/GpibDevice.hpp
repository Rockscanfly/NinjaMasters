#ifndef GPIBDEVICE_H
#define GPIBDEVICE_H

#include "SerialDevice.hpp"
#include "visawrapper.h"

class GpibDevice : public SerialDevice
{
public:
    GpibDevice(int board_index, int primary_address);
    ~GpibDevice(void);

    int Read(char *data);
    int Write(char *data);

private:
    ViSession resourceManager_ = 0;
    ViSession device_ = 0;

};


#endif // GPIBDEVICE_H
