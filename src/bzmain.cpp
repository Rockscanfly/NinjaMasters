#include "KeithleyInterface.hpp"
#include "HamegInterface.hpp"
#include "HP66332Interface.hpp"
#include "E5270Interface.hpp"
#include "W5000Interface.hpp"
#include "PsuInterface.hpp"
#include "bzhelper.hpp"

#include "string.h"
#include <stdio.h>
#include <stdlib.h>

#define VERSION_MAJOR 4
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#define PATCH_DATE "10/04/2020"
//#define CREATED_DATE "18/09/2019"

int main (int argc, char *argv[])
{
    printf("Battery Impedance, V%i.%i.%i \n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    printf("Last Updated: Vance Farrow %s\n", PATCH_DATE);

    device_list device_selection = None;
    std::string device_string;

    // Select correct device
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
        else if ((!(strcmp(argv[1], "W5000"))) | (!(strcmp(argv[1], "W5000"))))
        {
            device_selection = W5000;
            device_string = "WILDSCHUT5000";
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

    // process arguments
    bzargs args = bzparse(argc, argv, device_string);

    // print processed arguments for user
    // printf("VISA#: %i\n", args.gpib_major);
    // printf("gpib_minor: %i\n", args.gpib_minor);
    printf("vmax: %.3fV\n", args.max_voltage);
    printf("vmin: %.3fV\n", args.min_voltage);
    printf("imax: %.3fA\n", args.max_current);
    printf("qmax: %.5fAh\n", args.max_charge);
    printf("ncycles: %i\n", args.num_cycles);
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
            printf("frequency: %.3fuHz\n", args.frequency[i]*1000000);
        }
    }
    printf("filestring: %s\n", args.filestring);

    if(device_selection == None)
    {
        printf("Error, invalid device selected. Exiting...\n");
        exit(1);
    }

    // create pointer to device
    PsuInterface* p_device = nullptr;

    // create real device for pointer and assign
    switch (device_selection)
    {
        case Keithley:
        {
            // p_device = new KeithleyInterface(args.vmax, args.vmin, args.imax, args.filestring);
            p_device = new KeithleyInterface(argv[2], argv[3], args.max_voltage, args.min_voltage, args.max_current, args.filestring);
            break;
        }
        case Hameg:
        {
            // p_device = new HamegInterface(args.vmax, args.vmin, args.imax, args.filestring);
            p_device = new HamegInterface(argv[2], argv[3], args.max_voltage, args.min_voltage, args.max_current, args.filestring);
            break;
        }
        case HP66332:
        {
            // p_device = new HP66332Interface(args.vmax, args.vmin, args.imax, args.filestring);
            p_device = new HP66332Interface(argv[2], argv[3], args.max_voltage, args.min_voltage, args.max_current, args.filestring);
            break;
        }
        case E5270:
        {
            // p_device = new E5270Interface(args.vmax, args.vmin, args.imax, args.filestring);
            p_device = new E5270Interface(argv[2], argv[3], args.max_voltage, args.min_voltage, args.max_current, args.filestring);
            break;
        }
        case W5000:
        {
            // p_device = new E5270Interface(args.vmax, args.vmin, args.imax, args.filestring);
            p_device = new W5000Interface(argv[2], argv[3], args.max_voltage, args.min_voltage, args.max_current, args.filestring);
            break;
        }
        case None:
        default:
            printf("Error, invalid device selected. Exiting...\n");
            exit(1);
    }

    char filename[256];

    printf("\n Starting Sweep\n");
    // Step through frequencies and produce waveform
    if (!(args.single_sweep))
    {
        if(args.filestring[0] != '\0')
        {
            sprintf(filename, "%s_%iuHz%itone.tvi", args.filestring, (int)(args.frequency[0]*1e6), args.num_frequencies);
            p_device->ChangeDataFile(filename);
        }
        p_device->Waveform(args.max_voltage, args.min_voltage, args.max_current, args.max_charge, args.num_cycles, args.frequency);

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
            p_device->Waveform(args.max_voltage, args.min_voltage, args.max_current, args.max_charge, args.num_cycles, frequency_holder);

        }
    }
    delete(p_device);
}
