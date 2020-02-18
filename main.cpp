#include "HamegInterface.hpp
#include "KeithleyInterface.hpp"


#include "string.h"
#include <stdio.h>
#include <stdlib.h>

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#define PATCH_DATE "10/10/2019"
//#define CREATED_DATE "18/09/2019"

/*
Battery Cycler Keithley, BCK V%i.%i.%i \nAuthor: Vance Farrow %s\n, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, PATCH_DATE);
VGF Oct 2019
Invalid number of parameters %i
Usage: bck.exe <VISA#> <gpib_addr> <ncycles> <Vmax> <Vmin> <Imax> <Iend> <Qend> <timeout> <relaxtime> [filestring]

This program requires a Keithley 2460 SourceMeter connected to the host machine accessible at the specified
GPIB address, and the visa driver interface to be installed on the host
The expected device address is in the format: GPIB<VISA#>::<gpib_addr>::INSTR

The program is designed to cycle a battery attached to the Keithley 2460 between the specified maximum and
minimum voltages at the specified maximum current. The program expects the voltage and current ranges to
accurately represent the safe operating values of the battery and provides no guarantee it will not
damage any attached device

Data and log files will be created in the same directory as BCK
The optional argument [filestring] will overwrite the default file names DataFile.tvi and LogFile.log
and results will be stored in [filestring].tvi and [filestring].log
The .tvi file stores measurements in three tab separated columns time(s) volts(V) current(A)
The .log file stores timestamped plain english descriptions of milestones reached by the program

<VISA#> is the GPIB address value of the primary switch that the Keithley 2460 is attached to
<gpib_addr> is the GPIB subaddress value of Keithley 2460 attached to the GPIB switch at <VISA#>
<ncycles> is the number of times the attached battery will be fully discharged and then fully charged
<vmax> is the maximum voltage up to which the battery will be charged
<vmin> is the minimum voltage down to which the battery will be discharged
<imax> is the maximum current, in Amps, that Keithley 2460 will source or sink to charge or discharge the battery
<iend> is the minimum current, in Amps, flowing when the program will decide the battery is fullly charged or discharged
<qend> is the final fraction of charge left in the battery as a percent of fully charged, NOTE not the same as voltage
<timeout> is the maximum amount of time the current will be allowed to decay after falling below <imax>
	before the program decides the battery is fully charged or discharged
<relaxtime> is the time the voltage will be allowed to relax after all cycling is complete and the
	charge level has reached <quend>, zero current will flow but the Keithley will continue measurements for this time
[filestring] sets the name of the target .tvi and .log files, NOTE existing files with the same name will be OVERWRITTEN
*/

