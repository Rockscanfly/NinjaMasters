#ifndef HP66332_INTERFACE_H
#define HP66332_INTERFACE_H
#define HP66332_INTERFACE_CHANNEL 1
#define HP66332_INTERFACE_VERSION 3.0
#include "PsuInterface.hpp"

class HP66332Interface : public PsuInterface {
    public:
        HP66332Interface(char serial_mode[256], char serial_value[256],
            double max_voltage, double min_voltage, double max_current, char filestring[255]);
        ~HP66332Interface();

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

};

#endif // HP66332_INTERFACE_H
