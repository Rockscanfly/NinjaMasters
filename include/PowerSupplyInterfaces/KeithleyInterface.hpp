#ifndef KEITHLEY_INTERFACE_H
#define KEITHLEY_INTERFACE_H
#define KEITHLEY_INTERFACE_CHANNEL 1
#define KEITHLEY_INTERFACE_VERSION 3.0
#include "PsuInterface.hpp"

class KeithleyInterface : public PsuInterface {
    public:
        KeithleyInterface(char serial_mode[256], char serial_value[256],
            double max_voltage, double min_voltage, double max_current, char filestring[255]);
        ~KeithleyInterface();

        /*
        * Device Specific Functions
        * Required by high level functions
        * Should be implemented by new devices
        */
        int SetOutput(const double voltage, const double current);
        int GetOutput(double *voltage, double *current);
        int SMUVoltage(double voltage, double current);
        int SMUCurrent(double voltage_max, double voltage_min, double current_t);
        double SetVoltageRange(double voltage);
        double SetCurrentRange(double current);
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
        void Beep(int frequency, double duration);
        void ReadRange(void);

};

#endif // KEITHLEY_INTERFACE_H
