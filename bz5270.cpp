#include "E5270Interface.hpp"
#include "bzhelper.hpp"

#include "string.h"
#include <stdio.h>
#include <stdlib.h>

#define VERSION_MAJOR 2
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#define PATCH_DATE "14/02/2020"

int main (int argc, char *argv[])
{
    printf("Battery Impedance 5270, BZ5270 V%i.%i.%i \n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    printf("Last Updated: Vance Farrow %s\n", PATCH_DATE);

    const char device_string[256] = "E5270";
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

    E5270Interface device(args.gpib_major, args.gpib_minor, E5270_INTERFACE_CHANNEL, args.vmax, args.vmin, args.imax, args.filestring);

    PSUInterface* p_device = &device;

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
