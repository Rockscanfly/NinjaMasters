#ifndef PSUINTERFACE_H
#define PSUINTERFACE_H

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <cstdio>
#include <stdint.h>
#define _USE_MATH_DEFINES
#include <cmath>

#include "SerialDevice.hpp"

class PsuInterface
{
    public:
        /** Default constructor */
        PsuInterface(char serial_mode[256], char serial_value[256],
            double max_voltage, double min_voltage, double max_current, char filestring[255]);


        PsuInterface(PsuInterface&) = delete;
        int operator=(PsuInterface&) = delete;
        /** Default destructor */
        virtual ~PsuInterface();

        /*
        * Primary functions
        * Should be inherited from PsuInterface
        */
        double CycleBattery(const int number_cycles, const double voltage_max, const double voltage_min,
                              const double current_max, const double current_end, const double charge_end,
                              const double timeout, const double relax_time);

        double Waveform(const double voltage_max, const double voltage_min, const double current_max, const double charge_max,
                        const int number_cycles, const double frequency[16]);

        double GetToVoltage(const double voltage_target, const double current_max, const double current_end, const double timeout);
        double MoveCharge(const double voltage_max, const double voltage_min, const double current_max, const double charge_to_move);
        int Write(char *inst);
        int Query(char *inst, char *val);

        /*
        * Device Specific Functions
        * Must be implemented by new devices
        */
        virtual int SetOutput(const double voltage, const double current) = 0;
        virtual int GetOutput(double *voltage, double *current) = 0;
        virtual int SMUVoltage(double voltage, double current) = 0;
        virtual int SMUCurrent(double voltage_max, double voltage_min, double current_t) = 0;
        virtual double SetVoltageRange(double voltage) = 0;
        virtual double SetCurrentRange(double current) = 0;
        virtual int IsCurrentLimited(void) = 0;
        virtual int OutputOn(void) = 0;
        virtual int OutputOff(void) = 0;
        virtual int ClearErrors(void) = 0;
        virtual int CheckErrors(void) = 0;


        /*
        * File Management
        * Should be inherited from PsuInterface
        */
        int WriteData(const double voltage, const double current);
        int MarkData(const char* str);
        int WriteLog(const char* str);
        int ChangeDataFile(const char* str);
        int ChangeLogFile(const char* str);

        // test function
        // double Test(double V, double I);

    protected:

        SerialDevice *device_ = nullptr;
        void mwait(int msecs); // wait some milliseconds in real-time work

        time_t t = 0;
        time_t t0 = 0;
        clock_t clock_initial_ = 0;
        clock_t clock_now_ = 0;

        char inst_[256] = {'\0'};
        char val_[256] = {'\0'};
        int current_cycle_ = 0;

        double max_voltage_;
        double min_voltage_;
        double max_current_;

        char filestring_[255] = {'\0'};

        FILE * p_data = nullptr;
        FILE * p_log = nullptr;
};

#endif // PSUINTERFACE_H
