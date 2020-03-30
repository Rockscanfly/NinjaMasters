#ifndef EMPTYInterface_H
#define EMPTYInterface_H

#include "PsuInterface.hpp"


class EMPTYInterface : public PsuInterface
{
    public:
        /** Constructor */
        EMPTYInterface(
                       double Vmax,
                       double Vmin,
                       double Imax,
                       const char filestring[255]);
//        virtual EMPTYInterface();
        /** Default destructor */
        ~EMPTYInterface();

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

#endif // EMPTYInterface_H
