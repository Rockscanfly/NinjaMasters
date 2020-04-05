#include "bzhelper.hpp"
#include "string.h"
#include "stdio.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

bzargs bzparse(int argc, char ** argv, std::string device_string)
{

    bzargs args;

    if((argc < 10))
    {
        cout << "Invalid number of parameters " <<  argc-1 << endl;
        cout << "Usage: bz <device> <GPIB#> <gpib_addr> <Vmax> <Vmin> <Imax> <QMax> <Ncylces> [S|s|Single] <Frequency List> [filestring]" << endl;
        cout << "Usage: bz <device> <serial_port> <baud> <Vmax> <Vmin> <Imax> <QMax> <Ncylces> [S|s|Single] <Frequency List> [filestring]" << endl;
        cout << "Parameters in square brackets [] are optional"
        cout << endl;
        cout << "bz HP66332 2 3 ...               < connects to a HP66332 at GPIB2::3::INSTR" << endl;
        cout << "bz HP66332 GPIB2 3 ...           < connects to a HP66332 at GPIB2::3::INSTR" << endl;
        cout << "bz Keithley COM20 9600 ...       < connects to a Keithley at COM20 with a baud rate of 9600" << endl;
        cout << "bz Hameg /dev/ttyS20 115200 ...  < connects to a Hameg at /dev/ttyS20 with a baud rate of 115200" << endl;
        cout << endl;
        cout << "This program requires a " << device_string << " connected to the host machine accessible at the specified" << endl;
        cout << "GPIB address or serial port, and the visa driver interface to be installed on the host" << endl;
        cout << endl;
        cout << "The program is designed to measure the imepedance of a battery attached to the " << device_string << endl;
        cout << "at the specified frequencies. The program expects the voltage and current ranges to" << endl;
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
        cout << "<GPIB#> is the GPIB address value of the primary switch that the " << device_string << " is attached to" << endl;
        cout << "<gpib_addr> is the GPIB subaddress value of the " << device_string << " attached to the GPIB switch at <GPIB#>" << endl;
        cout << "<serial_port> is the alternative serial port to connect to ie. COM20 or /dev/ttyS20" << endl;
        cout << "<baud> is the baud rate when using a non-GPIB serial port" << endl;
        cout << "<vmax> is the maximum voltage, in Volts,  up to which the battery will be charged, the program will exit if this limit is reached during cycling" << endl;
        cout << "<vmin> is the minimum voltage, in Volts,  down to which the battery will be discharged, the program will exit if this limit is reached during cycling" << endl;
        cout << "<imax> is the maximum current, in Amps, that will be sourced or sunk to force a waveform into the battery" << endl;
        cout << "<qmax> is the maximum charge, in Amphere Hours, that will sourced or sunk to force a waveform into the battery" << endl;
        cout << "<ncycles> is the number of sinusoidal current cycles, at the lowest given frequency, the battery will experience" << endl;
        cout << "[Single?] is an option string to specify that the given list of frequencies should be iterated over individually instead of simultaneously" << endl;
        cout << " to use this option, type the word \"Single\" BEFORE listing frequencies" << endl;
        cout << "<frequency list> is a list of frequencies, in Hz,  used to create the current waveform that will be produced, limit 16 frequencies, maximum 1Hz" << endl;
        cout << "[filestring] sets the base name of the target .tvi and .log files, NOTE existing files with the same name will be OVERWRITTEN" << endl;
        cout << "data files of the name format [filestring]_[first frequency in uHz]uHz[# of frequencies]tone.tvi will be made for multitone sweeps" << endl;
        cout << "data files of the name format [filestring]_[frequency in uHz]uHz.tvi will be made for single tone sweeps" << endl;
        cout.flush( );
		// args.gpib_major = -1;
        return args;
    } else {

        // args.gpib_major = atoi(argv[2]);
        // args.gpib_minor = atoi(argv[3]);
        args.vmax = atof(argv[4]);
        args.vmin = atof(argv[5]);
        args.imax = atof(argv[6]);
        args.qmax = atof(argv[7]);
        args.ncycles = atoi(argv[8]);

        int i = 9;
        if ( ( argv[9][0] == 'S' ) | ( argv[9][0] == 's' ) )
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
    args_good = true;
	return args;
}

