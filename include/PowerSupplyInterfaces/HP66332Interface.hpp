#ifndef HP66332_INTERFACE_H
#define HP66332_INTERFACE_H
#define HP66332_INTERFACE_CHANNEL 1
#define HP66332_INTERFACE_VERSION 3.0
#include "PsuInterface.hpp"


class HP66332Interface : public PsuInterface {
    public:
        HP66332Interface(
                        const char filestring[255]);
        ~HP66332Interface();

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

};

#endif // HP66332_INTERFACE_H
