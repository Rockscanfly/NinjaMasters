#ifndef E5270INTERFACE_H
#define E5270INTERFACE_H
#define E5270_INTERFACE_CHANNEL 2
#define E5270_INTERFACE_CHANNEL_GND 4

#include "PSUInterface.hpp"


class E5270Interface : public PSUInterface
{
    public:
        /** Constructor */
        E5270Interface(int id,
                       int addr,
                       int channel,
                       double Vmax,
                       double Vmin,
                       double Imax,
                       const char filestring[255]);
//        virtual E5270Interface();
        /** Default destructor */
        ~E5270Interface();

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
        // virtual bool NComplete(void);

    protected:

    private:
        
        int err = 0;
        char m_inst[256];
        char m_val[256];
        int m_current_cycle = 0;

        int DataDecode(const char t_data[256], double *t_return);


};

#endif // E5270INTERFACE_H
