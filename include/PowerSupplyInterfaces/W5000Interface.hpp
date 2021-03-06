#ifndef W5000Interface_H
#define W5000Interface_H
#define W5000_INTERFACE_CHANNEL 1
#define W5000_INTERFACE_VERSION 3.0
#include "PsuInterface.hpp"

class W5000Interface : public PsuInterface {
    public:
        W5000Interface(char serial_mode[256], char serial_value[256],
            double max_voltage, double min_voltage, double max_current, char filestring[255]);
        ~W5000Interface();

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

    private:
        int channel = 1;

};

#endif // W5000Interface_H
