struct bcargs
{ 
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
};

bcargs bcparse(int argc, char ** argv, char device_string[256]);
