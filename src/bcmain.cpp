#include "KeithleyInterface.hpp"
#include "HamegInterface.hpp"
#include "HP66332Interface.hpp"
#include "E5270Interface.hpp"
#include "PsuInterface.hpp"
#include "bchelper.hpp"

#include "string.h"
#include <stdio.h>
#include <stdlib.h>

#define VERSION_MAJOR 3
#define VERSION_MINOR 0
#define VERSION_PATCH 2
#define PATCH_DATE "18/03/2020"
//#define CREATED_DATE "18/09/2019"

int main (int argc, char *argv[])
{
    printf("Battery Cycler, V%i.%i.%i \n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    printf("Last Updated: Vance Farrow %s\n", PATCH_DATE);

    device_list device_selection = None;
    std::string device_string;

    // select correct device
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
            device_string = "66332A System DC Power Supply";

        }
        else if ((!(strcmp(argv[1], "E5270"))) | (!(strcmp(argv[1], "e5270"))))
        {
            device_selection = E5270;
            device_string = "Keysight E5270B Precision IV Analyzer";        }
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
    bcargs args = bcparse(argc, argv, device_string);

    // print processed arguments for user
    printf("VISA#: %i\n", args.gpib_major);
    printf("gpib_minor: %i\n", args.gpib_minor);
    printf("ncycles: %i\n", args.ncycles);
    printf("vmax: %.3fV\n", args.vmax);
    printf("vmin: %.3fV\n", args.vmin);
    printf("imax: %.3fA\n", args.imax);
    printf("iend: %.3fA\n", args.iend);
    printf("qend: %.3f%%Q\n", args.qend*100);
    printf("timeout: %.0fs\n", args.timeout);
    printf("trelax: %.0fs\n", args.trelax);
    printf("filestring: %s\n", args.filestring);

    // create pointer to device
    PsuInterface* p_device = nullptr;


    // create real device for pointer and assign
    switch (device_selection)
    {
        case Keithley:
        {
            // p_device = new KeithleyInterface(args.vmax, args.vmin, args.imax, args.filestring);
            p_device = new KeithleyInterface(args.filestring);
            break;
        }
        case Hameg:
        {
            // p_device = new HamegInterface(args.vmax, args.vmin, args.imax, args.filestring);
            p_device = new HamegInterface(args.filestring);
            break;
        }
        case HP66332:
        {
            // p_device = new HP66332Interface(args.vmax, args.vmin, args.imax, args.filestring);
            p_device = new HP66332Interface(args.filestring);
            break;
        }
        case E5270:
        {
            // Function not written yet, program will not behave as expected when using this device.
            printf("Error, E5270Interface::IsCurrentLimited() not defined, Exiting...\n");
            exit(1);
            // p_device = new E5270Interface(args.vmax, args.vmin, args.imax, args.filestring);
            p_device = new E5270Interface(args.filestring);
            break;

        }
        case None:
        default:
            printf("Error, invalid device selected. Exiting...\n");
            exit(1);
    }

    printf("\n Starting Cycle\n");
    char filename[256];
    sprintf(filename, "%s.tvi", args.filestring);
    p_device->ChangeDataFile(filename);

    // cycle battery
    p_device->CycleBattery(args.ncycles, args.vmax, args.vmin, args.imax, args.iend, args.qend, args.timeout, args.trelax);
    delete(p_device);
}
