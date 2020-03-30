#ifndef PSUINTERFACE_H
#define PSUINTERFACE_H

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#define _USE_MATH_DEFINES
#include <cmath>



class PsuInterface
{
    public:
        /** Default constructor */
        PsuInterface(double Vmax, double Vmin, double  Imax, const char filestring[255]);
        PsuInterface(const PsuInterface&) = delete;
        int operator=(const PsuInterface&) = delete;
        /** Default destructor */
        virtual ~PsuInterface();

        /*
        * Primary High Level Functions
        * Should be inherited from PsuInterface
        */
        double CycleBattery(int t_number_cycles, double t_voltage_max, double t_voltage_min,
                              double t_current_max, double t_current_end, double t_charge_end, double t_timeout, double t_relax_time);
        double Waveform(double t_voltage_max, double t_voltage_min, double t_current_max, double t_charge_max, int t_number_cycles, double t_frequency[16]);

        /*
        * Secondary High Level Functions
        * Should be inherited from PsuInterface
        */
        double GetToVoltage(double t_voltage_target, double t_current_max, double t_current_end, double t_timeout);
        double MoveCharge(double t_voltage_max, double t_voltage_min, double t_current_max, double t_charge_to_move);

        /*
        * Visa Communication high level
        */
        int Write(char *inst);
        int Query(char *inst, char *val);

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
        virtual int ClearErrors(void) = 0;
        virtual int CheckErrors(void) = 0;


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

        void mwait(int msecs); // wait some milliseconds in real-time work


        /** Access channel
         * \return The current value of channel
         */

        double vMax;
        double vMin;
        double iMax;

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
