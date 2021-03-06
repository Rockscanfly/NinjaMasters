#ifndef E5270INTERFACE_H
#define E5270INTERFACE_H
#define E5270_INTERFACE_VERSION 3.0
#define E5270_INTERFACE_CHANNEL 2
#define E5270_INTERFACE_CHANNEL_GND 4
#include "PsuInterface.hpp"

class E5270Interface : public PsuInterface
{
    public:
        E5270Interface(char serial_mode[256], char serial_value[256],
            double max_voltage, double min_voltage, double max_current, char filestring[255]);
        ~E5270Interface();

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

    protected:

    private:
        int DataDecode(char data[256], double *value);

        int channel = E5270_INTERFACE_CHANNEL;

};

#endif // E5270INTERFACE_H
