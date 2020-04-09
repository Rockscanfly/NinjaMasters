#include "stdio.h"
#include <stdlib.h>
#include <string>

enum device_list {
    Keithley,
    Hameg,
    HP66332,
    E5270,
    None
};

struct bzargs
{
    char serial_mode[255] = {'\0'};
    char serial_value[255] = {'\0'};
    double max_voltage = 0;
    double min_voltage = 0;
    double max_current = 0;
    double max_charge = 0;
    int num_cycles = 0;
    double frequency[16] = {0};
    bool single_sweep = false;
    int num_frequencies = 0;
    char filestring[256] = {'\0'};
    bool args_good = false;
};

bzargs bzparse(int argc, char ** argv, std::string device_string);
