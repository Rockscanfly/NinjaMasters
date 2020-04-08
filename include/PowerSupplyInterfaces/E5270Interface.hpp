#ifndef E5270INTERFACE_H
#define E5270INTERFACE_H
#define E5270_INTERFACE_VERSION 3.0
#define E5270_INTERFACE_CHANNEL 2
#define E5270_INTERFACE_CHANNEL_GND 4

#include "PsuInterface.hpp"


class E5270Interface : public PsuInterface
{
    public:
        E5270Interface(
                        const char filestring[255]);
        ~E5270Interface();

        /*
        * Device Specific Functions
        * Required by high level functions
        * Should be implemented by new devices
        */
        int SetOutput(double V, double I);
        int GetOutput(double * V, double * I);
        int SMUVoltage(double V, double I);
        int SMUCurrent(double voltage_max, double voltage_min, double current_t);
        double SetVoltageRange(double V);
        double SetCurrentRange(double I);
        int IsCurrentLimited(void);
        int OutputOn(void);
        int OutputOff(void);

        /*
        * Visa Communication
        * device specific
        */
        int ClearErrors(void);
        int CheckErrors(void);

    protected:

    private:
        int DataDecode(const char data[256], double *return);

        int channel = E5270_INTERFACE_CHANNEL;



};

#endif // E5270INTERFACE_H