int main (int argc, char *argv[])
{
    printf("Battery Cycler Keithley, BCK V%i.%i.%i \nAuthor: Vance Farrow %s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, PATCH_DATE);
    printf("Last Updated: VGF Oct 2019\n");
//    printf("BCK, the battery cycler keithly, is a command line interface to control a keithly 2460 source measurement unit\n");
//    printf("Expected usage:\n");
//    printf("bck.exe <gpib_major> <gpib_minor> <ncycles> <vmax> <vmin> <imax> <iend> <qend> <timeout> <trelax>\n");
//
//    argfield args;
//
//    argparse(argc, argv, &args);

//    cxxopts::parse(argc, argv);

    int gpib_major = 0;
    int gpib_minor = 0;
    int ncycles = 0;
    double vmin = 0;
    double vmax = 0;
    double imax = 0;
    double iend = 0;
    double qend = 0;
    double timeout = 0;
    double trelax = 0;
    char filestring[255] = {'\0'};

    if((argc != 11) && (argc != 12))
    {
        printf("Invalid number of parameters %i\n", argc-1);
        printf("Usage: bck.exe <VISA#> <gpib_addr> <ncycles> <Vmax> <Vmin> <Imax> <Iend> <Qend> <timeout> <relaxtime> [filestring]\n");
        printf("\n");
        printf("This program requires a Keithley 2460 SourceMeter connected to the host machine accessible at the specified\n");
        printf("GPIB address, and the visa driver interface to be installed on the host\n");
        printf("The expected device address is in the format: GPIB<VISA#>::<gpib_addr>::INSTR\n");
        printf("\n");
        printf("The program is designed to cycle a battery attached to the Keithley 2460 between the specified maximum and\n");
        printf("minimum voltages at the specified maximum current. The program expects the voltage and current ranges to\n");
        printf("accurately represent the safe operating values of the battery and provides no guarantee it will not\n");
        printf("damage any attached device\n");
        printf("\n");
        printf("Data and log files will be created in the same directory as BCK\n");
        printf("The optional argument [filestring] will overwrite the default file names DataFile.tvi and LogFile.log\n");
        printf("and results will be stored in [filestring].tvi and [filestring].log\n");
        printf("The .tvi file stores measurements in three tab separated columns time(s) volts(V) current(A)\n");
        printf("The .log file stores timestamped plain english descriptions of milestones reached by the program\n");
        printf("\n");
        printf("<VISA#> is the GPIB address value of the primary switch that the Keithley 2460 is attached to\n");
        printf("<gpib_addr> is the GPIB subaddress value of Keithley 2460 attached to the GPIB switch at <VISA#>\n");
        printf("<ncycles> is the number of times the attached battery will be fully discharged and then fully charged\n");
        printf("<vmax> is the maximum voltage up to which the battery will be charged\n");
        printf("<vmin> is the minimum voltage down to which the battery will be discharged\n");
        printf("<imax> is the maximum current, in Amps, that Keithley 2460 will source or sink to charge or discharge the battery\n");
        printf("<iend> is the minimum current, in Amps, flowing when the program will decide the battery is fullly charged or discharged\n");
        printf("<qend> is the final fraction of charge left in the battery as a percent of fully charged, NOTE not the same as voltage\n");
        printf("<timeout> is the maximum amount of time the current will be allowed to decay after falling below <imax>\n");
        printf("    before the program decides the battery is fully charged or discharged\n");
        printf("<relaxtime> is the time the voltage will be allowed to relax after all cycling is complete and the\n");
        printf("    charge level has reached <quend>, zero current will flow but the Keithley will continue measurements for this time\n");
        printf("[filestring] sets the name of the target .tvi and .log files, NOTE existing files with the same name will be OVERWRITTEN\n");

        return 1;
    }else{
        for(int i = 1; i < argc; i++)
        {
            switch(i) {
                case 1:
                    gpib_major = atoi(argv[i]);
                    break;

                case 2:
                    gpib_minor = atoi(argv[i]);
                    break;

                case 3:
                    ncycles = atoi(argv[i]);
                    break;

                case 4:
                    vmax = atof(argv[i]);
                    break;

                case 5:
                    vmin = atof(argv[i]);
                    break;

                case 6:
                    imax = atof(argv[i]);
                    break;

                case 7:
                    iend = atof(argv[i]);
                    break;

                case 8:
                    qend = atof(argv[i]);
                    break;

                case 9:
                    timeout = atof(argv[i]);
                    break;

                case 10:
                    trelax = atof(argv[i]);
                    break;

                case 11:
                    sprintf(filestring, "%s", argv[i]);
                    break;

                default:
                   printf("Error unknown argument %s\n", argv[i]);
                    return 1;

            }
        }
    }


    printf("gpib_major: %i\n", gpib_major);
    printf("gpib_minor: %i\n", gpib_minor);
    printf("ncycles: %i\n", ncycles);
    printf("vmax: %f\n", vmax);
    printf("vmin: %f\n", vmin);
    printf("imax: %f\n", imax);
    printf("iend: %f\n", iend);
    printf("qend: %f\n", qend);
    printf("timeout: %f\n", timeout);
    printf("trelax: %f\n", trelax);
    printf("filestring: %s\n", filestring);


//    PSUInterface(int id, int addr, int channel, int Vmax, int Vmin, int Imax, int capacity);
//    if (argc[4] == 'H')
//    {
//        HamegInterface device(0, 1, 2, 3, 4, 5, 6);
//
//    }
//    if (argc[4] == 'K')
//    {
        HamegInterface device(gpib_major, gpib_minor, KEITHLEY_INTERFACE_CHANNEL, vmax, vmin, imax, filestring);

//    }

    PSUInterface* p_device = &device;
//    p_device->CycleBattery(ncycles, vmax, vmin, imax, ipercent, timeout);
    p_device->CycleBattery(ncycles, vmax, vmin, imax, iend, qend, timeout, trelax);

//    printf("%i\n", p_device->GetVisaID());
//    printf("%i\n", p_device->GetSubAddr());
//    printf("%i\n", p_device->GetChannel());
//    printf("%i\n", p_device->GetVMax());
//    printf("%i\n", p_device->GetVMin());
//    printf("%i\n", p_device->GetIMax());
//    printf("%i\n", p_device->GetRatedCapacity());
//
//    double V = 0.0;
//    double I = 0.0;
//    p_device->SetOutput(1.40, 0.001);
//    mwait(2000);
//    p_device->GetOutput(&V, &I);
//    printf("V:%f, I:%f\n", V, I);
//    mwait(2000);
//    p_device->GetOutput(&V, &I);
//    printf("V:%f, I:%f\n", V, I);
//    mwait(2000);
//    p_device->GetOutput(&V, &I);
//    printf("V:%f, I:%f\n", V, I);
//    p_device->Charge(2.0, 4.3);
//    p_device->Discharge(2.0, 3.5);
//    p_device->CycleBattery(2.0, 1.0, 3.4);
//    p_device->Waveform(2.0, 0.5, 2.0, 100e-6);




}
