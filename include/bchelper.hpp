#include "stdio.h"
#include <stdlib.h>
#include <string>

enum device_list {
    Keithley,
    Hameg,
    HP66332,
    E5270,
    W5000,
    None
};

struct bcargs
{
    char serial_mode[255] = {'\0'};
    char serial_value[255] = {'\0'};
    int num_cycles = 0;
    double max_voltage = 0;
    double min_voltage = 0;
    double max_current = 0;
    double end_current = 0;
    double end_charge = 0;
    double timeout = 0;
    double relax_time = 0;
    char filestring[128] = {'\0'};
    bool args_good = false;

};

bcargs bcparse(int argc, char ** argv, std::string device_string);
