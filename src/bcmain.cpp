#include "KeithleyInterface.hpp"
#include "HamegInterface.hpp"
#include "HP66332Interface.hpp"
#include "E5270Interface.hpp"
#include "W5000Interface.hpp"
#include "PsuInterface.hpp"
#include "bchelper.hpp"

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
            device_string = "Keysight E5270B Precision IV Analyzer";
        }
        else if ((!(strcmp(argv[1], "W5000"))) | (!(strcmp(argv[1], "w5000"))))
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
    bcargs args = bcparse(argc, argv, device_string);

    // // print processed arguments for user
    // printf("VISA#: %i\n", args.gpib_major);
    // printf("gpib_minor: %i\n", args.gpib_minor);
    printf("ncycles: %i\n", args.num_cycles);
    printf("vmax: %.3fV\n", args.max_voltage);
    printf("vmin: %.3fV\n", args.min_voltage);
    printf("imax: %.3fA\n", args.max_current);
    printf("iend: %.3fA\n", args.end_current);
    printf("qend: %.3f%%Q\n", args.end_charge*100);
    printf("timeout: %.0fs\n", args.timeout);
    printf("trelax: %.0fs\n", args.relax_time);
    printf("filestring: %s\n", args.filestring);

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
            // Function not written yet, program will not behave as expected when using this device.
            printf("Error, E5270Interface::IsCurrentLimited() not defined, Exiting...\n");
            exit(1);
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
    if (args.filestring[0] != '\0') {
        sprintf(filename, "%s.tvi", args.filestring);
    } else {
        sprintf(filename, "%s.tvi", "DataFile");
    }
    if (p_device->ChangeDataFile(filename)){
        printf("Failed to open data file\n");
        exit(1);
    }


    char logdata[256];
    sprintf(logdata, "%s", );
    p_device->WriteLog(device_string.c_str());
    sprintf(logdata,"ncycles: %i\n", args.num_cycles);
    p_device->WriteLog(logdata);
    sprintf(logdata,"vmax: %.3fV\n", args.max_voltage);
    p_device->WriteLog(logdata);
    sprintf(logdata,"vmin: %.3fV\n", args.min_voltage);
    p_device->WriteLog(logdata);
    sprintf(logdata,"imax: %.3fA\n", args.max_current);
    p_device->WriteLog(logdata);
    sprintf(logdata,"iend: %.3fA\n", args.end_current);
    p_device->WriteLog(logdata);
    sprintf(logdata,"qend: %.3f%%Q\n", args.end_charge*100);
    p_device->WriteLog(logdata);
    sprintf(logdata,"timeout: %.0fs\n", args.timeout);
    p_device->WriteLog(logdata);
    sprintf(logdata,"trelax: %.0fs\n", args.relax_time);
    p_device->WriteLog(logdata);
    sprintf(logdata,"filestring: %s\n", args.filestring);
    p_device->WriteLog(logdata);



    printf("\n Starting Cycle\n");

    // cycle battery
    p_device->CycleBattery(args.num_cycles, args.max_voltage, args.min_voltage, args.max_current,
        args.end_current, args.end_charge, args.timeout, args.relax_time);
    delete(p_device);
}
