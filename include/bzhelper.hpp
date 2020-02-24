struct bzargs
{
    int gpib_major = 0;
    int gpib_minor = 0;
    double vmax = 0;
    double vmin = 0;
    double imax = 0;
    double qmax = 0;
    int ncycles = 0;
    double frequency[16] = {0};
    bool single_sweep = false;
    int num_frequencies = 0;
    char filestring[256] = {'\0'};
};

bzargs bzparse(int argc, char ** argv, const char device_string[256]);
