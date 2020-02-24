#include "HP66332Interface.hpp"
#include "bchelper.hpp"

#include "string.h"
#include <stdio.h>
#include <stdlib.h>

#define VERSION_MAJOR 2
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#define PATCH_DATE "14/02/2020"
//#define CREATED_DATE "18/09/2019"

int main (int argc, char *argv[])
{
    printf("Battery Cycler 66332, BC66332 V%i.%i.%i \n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    printf("Last Updated: Vance Farrow %s\n", PATCH_DATE);

    const char device_string[256] = "HP66332";
    bcargs args = bcparse(argc, argv, device_string);

    printf("gpib_major: %i\n", args.gpib_major);
    printf("gpib_minor: %i\n", args.gpib_minor);
    printf("ncycles: %i\n", args.ncycles);
    printf("vmax: %f\n", args.vmax);
    printf("vmin: %f\n", args.vmin);
    printf("imax: %f\n", args.imax);
    printf("iend: %f\n", args.iend);
    printf("qend: %f\n", args.qend);
    printf("timeout: %f\n", args.timeout);
    printf("trelax: %f\n", args.trelax);
    printf("filestring: %s\n", args.filestring);

    HP66332Interface device(args.gpib_major, args.gpib_minor, 0, args.vmax, args.vmin, args.imax, args.filestring);

    PSUInterface* p_device = &device;
    p_device->CycleBattery(args.ncycles, args.vmax, args.vmin, args.imax, args.iend, args.qend, args.timeout, args.trelax);

}
