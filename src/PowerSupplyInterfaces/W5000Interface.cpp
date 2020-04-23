#include "W5000Interface.hpp"

W5000Interface::W5000Interface(char serial_mode[256], char serial_value[256],
            double max_voltage, double min_voltage, double max_current, char filestring[255]):
               PsuInterface::PsuInterface(serial_mode, serial_value, max_voltage, min_voltage, max_current, filestring)
{
    #if DEBUG
     	printf("Start of WILDSCHUT5000 Interface Constructor\n");
    #endif // DEBUG

    printf("Checking Device ID\n");
    sprintf(inst_, "#*IDN?");
    device_->Write(inst_);
    device_->Read(val_);
    printf("%s\n", val_);

    if(strncmp("GORDIE Instruments, WILDSCHUT5000", val_, 22))
    {
        printf("Device at address GPIB%i::%i::INSTR was not the expected device, exiting...\n", 1, 1);
        exit(1);
    }

    sprintf(inst_, "#RST\n"); // reset
    device_->Write(inst_);

    sprintf(inst_, "#CLR\n"); // connect output relay (not the same as output on)
    if(Write(inst_))   {   printf("Error: Error in initial clear\n");   }

    sprintf(inst_, "#STA");
    Query(inst_, val_);
    #if DEBUG
    printf("%s", val_);
    printf("\n");
    #endif // DEBUG

	time(&t0);
	printf("W5000Interface (V%.2f): started at %s", W5000_INTERFACE_VERSION, ctime(&t0));

    #if DEBUG
 	    printf("End of W5000 Interface Constructor\n");
    #endif // DEBUG

    //ctor
}

W5000Interface::~W5000Interface()
{
    #if DEBUG
        printf("Start of W5000 Interface Destructor\n");
    #endif // DEBUG

    // turn off output and disengage remote mode
    sprintf(inst_, "#OP0");
    Write(inst_);

    printf("Closing W5000Interface Device\n");
    #if DEBUG
        printf("End of W5000 Interface Destructor\n");
    #endif // DEBUG

    //dtor
}

int W5000Interface::SetOutput(double V, double I)
{
    #if DEBUG
        printf("Call to W5000Interface::SetOutput\n");
        printf(">> %f, %f\n", V, I);
    #endif // DEBUG
    sprintf(inst_, "#SU%i:%2.2f\n", this->channel, V);
    if(Write(inst_))   {   printf("Error: Error setting output voltage \n");    }
    sprintf(inst_, "#SI%i:%1.3f\n", this->channel, fabs(I));
    if(Write(inst_))   {   printf("\nError: Error setting output current: %1.3f\n", I);    }

    mwait(2);
    return 0;
}

int W5000Interface::GetOutput(double *V, double *I)
{
    #if DEBUG
        printf("Call to W5000Interface::GetOutput\n");
    #endif // DEBUG

    mwait(2);

    sprintf(inst_, "#MU%i\n", this->channel);
    if(Query(inst_, val_))   {   printf("Error: Error getting output voltage \n");    }
    *V = atof(&val_[3]);

    sprintf(inst_, "#MI%i\n", this->channel);
    if(Query(inst_, val_))   {   printf("Error: Error getting output current \n");    }
    *I = atof(&val_[3]);

    return 0;
}


int W5000Interface::SMUVoltage(double V, double I)
{
    #if DEBUG
        printf("Call to W5000Interface::SMUVoltage\n");
        printf("V: %f, I: %f\n", V, I);
    #endif // DEBUG

    SetOutput(V, I);

    mwait(20);
    return 0;
}

int W5000Interface::SMUCurrent(double voltage_max, double voltage_min, double current_t)
{
    #if DEBUG
        printf("Call to W5000Interface::SMUCurrent\n");
        printf("V: %f-%f, I: %f\n", voltage_max, voltage_min, current_t);
    #endif // DEBUG

    if (current_t >= 0)
    {
        return SetOutput(voltage_max,current_t);
    }
    else
    {
        return SetOutput(voltage_min,current_t);
    }
    mwait(20);

    return 0;
}

double W5000Interface::SetVoltageRange(double V)
{
    #if DEBUG
        printf("Call to W5000Interface::SetVoltageRange\n");
    #endif // DEBUG

    return 0.0f;
}

double W5000Interface::SetCurrentRange(double I)
{
    #if DEBUG
        printf("Call to W5000Interface::SetCurrentRange\n");
    #endif // DEBUG

    return 0.0f;
}

int W5000Interface::IsCurrentLimited(void)
{
    #if DEBUG
        printf("\nFUNCTION: W5000Interface::IsCurrentLimited\n");
    #endif // DEBUG

    uint32_t status = 0;
    sprintf(inst_, "#STA?");
    if(Query(inst_, val_))    {   printf("ERROR: Error reading current trip limit\n"); }
    #if DEBUG
        printf("%s\n", val_);
    #endif // DEBUG

    if(this->channel == 1)
    {
        #if DEBUG
            printf("%i\n", (strncmp(&val_[4], "CC1", 3) == 0));
        #endif // DEBUG
        return (strncmp(&val_[4], "CC1", 3) == 0);
    }
    else if(this->channel == 2)
    {
        #if DEBUG
            printf("%i\n", (strncmp(&val_[8], "CC2", 3) == 0));
        #endif // DEBUG
        return (strncmp(&val_[8], "CC2", 3) == 0);
    }
    else
    {
        printf("Error: Invalid channel selected");
        exit(1);
    }
}

int W5000Interface::OutputOn()
{
    int err = 0;
    sprintf(inst_, "#OP1\n"); // turn off output (not the same as output on)
    err = Write(inst_);
    mwait(1000);
    return err;
}

int W5000Interface::OutputOff()
{
    sprintf(inst_, "#OP0\n"); // turn off output (not the same as output on)
    return Write(inst_);
}

int W5000Interface::ClearErrors()
{
    return 0;
}

int W5000Interface::CheckErrors()
{
    return 0;
}

