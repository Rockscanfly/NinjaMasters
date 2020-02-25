#include "KeithleyInterface.hpp"
#include "HamegInterface.hpp"
#include "HP66332Interface.hpp"
#include "E5270Interface.hpp"
#include "PSUInterface.hpp"
#include "bzhelper.hpp"

#include "string.h"
#include <stdio.h>
#include <stdlib.h>

#define VERSION_MAJOR 3
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#define PATCH_DATE "25/02/2020"
//#define CREATED_DATE "18/09/2019"

int main (int argc, char *argv[])
{
    printf("Battery Impedance, V%i.%i.%i \n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    printf("Last Updated: Vance Farrow %s\n", PATCH_DATE);

    device_list device_selection = None;
    std::string device_string;

    if(argc > 1)
    {
        if ((!(strcmp(argv[1], "Keithley"))) | (!(strcmp(argv[1], "keithley"))))
        {
            device_selection = Keithley;
            device_string = "Keithley 2460A Sourcemeter";

        }
        else if ((!(strcmp(argv[1], "Hameg"))) | (!(strcmp(argv[1], "hameg"))))
        {
            device_selection = Hameg;
            device_string = "HM8143 Arbitrary Power Supply";

        }
        else if ((!(strcmp(argv[1], "HP66332"))) | (!(strcmp(argv[1], "hp66332"))))
        {
            device_selection = HP66332;
            device_string = "Agilent Model 66332A System DC Power Supply";

        }
        else if ((!(strcmp(argv[1], "E5270"))) | (!(strcmp(argv[1], "e5270"))))
        {
            device_selection = E5270;
            device_string = "Keysight E5270B Precision IV Analyzer";
        }
        else
        {
            device_selection = None;
            device_string = "device";
        }
    }
    else
    {
        device_string = "device";
    }

    bzargs args = bzparse(argc, argv, device_string);

    printf("VISA#: %i\n", args.gpib_major);
    printf("gpib_minor: %i\n", args.gpib_minor);
    printf("vmax: %f\n", args.vmax);
    printf("vmin: %f\n", args.vmin);
    printf("imax: %f\n", args.imax);
    printf("qmax: %f\n", args.qmax);
    printf("ncycles: %i\n", args.ncycles);
    printf("n_freq: %i\n", args.num_frequencies);
    if(args.single_sweep)
    {
        printf("Single Frequency Sweep?: true\n");
    } else {
        printf("Single Frequency Sweep?: false\n");
    }
    for(int i = 0; i < 16; i++)
    {
        if (args.frequency[i] != 0)
        {
            printf("frequency: %f\n", args.frequency[i]);
        }
    }
    printf("filestring: %s\n", args.filestring);

    if(device_selection == None)
    {
        printf("Error, invalid device selected. Exiting...\n");
        exit(1);
    }

    PSUInterface* p_device = nullptr;
    switch (device_selection)
    {
        case Keithley:
        {
            KeithleyInterface device(args.gpib_major, args.gpib_minor, KEITHLEY_INTERFACE_CHANNEL, args.vmax, args.vmin, args.imax, args.filestring);
            p_device = &device;
            break;
        }
        case Hameg:
        {   HamegInterface device(args.gpib_major, args.gpib_minor, HAMEG_INTERFACE_CHANNEL, args.vmax, args.vmin, args.imax, args.filestring);
            p_device = &device;
            break;
        }
        case HP66332:
        {
            HP66332Interface device(args.gpib_major, args.gpib_minor, 0, args.vmax, args.vmin, args.imax, args.filestring);
            p_device = &device;
            break;
        }
        case E5270:
        {
            E5270Interface device(args.gpib_major, args.gpib_minor, E5270_INTERFACE_CHANNEL, args.vmax, args.vmin, args.imax, args.filestring);
            p_device = &device;
            break;
        }
        case None:
        default:
            printf("Error, invalid device selected. Exiting...\n");
            exit(1);
    }

    char filename[256];

    if (!(args.single_sweep))
    {
        if(args.filestring[0] != '\0')
        {
            sprintf(filename, "%s_%iuHz%itone.tvi", args.filestring, (int)(args.frequency[0]*1e6), args.num_frequencies);
            p_device->ChangeDataFile(filename);
        }
        p_device->Waveform(args.vmax, args.vmin, args.imax, args.qmax, args.ncycles, args.frequency);

    } else {

        double frequency_holder[16] = {0};

        for(int i = 0; i < args.num_frequencies; i++)
        {
            if(args.filestring[0] != '\0')
            {
                sprintf(filename, "%s_%iuHz.tvi", args.filestring, (int)(args.frequency[i]*1e6));
                p_device->ChangeDataFile(filename);
            }
            frequency_holder[0] = args.frequency[i];
            p_device->Waveform(args.vmax, args.vmin, args.imax, args.qmax, args.ncycles, frequency_holder);

        }
    }

}
