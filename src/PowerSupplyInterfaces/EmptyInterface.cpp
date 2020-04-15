#include "EmptyInterface.hpp"

EMPTYInterface::EMPTYInterface(char serial_mode[256], char serial_value[256],
            double max_voltage, double min_voltage, double max_current, char filestring[255]):
               PsuInterface::PsuInterface(serial_mode, serial_value, max_voltage, min_voltage, max_current, filestring)
{
    #if DEBUG
     	printf("Start of EMPTYInterface Constructor\n");
    #endif // DEBUG

    sprintf(inst_, "*IDN?\n");
    Query(inst_, val_);
    printf("%s", val_);
    printf("\n");

    if(strncmp("Device IDN string",val_, 29))
    {
        printf("Device at address GPIB%i::%i::INSTR was not the expected device, exiting...\n", id, addr);
        exit(1);
    }

    sprintf(inst_, "*RST\n");
    Write(inst_);

    // sprintf(inst_, "CLR");
    // Write(inst_);

    sprintf(inst_, "UNT?\n");
    Query(inst_, val_);
    #if DEBUG
        printf(val_);
        printf("\n");
    #endif // DEBUG

	time(&t0);
	printf("EMPTYInterface (V%.2f): started at %s", EMPTY_INTERFACE_VERSION, ctime(&t0));
    // wbstr(device, "SU1:02.22"); // engage remote control mode


    #if DEBUG
 	    printf("End of EMPTYInterface Constructor\n");
    #endif // DEBUG

    //ctor
}

EMPTYInterface::~EMPTYInterface()
{
 	#if DEBUG
        printf("Start of EMPTYInterface Destructor\n");
    #endif // DEBUG

    sprintf(inst_, "OUTP:REL:STAT OFF\n");
    if(Write(inst_))   {   printf("Error: Error setting output relay off\n");   }

    sprintf(inst_, "OUTP:STAT OFF\n"); // connect output relay (not the same as output on)
    if(Write(inst_))   {   printf("Error: Error setting output state off\n");   }

    printf("Closing EMPTYInterface Device\n");
    #if DEBUG
        printf("End of  EMPTYInterface Destructor\n");
    #endif // DEBUG

    //dtor
}

int EMPTYInterface::SetOutput(double V, double I)
{
    #if DEBUG
	    printf("Call to EMPTYInterface::SetOutput\n");
    #endif // DEBUG
    return SMUVoltage(V,I);
}

int EMPTYInterface::GetOutput(double *V, double *I)
{
    int err = 0;
    #if DEBUG
	    printf("Call to EMPTYInterface::GetOutput\n");
    #endif // DEBUG

    sprintf(inst_, "MEAS:CURR:DC?");
    err = Query(inst_, val_);
    if (err)   {   printf("Error reading output current\n");    }
    #if DEBUG
        printf("QUERY: %s", val_);
    #endif // DEBUG

    *I = atof(val_);

    sprintf(inst_, "MEAS:VOLT:DC?");
    err = Query(inst_, val_);
    if (err)    {  printf("Error reading output voltage\n");    }
    #if DEBUG
        printf("QUERY: %s", val_);
    #endif // DEBUG
    *V = atof(val_);

    return err;
}

int EMPTYInterface::SMUVoltage(double V, double I)
{
    #if DEBUG
	    printf("Call to EMPTYInterface::SMUVoltage\n");
        printf("V: %f, I: %f\n", V, I);
    #endif // DEBUG

    SetVoltageRange(fabs(V));
    SetCurrentRange(fabs(I));
    sprintf(inst_, "SOUR:VOLT %1.3f\n", V);
    if(Write(inst_))   {   printf("Error: Error setting output voltage \n");    }
    sprintf(inst_, "SOUR:CURR %1.3f\n", fabs(I));
    if(Write(inst_))   {   printf("\nError: Error setting output current: %1.3f\n", I);    }

    mwait(2);
    return 0.0f;
}

int EMPTYInterface::SMUCurrent(double voltage_max, double voltage_min, double current)
{
    #if DEBUG
	    printf("Call to EMPTYInterface::SMUCurrent\n");
        printf("V: %f-%f, I: %f\n", voltage_max, voltage_min, current);
    #endif // DEBUG

    // if(fabs(I) < 0.001)
    // {
    //     I = 0;
    // }
    SetVoltageRange(fabs(voltage_max));
    SetCurrentRange(fabs(current));
    sprintf(inst_, "SOUR:VOLT %1.3f\n", voltage_max);
    if(Write(inst_))   {   printf("Error: Error setting output voltage \n");    }
    sprintf(inst_, "SOUR:CURR %1.3f\n", fabs(current));
    if(Write(inst_))   {   printf("\nError: Error setting output current: %1.3f\n", current);    }

    sprintf(inst_, "*WAI\n");
    if(Write(inst_))   {   printf("\nError: Error waiting for command completion\n");    }

    return 0.0f;
}

double EMPTYInterface::SetVoltageRange(double V)
{
    #if DEBUG
	    printf("Call to EMPTYInterface::SetVoltageRange\n");
    #endif // DEBUG

    return 0.0f;
}

double EMPTYInterface::SetCurrentRange(double I)
{
    #if DEBUG
	    printf("Call to EMPTYInterface::SetCurrentRange\n");
    #endif // DEBUG

    sprintf(inst_, "SENS:CURR:RANG %.6f", I);
    if(Write(inst_))   { printf("Error: Error setting current range"); }

    return 0.0f;
}

int EMPTYInterface::IsCurrentLimited(void)
{
    #if DEBUG
        printf("\nFUNCTION: EMPTYInterface::IsCurrentLimited\n");
    #endif // DEBUG

    uint32_t status = 0;

    sprintf(inst_, "STAT:OPER:COND?");
    if(Query(inst_, val_))    {   printf("ERROR: Error reading current trip limit\n"); }

    status = atoi(val_);
    #if DEBUG
        printf("Operaton Status Condition Register: \n");
        printf("0x%04x %i\n", status, status);
    #endif // DEBUG

    return ((status & 0x0800) || (status & 0x0400));

}

int EMPTYInterface::OutputOn()
{
    sprintf(inst_, "OUTP:STAT ON\n"); // turn off output (not the same as output on)
    return Write(inst_);
}

int EMPTYInterface::OutputOff()
{
    sprintf(inst_, "OUTP:STAT OFF\n"); // turn off output (not the same as output on)
    return Write(inst_);
}

int EMPTYInterface::ClearErrors()
{
    char buff[256];
    int error = 0;

    do // clear last errors if any remain
    {
        //visa::wbstr(device, "SYST:ERR?");
        //visa::rbstr(device, buff, 255);
    } while (atoi(buff) != 0);

    return 0;
}

int EMPTYInterface::CheckErrors()
{
    char buff[256];
    int error = 0;

    //visa::wbstr(device, "SYST:ERR?");
    //visa::rbstr(device, buff, 255);

    error = atoi(buff);
    if (error)
    {
        printf("\n");
        printf("%s", buff);
    }
    return error;
}
