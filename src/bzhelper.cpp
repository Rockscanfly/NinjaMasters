#include "bzhelper.hpp"
#include "stdio.h"
#include <stdlib.h>

bzargs bzparse(int argc, char ** argv, const char device_string[256])
{

    bzargs args;

    if(argc < 10)
    {
        printf("Invalid number of parameters %i\n", argc-1);
        printf("Usage: %s <VISA#> <gpib_addr> <Vmax> <Vmin> <Imax> <QMax> <Ncylces> [Single?] <Frequency List> [filestring]\n", argv[0]);
        printf("\n");
        printf("This program requires a %s connected to the host machine accessible at the specified\n", device_string);
        printf("GPIB address, and the visa driver interface to be installed on the host\n");
        printf("The expected device address is in the format: GPIB<VISA#>::<gpib_addr>::INSTR\n");
        printf("\n");
        printf("The program is designed to cycle a battery attached to the %s between the specified maximum and\n", device_string);
        printf("minimum voltages at the specified maximum current. The program expects the voltage and current ranges to\n");
        printf("accurately represent the safe operating values of the battery and provides no guarantee it will not\n");
        printf("damage any attached device\n");
        printf("\n");
        printf("Data and log files will be created in the same directory as where the program was called from\n");
        printf("The optional argument [filestring] will overwrite the default file names DataFile.tvi and LogFile.log\n");
        printf("and results will be stored in [filestring].tvi and [filestring].log\n");
        printf("The .tvi file stores measurements in three tab separated columns time(s) volts(V) current(A)\n");
        printf("The .log file stores timestamped plain english descriptions of milestones reached by the program\n");
        printf("\n");
        printf("<VISA#> is the GPIB address value of the primary switch that the Keithley 2460 is attached to\n");
        printf("<gpib_addr> is the GPIB subaddress value of Keithley 2460 attached to the GPIB switch at <VISA#>\n");
        printf("<vmax> is the maximum voltage up to which the battery will be charged, the program will exit if this limit is reached during cycling\n");
        printf("<vmin> is the minimum voltage down to which the battery will be discharged, the program will exit if this limit is reached during cycling\n");
        printf("<imax> is the maximum current, in Amps, that will be sourced or sunk to force a waveform into the battery\n");
        printf("<qmax> is the maximum charge, in Amphere Hours, that will sourced or sunk to force a waveform into the battery\n");
        printf("<ncycles> is the number of times the attached battery will be cycled at the lowest given frequency\n");
        printf("[Single?] is an option string to specify that the given list of frequencies should be iterated over individually instead of simultaneously\n");
        printf(" to use this option, type the word \"Single\" BEFORE listing frequencies\n");
        printf("<frequency list > is a list of frequencies used to create the current waveform that will be produced, limit 16 frequencies, maximum 1Hz\n");
        printf("[filestring] sets the base name of the target .tvi and .log files, NOTE existing files with the same name will be OVERWRITTEN\n");

		args.gpib_major = -1;
        return args;
    }else{

        args.gpib_major = atoi(argv[1]);
        args.gpib_minor = atoi(argv[2]);
        args.vmax = atof(argv[3]);
        args.vmin = atof(argv[4]);
        args.imax = atof(argv[5]);
        args.qmax = atof(argv[6]);
        args.ncycles = atoi(argv[7]);

        int i = 8;
        if ((argv[8][0] == 'S') | (argv[8][0] == 's'))
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
                printf("\nFrequency %f entered, frequencies above 1Hz are not supported\n", args.frequency[args.num_frequencies]);
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

