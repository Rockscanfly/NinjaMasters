#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

#include <string>
#include "string.h"

class SerialDevice
{
public:
    SerialDevice( void );
    virtual ~SerialDevice( void );

    virtual int Read( char *data );
    virtual int Write( char *data );

};


#endif // SERIALDEVICE_H
