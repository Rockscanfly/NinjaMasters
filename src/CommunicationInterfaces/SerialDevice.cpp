#include "SerialDevice.hpp"

SerialDevice::SerialDevice(void)
{
    #if DEBUG
        fprintf(stderr, "Start of SerialDevice Constructor\n");
    #endif // DEBUG

    #if DEBUG
        fprintf(stderr, "End of SerialDevice Constructor\n");
    #endif // DEBUG
}

SerialDevice::~SerialDevice(void)
{
    #if DEBUG
        fprintf(stderr, "Start of SerialDevice Destructor\n");
    #endif // DEBUG

    #if DEBUG
        fprintf(stderr, "End of SerialDevice Destructor\n");
    #endif // DEBUG
}

