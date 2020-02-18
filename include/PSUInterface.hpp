#ifndef PSUINTERFACE_H
#define PSUINTERFACE_H

#include <string.h>
#include <time.h>
#include "visawrapper.h"
#define _USE_MATH_DEFINES
#include <cmath>


class PSUInterface
{
    public:
        /** Default constructor */
        PSUInterface(int id, int addr, int channel, double Vmax, double Vmin, double  Imax, const char filestring[255]);
        PSUInterface(const PSUInterface&) = delete;
        int operator=(const PSUInterface&) = delete;
        /** Default destructor */
        virtual ~PSUInterface();

        /* 
        * Primary High Level Functions
        * Should be inherited from PSUInterface
        */
        double CycleBattery(int t_number_cycles, double t_voltage_max, double t_voltage_min,
                              double t_current_max, double t_current_end, double t_charge_end, double t_timeout, double t_relax_time);
        double Waveform(double t_voltage_max, double t_voltage_min, double t_current_max, double t_charge_max, int t_number_cycles, double t_frequency[16]);

        /* 
        * Secondary High Level Functions
        * Should be inherited from PSUInterface
        */
        double GetToVoltage(double t_voltage_target, double t_current_max, double t_current_end, double t_timeout);
        double MoveCharge(double t_voltage_max, double t_voltage_min, double t_current_max, double t_charge_to_move);

        /*
        * Device Specific Functions
        * Required by high level functions
        * Should be implemented by new devices
        */
        virtual int SetOutput(double V, double I) = 0;
        virtual int GetOutput(double * V, double * I) = 0;
        virtual int SMUVoltage(double V, double I) = 0;
        virtual int SMUCurrent(double t_voltage_max, double t_voltage_min, double t_current) = 0;
        virtual double SetVoltageRange(double V) = 0;
        virtual double SetCurrentRange(double I) = 0;
        virtual int IsCurrentLimited(void) = 0;
        virtual int OutputOn(void) = 0;
        virtual int OutputOff(void) = 0;

        /*
        * Visa Communication high level
        */
        int Write(char *inst);
        int Query(char *inst, char *val);

        /*
        * Visa Communication 
        * device specific
        */
        virtual int ClearErrors(void) = 0;
        virtual int CheckErrors(void) = 0;
        // virtual bool NComplete(void) = 0;
        
        /*
        * Data File Management
        */
        int WriteData(const double v, const double i);
        int MarkData(const char* str);
        int WriteLog(const char* str);
        int ChangeDataFile(const char* str);
        int ChangeLogFile(const char* str);


        // optional test function
        double Test(double V, double I);

    protected:



        /** Access channel
         * \return The current value of channel
         */
        int GetChannel() { return channel; }

        int visaID;
        int subAddr;
        int channel;
        double vMax;
        double vMin;
        double iMax;

        ViSession resourceManager = 0; //!< Member variable "resourceManager"
        ViSession device = 0; //!< Member variable "device"
        char busname[32]; //!< Member variable "busname"
        char cmd[255]; //!< Member variable "cmd"

        time_t t = 0;
        time_t t0 = 0;
        clock_t m_clock_initial = 0;
        clock_t m_clock_now = 0;

        char m_inst[256];
        char m_val[256];
        int m_current_cycle = 0;

        char filestring[255];

    private:
        FILE * p_data = nullptr;
        FILE * p_log = nullptr;

};

#endif // PSUINTERFACE_H
