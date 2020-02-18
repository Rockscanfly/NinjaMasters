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
    printf("Battery Cycler 66332, BC66332 V%i.%i.%i \nAuthor: Vance Farrow %s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, PATCH_DATE);
    printf("Last Updated: VGF Oct 2019\n");

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

    HP66332Interface device(gpib_major, gpib_minor, 0, vmax, vmin, imax, filestring);

    PSUInterface* p_device = &device;
    p_device->CycleBattery(ncycles, vmax, vmin, imax, iend, qend, timeout, trelax);

}
