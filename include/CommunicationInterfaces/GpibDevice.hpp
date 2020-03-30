#ifndef GPIBDEVICE_H
#define GPIBDEVICE_H

#include "SerialDevice.hpp"
#include "visawrapper.h"

class GpibDevice
{
public:
    GpibDevice( int major, int minor );
    ~GpibDevice( void );

    int Read( char *data );
    int Write( char *data );

private:
    ViSession resourceManager = 0;
    ViSession device = 0;

};


#endif // GPIBDEVICE_H
