#include "bchelper.hpp"
#include "string.h"
#include "stdio.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

bcargs bcparse(int argc, char ** argv, string device_string)
{

    bcargs args;

    if((argc != 12) && (argc != 13))
    {
        cout << "Invalid number of parameters "  <<  argc-1 << endl;
        cout << "Usage: bc <device> <GPIB#> <gpib_addr> <ncycles> <Vmax> <Vmin> <Imax> <Iend> <Qend> <timeout> <relaxtime> [filestring]" << endl;
        cout << "Usage: bc <device> <serial_port> <baud> <ncycles> <Vmax> <Vmin> <Imax> <Iend> <Qend> <timeout> <relaxtime> [filestring]" << endl;
        cout << "Parameters in square brackets [] are optional" << endl;
        cout << endl;
        cout << "bc HP66332 2 3 ...               < connects to a HP66332 at GPIB2::3::INSTR" << endl;
        cout << "bc HP66332 GPIB2 3 ...           < connects to a HP66332 at GPIB2::3::INSTR" << endl;
        cout << "bc Keithley COM20 9600 ...       < connects to a Keithley at COM20 with a baud rate of 9600" << endl;
        cout << "bc Hameg /dev/ttyS20 115200 ...  < connects to a Hameg at /dev/ttyS20 with a baud rate of 115200" << endl;
        cout << endl;
        cout << "This program requires a " << device_string << " connected to the host machine accessible at the specified" << endl;
        cout << "GPIB address, and the visa driver interface to be installed on the host" << endl;
        cout << "The expected device address is in the format: GPIB<GPIB#>::<gpib_addr>::INSTR" << endl;
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
        cout << "<device> is the hardware to be used to cycle the battery with. Options are: " << endl;
        cout << "Keithley Hameg HP66332 E5270" << endl;
        cout << "<GPIB#> is the GPIB address value of the primary switch that the " << device_string << " is attached to" << endl;
        cout << "<gpib_addr> is the GPIB subaddress value of the " << device_string << " attached to the GPIB switch at <GPIB#>" << endl;
        cout << "<serial_port> is the alternative serial port to connect to ie. COM20 or /dev/ttyS20" << endl;
        cout << "<baud> is the baud rate when using a non-GPIB serial port" << endl;
        cout << "<ncycles> is the number of times the attached battery will be fully discharged and then fully charged" << endl;
        cout << "<vmax> is the maximum voltage, in Volts,  up to which the battery will be charged" << endl;
        cout << "<vmin> is the minimum voltage, in Volts,  down to which the battery will be discharged" << endl;
        cout << "<imax> is the maximum current, in Amps, that " << device_string << " will source or sink to charge or discharge the battery" << endl;
        cout << "<iend> is the minimum current, in Amps, flowing when the program will decide the battery is fully charged or discharged" << endl;
        cout << "<qend> is the final fraction of charge left in the battery as a percent of fully charged, expected values are [100 - 0]" << endl;
        cout << "<timeout> is the maximum amount of time, in Seconds, the current will be allowed to decay after falling below <imax>" << endl;
        cout << "    before the program decides the battery is fully charged or discharged" << endl;
        cout << "<relaxtime> is the time, in Seconds, the voltage will be allowed to relax after all cycling is complete and the" << endl;
        cout << "    charge level has reached <qend>, zero current will flow but the " << device_string << " will continue measurements for this time" << endl;
        cout << "[filestring] sets the name of the target .tvi and .log files, NOTE existing files with the same name will be OVERWRITTEN" << endl;
        // args.gpib_major = -1;
        // return args;
        exit(1);
    }
    else
    {
        // args.gpib_major = atoi(argv[2]);
        // args.gpib_minor = atoi(argv[3]);
        args.num_cycles = atoi(argv[4]);
        args.max_voltage = atof(argv[5]);
        args.min_voltage = atof(argv[6]);
        args.max_current = atof(argv[7]);
        args.end_current = atof(argv[8]);
        args.end_charge = atof(argv[9]);
        args.end_charge = args.end_charge/100;
        args.timeout = atof(argv[10]);
        args.relax_time = atof(argv[11]);
    }
    if (!(atof(argv[argc-1])))
    {
        sprintf(args.filestring, "%s", argv[argc-1]);
    }

    return args;
}
