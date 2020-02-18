#include "bzhelper.hpp"
#include "stdio.h"
#include <stdlib.h>

bzargs bzparse(int argc, char ** argv, char device_string[256]) 
{

    bzargs args; 

    if((argc != 11) && (argc != 12))
    {   // if(argc < 9)
    if(argc != 11)
    {
        printf("Invalid number of parameters %i\n", argc-1);
        printf("Expected usage:\n");
        printf("bz5270.exe <VISA#> <gpib_addr> <vmax> <vmin> <imax> <qmax> <ncycles> <frequency> [additional frequencies] [filename]\n");
        printf("\n");
        // printf("This program requires a Keithley 2460 SourceMeter connected to the host machine accessible at the specified\n");
        // printf("GPIB address, and the visa driver interface to be installed on the host\n");
        // printf("The expected device address is in the format: GPIB<VISA#>::<gpib_addr>::INSTR\n");
        // printf("\n");
        // printf("The program is designed to cycle a battery attached to the Keithley 2460 between the specified maximum and\n");
        // printf("minimum voltages at the specified maximum current. The program expects the voltage and current ranges to\n");
        // printf("accurately represent the safe operating values of the battery and provides no guarantee it will not\n");
        // printf("damage any attached device\n");
        // printf("\n");
        // printf("Data and log files will be created in the same directory as BCK\n");
        // printf("The optional argument [filestring] will overwrite the default file names DataFile.tvi and LogFile.log\n");
        // printf("and results will be stored in [filestring].tvi and [filestring].log\n");
        // printf("The .tvi file stores measurements in three tab separated columns time(s) volts(V) current(A)\n");
        // printf("The .log file stores timestamped plain english descriptions of milestones reached by the program\n");
        // printf("\n");
        printf("<VISA#> is the GPIB address value of the primary switch that the Keithley 2460 is attached to\n");
        printf("<gpib_addr> is the GPIB subaddress value of Keithley 2460 attached to the GPIB switch at <VISA#>\n");
        printf("<vmax> is the maximum voltage up to which the battery will be charged, the program will exit if this limit is reached during cycling\n");
        printf("<vmin> is the minimum voltage down to which the battery will be discharged, the program will exit if this limit is reached during cycling\n");
        printf("<imax> is the maximum current, in Amps, that will be sourced or sunk to force a waveform into the battery\n");
        printf("<qmax> is the maximum charge, in Amphere Hours, that will sourced or sunk to force a waveform into the battery\n");
        printf("<ncycles> is the number of times the attached battery will be cycled at the lowest given frequency\n");
        printf("<frequency> is the frequency of the current waveform that will be produced \n");
        printf("[filestring] sets the name of the target .tvi and .log files, NOTE existing files with the same name will be OVERWRITTEN\n");
		
		args.gpib_major = -1;
        return args;
    }else{
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
                    args.vmax = atof(argv[i]);
                    break;

                case 4:
                    args.vmin = atof(argv[i]);
                    break;

                case 5:
                    args.imax = atof(argv[i]);
                    break;

                case 6:
                    args.qmax = atof(argv[i]);
                    break;

                case 7:
                    args.ncycles = atoi(argv[i]);
                    break;

                case 8:
                    args.frequency[0] = atof(argv[i]);
                    break;

                case 9:
                    args.frequency[1] = atof(argv[i]);
                    break;

                case 10:
                    sprintf(args.filestring, "%s", argv[i]);
                    break;

                default:
                   printf("Error unknown argument %s\n", argv[i]);
					args.gpib_major = -1;
                    return args;

            }
        }
    }
	return args;
}

