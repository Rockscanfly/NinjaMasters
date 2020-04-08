#ifndef KEITHLEY_INTERFACE_H
#define KEITHLEY_INTERFACE_H
#define KEITHLEY_INTERFACE_CHANNEL 1
#define KEITHLEY_INTERFACE_VERSION 3.0
#include "PsuInterface.hpp"


class KeithleyInterface : public PsuInterface {
    public:
        KeithleyInterface(
                        const char filestring[255]);
        ~KeithleyInterface();

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

        /*
        * Unique to this hardware device
        */
        void Beep(int f, double t);
        void ReadRange(void);

};

#endif // KEITHLEY_INTERFACE_H
