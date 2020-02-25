#include "bzhelper.hpp"
#include "string.h"
#include "stdio.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

bzargs bzparse(int argc, char ** argv, std::string device_string)
{

    std::cout << "Things One" << std::endl;
    std::cout.flush();
    std::cout << "Things Two" << std::endl;
    std::cout.flush();
    bzargs args;

    if((argc < 11))
    {
        cout << "Invalid number of parameters" <<  argc-1 << endl;
        cout << "Usage: bz <device> <VISA#> <gpib_addr> <Vmax> <Vmin> <Imax> <QMax> <Ncylces> [Single?] <Frequency List> [filestring]" << endl;
        cout << endl;
        cout << "This program requires a " << device_string << " connected to the host machine accessible at the specified" << endl;
        cout << "GPIB address, and the visa driver interface to be installed on the host" << endl;
        cout << "The expected device address is in the format: GPIB<VISA#>::<gpib_addr>::INSTR" << endl;
        cout << endl;
        cout << "The program is designed to cycle a battery attached to the " << device_string << " between the specified maximum and" << endl;
        cout << "minimum voltages at the specified maximum current. The program expects the voltage and current ranges to" << endl;
        cout << "accurately represent the safe operating values of the battery and provides no guarantee it will not" << endl;
        cout << "damage any attached device" << endl;
        cout << endl;
        cout << "Data and log files will be created in the same directory as where the program was called from" << endl;
        cout << "The optional argument [filestring] will overwrite the default file names DataFile.tvi and LogFile.log" << endl;
        cout << "and results will be stored in [filestring].tvi and [filestring].log" << endl;
        cout << "The .tvi file stores measurements in three tab separated columns time(s) volts(V) current(A)" << endl;
        cout << "The .log file stores timestamped plain english descriptions of milestones reached by the program" << endl;
        cout << endl;
        cout << "<device> is the hardware to be used to measure imepedance with. Options are: " << endl;
        cout << "Keithley Hameg HP66332 E5270" << endl;
        cout << "<VISA#> is the GPIB address value of the primary switch that the Keithley 2460 is attached to" << endl;
        cout << "<gpib_addr> is the GPIB subaddress value of Keithley 2460 attached to the GPIB switch at <VISA#>" << endl;
        cout << "<vmax> is the maximum voltage up to which the battery will be charged, the program will exit if this limit is reached during cycling" << endl;
        cout << "<vmin> is the minimum voltage down to which the battery will be discharged, the program will exit if this limit is reached during cycling" << endl;
        cout << "<imax> is the maximum current, in Amps, that will be sourced or sunk to force a waveform into the battery" << endl;
        cout << "<qmax> is the maximum charge, in Amphere Hours, that will sourced or sunk to force a waveform into the battery" << endl;
        cout << "<ncycles> is the number of times the attached battery will be cycled at the lowest given frequency" << endl;
        cout << "[Single?] is an option string to specify that the given list of frequencies should be iterated over individually instead of simultaneously" << endl;
        cout << " to use this option, type the word \"Single\" BEFORE listing frequencies" << endl;
        cout << "<frequency list> is a list of frequencies used to create the current waveform that will be produced, limit 16 frequencies, maximum 1Hz" << endl;
        cout << "[filestring] sets the base name of the target .tvi and .log files, NOTE existing files with the same name will be OVERWRITTEN" << endl;
        cout.flush();
		args.gpib_major = -1;
        return args;
    }else{

        args.gpib_major = atoi(argv[2]);
        args.gpib_minor = atoi(argv[3]);
        args.vmax = atof(argv[4]);
        args.vmin = atof(argv[5]);
        args.imax = atof(argv[6]);
        args.qmax = atof(argv[7]);
        args.ncycles = atoi(argv[8]);

        int i = 9;
        if ((argv[9][0] == 'S') | (argv[9][0] == 's'))
        {
            args.single_sweep = true;
            i++;
        } else {
            args.single_sweep = false;
        }

        while(i < argc && args.num_frequencies < 16)
        {
            args.frequency[args.num_frequencies] = atof(argv[i]);

            if(args.frequency[args.num_frequencies] > 1.0)
            {
                cout << "Frequency " << args.frequency[args.num_frequencies] << " entered, frequencies above 1Hz are not supported" << endl;
                args.gpib_major = -1;
                return args;
            }

            if (args.frequency[args.num_frequencies])
            {
                args.num_frequencies++;
            }

            i++;
        }

        if(args.num_frequencies == 1)
        {
            args.single_sweep = true;
        }

        if (!(atof(argv[argc-1])))
        {
            sprintf(args.filestring, "%s", argv[argc-1]);
        }
    }
	return args;
}

