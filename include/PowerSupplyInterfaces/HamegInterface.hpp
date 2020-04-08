#ifndef HamegInterface_H
#define HamegInterface_H
#define HAMEG_INTERFACE_CHANNEL 1
#define HAMEG_INTERFACE_VERSION 3.0
#include "PsuInterface.hpp"

class HamegInterface : public PsuInterface {
    public:
        HamegInterface(
                        const char filestring[255]);
        ~HamegInterface();

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

    private:
        int channel = 1;

};

#endif // HamegInterface_H
