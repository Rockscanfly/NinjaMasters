#ifndef HamegInterface_H
#define HamegInterface_H
#define HAMEG_INTERFACE_CHANNEL 1
#include "PSUInterface.hpp"


class HamegInterface : public PSUInterface
{
    public:
        /** Constructor */
        HamegInterface(int id,
                       int addr,
                       int channel,
                       double Vmax,
                       double Vmin,
                       double Imax,
                       const char filestring[255]);
//        virtual HamegInterface();
        /** Default destructor */
        ~HamegInterface();

        int SetOutput(double V, double I);
        int GetOutput(double * V, double * I);
        int SMUVoltage(double V, double I);
        int SMUCurrent(double t_voltage_max, double t_voltage_min, double t_current);
        double SetVoltageRange(double V);
        double SetCurrentRange(double I);
        int IsCurrentLimited(void);
        int OutputOn(void);
        int OutputOff(void);

        int ClearErrors(void);
        int CheckErrors(void);
    protected:

    private:
    
};

#endif // HamegInterface_H
