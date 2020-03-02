#ifndef KEITHLEY_INTERFACE_H
#define KEITHLEY_INTERFACE_H
#define KEITHLEY_INTERFACE_CHANNEL 1
#define KEITHLEY_INTERFACE_VERSION 1.0
#include "PSUInterface.hpp"


class KeithleyInterface : public PSUInterface {
    public:
         /** Constructor */
        KeithleyInterface(int id,
                        int addr,
                        int channel,
                        double Vmax,
                        double Vmin,
                        double Imax,
                        const char filestring[255]);
        /** Default destructor */
        ~KeithleyInterface();

        /*
        * Device Specific Functions
        * Required by high level functions
        * Should be implemented by new devices
        */
        int SetOutput(double V, double I);
        int GetOutput(double * V, double * I);
        int SMUVoltage(double V, double I);
        int SMUCurrent(double t_voltage_max, double t_voltage_min, double t_current);
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

        void Beep(int f, double t);
    protected:

    private:
        int err = 0;
        char m_inst[256];
        char m_val[256];
        int m_current_cycle = 0;

        void ReadRange(void);

};

#endif // KEITHLEY_INTERFACE_H
