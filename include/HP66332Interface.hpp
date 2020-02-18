#ifndef HP66332_INTERFACE_H
#define HP66332_INTERFACE_H
#define HP66332_INTERFACE_CHANNEL 1
#define HP66332_INTERFACE_VERSION 1.0
#include "PSUInterface.hpp"


class HP66332Interface : public PSUInterface {
    public:
         /** Constructor */
        HP66332Interface(int id,
                        int addr,
                        int channel,
                        double Vmax,
                        double Vmin,
                        double Imax,
                        const char filestring[255]);
        /** Default destructor */
        ~HP66332Interface();

        /* 
        * Primary High Level Functions
        * Should be inherited from PSUInterface
        */
        // double CycleBattery(int t_number_cycles, double t_voltage_max, double t_voltage_min,
        //                       double t_current_max, double t_current_end, double t_charge_end, double t_timeout, double t_relax_time);
        // double Waveform(double t_voltage_max, double t_voltage_min, double t_current_max, double t_charge_max, int t_number_cycles, double t_frequency[16]);

        /* 
        * Secondary High Level Functions
        * Should be inherited from PSUInterface
        */
        // double GetToVoltage(double t_voltage_target, double t_current_max, double t_current_end, double t_timeout);
        // double MoveCharge(double t_voltage_limit, double t_current_max, double t_charge_to_move);

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
        * Visa Communication high level
        */
        // int Write(char *inst);
        // int Query(char *inst, char *val);

        /*
        * Visa Communication device specific
        */
        int ClearErrors(void);
        int CheckErrors(void);
        // bool NComplete(void);
        
        // optional test function
        // double Test(double V, double I);

    protected:

    private:

};

#endif // HP66332_INTERFACE_H
