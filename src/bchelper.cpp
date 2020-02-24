#include "bchelper.hpp"
#include "stdio.h"
#include <stdlib.h>

bcargs bcparse(int argc, char ** argv, const char device_string[256])
{

    bcargs args;

    if((argc != 11) && (argc != 12))
    {
        printf("Invalid number of parameters %i\n", argc-1);
        printf("Usage: %s <VISA#> <gpib_addr> <ncycles> <Vmax> <Vmin> <Imax> <Iend> <Qend> <timeout> <relaxtime> [filestring]\n", argv[0]);
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
        printf("<VISA#> is the GPIB address value of the primary switch that the %s is attached to\n", device_string);
        printf("<gpib_addr> is the GPIB subaddress value of %s attached to the GPIB switch at <VISA#>\n", device_string);
        printf("<ncycles> is the number of times the attached battery will be fully discharged and then fully charged\n");
        printf("<vmax> is the maximum voltage up to which the battery will be charged\n");
        printf("<vmin> is the minimum voltage down to which the battery will be discharged\n");
        printf("<imax> is the maximum current, in Amps, that %s will source or sink to charge or discharge the battery\n", device_string);
        printf("<iend> is the minimum current, in Amps, flowing when the program will decide the battery is fullly charged or discharged\n");
        printf("<qend> is the final fraction of charge left in the battery as a percent of fully charged, NOTE not the same as voltage\n");
        printf("<timeout> is the maximum amount of time the current will be allowed to decay after falling below <imax>\n");
        printf("    before the program decides the battery is fully charged or discharged\n");
        printf("<relaxtime> is the time the voltage will be allowed to relax after all cycling is complete and the\n");
        printf("    charge level has reached <quend>, zero current will flow but the %s will continue measurements for this time\n", device_string);
        printf("[filestring] sets the name of the target .tvi and .log files, NOTE existing files with the same name will be OVERWRITTEN\n");
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
                printf("Error unknown argument %s\n", argv[i]);
                    args.gpib_major = -1;

            }
        }
    }

    return args;
}