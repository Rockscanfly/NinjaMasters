#include "GpibDevice.hpp"

GpibDevice::GpibDevice(int board_index, int primary_address)
{
    #if DEBUG
        fprintf(stderr, "Start of GpibDevice Constructor\n");
    #endif // DEBUG

    char busname[128];

    strcpy(busname, "GPIB");
    sprintf(busname, "%s%d::%d::INSTR", busname, board_index, primary_address); //open session to device_
    printf(busname);
    printf("\n");
    if(board_index > 255 || board_index < 0) visa::err("Bad GPIB board index number given");
    if(primary_address > 30 || primary_address < 0)  visa::err("Bad GPIB primary address number given");

    visa::OpenVISA(&resourceManager_); //create VISA session (PC -> VISA)
    device_ = visa::OpenDevice(resourceManager_, busname); //connect session identifier to Psu (VISA -> device_)
    visa::setTMO(device_, 9000);

    sprintf(device_name_ , "GPIB Device %s", busname);

    #if DEBUG
        fprintf(stderr, "End of GpibDevice Constructor\n");
    #endif // DEBUG
}

GpibDevice::~GpibDevice(void)
{
    #if DEBUG
        fprintf(stderr, "Start of GpibDevice Destructor\n");
    #endif // DEBUG

    #if DEBUG
        fprintf(stderr, "End of GpibDevice Destructor\n");
    #endif // DEBUG
}



int GpibDevice::Read(char *data)
{
    #if DEBUG
        fprintf(stderr, "GpibDevice::Read\n");
    #endif // DEBUG

    int count = 0;
    count = visa::rbstr(device_, data, 255);

    return count;
}

int GpibDevice::Write(char *data)
{
    #if DEBUG
        fprintf(stderr, "GpibDevice::Write\n");
    #endif // DEBUG
    int count = 0;
    count = visa::wbstr(device_, data);
    return count;
}


