#include "bchelper.hpp"
#include "string.h"
#include "stdio.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

bcargs bcparse(int argc, char ** argv, string device_string)
{

    bcargs args;

    if((argc != 11) && (argc != 12))
    {
        cout << "Invalid number of parameters "  <<  argc-1 << endl;
        cout << "Usage: bc.exe <device> <VISA#> <gpib_addr> <ncycles> <Vmax> <Vmin> <Imax> <Iend> <Qend> <timeout> <relaxtime> [filestring]" << endl;
        cout << endl;
        cout << "This program requires a " << device_string << " connected to the host machine accessible at the specified" << endl;
        cout << "GPIB address, and the visa driver interface to be installed on the host" << endl;
        cout << "The expected device address is in the format: GPIB<VISA#>::<gpib_addr>::INSTR" << endl;
        cout << endl;
        cout << "The program is designed to cycle a battery attached to the " << device_string << " between the specified maximum and" << endl;
        cout << "minimum voltages at the specified maximum current. The program expects the voltage and current ranges to" << endl;
        cout << "accurately represent the safe operating values of the battery and provides no guarantee it will not" << endl;
        cout << "damage any attached device" << endl;
        cout  << endl;
        cout << "Data and log files will be created in the same directory as where the program was called from" << endl;
        cout << "The optional argument [filestring] will overwrite the default file names DataFile.tvi and LogFile.log" << endl;
        cout << "and results will be stored in [filestring].tvi and [filestring].log" << endl;
        cout << "The .tvi file stores measurements in three tab separated columns time(s) volts(V) current(A)" << endl;
        cout << "The .log file stores timestamped plain english descriptions of milestones reached by the program" << endl;
        cout << endl;
        cout << "<device> is the hardware to be used to measure imepedance with. Options are: " << endl;
        cout << "Keithley Hameg HP66332 E5270" << endl;
        cout << "<VISA#> is the GPIB address value of the primary switch that the " << device_string << " is attached to" << endl;
        cout << "<gpib_addr> is the GPIB subaddress value of " << device_string << " attached to the GPIB switch at <VISA#>" << endl;
        cout << "<ncycles> is the number of times the attached battery will be fully discharged and then fully charged" << endl;
        cout << "<vmax> is the maximum voltage up to which the battery will be charged" << endl;
        cout << "<vmin> is the minimum voltage down to which the battery will be discharged" << endl;
        cout << "<imax> is the maximum current, in Amps, that " << device_string << " will source or sink to charge or discharge the battery" << endl;
        cout << "<iend> is the minimum current, in Amps, flowing when the program will decide the battery is fullly charged or discharged" << endl;
        cout << "<qend> is the final fraction of charge left in the battery as a percent of fully charged, NOTE not the same as voltage" << endl;
        cout << "<timeout> is the maximum amount of time the current will be allowed to decay after falling below <imax>" << endl;
        cout << "    before the program decides the battery is fully charged or discharged" << endl;
        cout << "<relaxtime> is the time the voltage will be allowed to relax after all cycling is complete and the" << endl;
        cout << "    charge level has reached <quend>, zero current will flow but the " << device_string << " will continue measurements for this time" << endl;
        cout << "[filestring] sets the name of the target .tvi and .log files, NOTE existing files with the same name will be OVERWRITTEN" << endl;
        args.gpib_major = -1;
        return args;
    }
    else
    {
        for(int i = 1; i < argc; i++)
        {
            switch(i) {
                case 1:
                    args.gpib_major = atoi(argv[i]);
                    break;

                case 2:
                    args.gpib_minor = atoi(argv[i]);
                    break;

                case 3:
                    args.ncycles = atoi(argv[i]);
                    break;

                case 4:
                    args.vmax = atof(argv[i]);
                    break;

                case 5:
                    args.vmin = atof(argv[i]);
                    break;

                case 6:
                    args.imax = atof(argv[i]);
                    break;

                case 7:
                    args.iend = atof(argv[i]);
                    break;

                case 8:
                    args.qend = atof(argv[i]);
                    break;

                case 9:
                    args.timeout = atof(argv[i]);
                    break;

                case 10:
                    args.trelax = atof(argv[i]);
                    break;

                case 11:
                    sprintf(args.filestring, "%s", argv[i]);
                    break;

                default:
                cout << "Error unknown argument " << argv[i]  << endl;
                    args.gpib_major = -1;

            }
        }
    }

    return args;
}