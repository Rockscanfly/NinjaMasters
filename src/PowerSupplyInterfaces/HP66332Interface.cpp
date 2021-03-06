#include "HP66332Interface.hpp"

HP66332Interface::HP66332Interface(char serial_mode[256], char serial_value[256],
            double max_voltage, double min_voltage, double max_current, char filestring[255]):
               PsuInterface::PsuInterface(serial_mode, serial_value, max_voltage, min_voltage, max_current, filestring)
{
    #if DEBUG
     	printf("Start of HP66332 Interface Constructor\n");
    #endif // DEBUG

    printf("Checking Device ID\n");
    sprintf(inst_, "*IDN?");
    device_->Write(inst_);
    device_->Read(val_);
    printf("%s\n", val_);

    if(strncmp("HEWLETT-PACKARD,66332A", val_, 22))
    {
        printf("Device at address GPIB%i::%i::INSTR was not the expected device, exiting...\n", 1, 1);
        exit(1);
    }

    sprintf(inst_, "*RST\n"); // reset
    device_->Write(inst_);

    sprintf(inst_, ":OUTP:PON:STAT RST\n"); // reset on power failure
    if(Write(inst_))   {   printf("Error: Error setting reset state\n");   }

    sprintf(inst_, ":OUTP:STAT OFF\n"); // turn off output (not the same as output on)
    if(Write(inst_))   {   printf("Error: Error output state off\n");   }

    sprintf(inst_, ":OUTP:REL:STAT ON\n"); // connect output relay (not the same as output on)
    if(Write(inst_))   {   printf("Error: Error setting output relay state on\n");   }

    sprintf(inst_, ":SENS:SWE:TINT 15.6E-6\n");
    if(Write(inst_))   {   printf("Error: Error setting sweep time interval\n");   }

    sprintf(inst_, ":SENS:SWE:POIN 2565\n");
    if(Write(inst_))   {   printf("Error: Error setting sweep number of points\n");   }

    // sprintf(inst_, "DISPLAY:MODE TEXT\n");
    // if(Write(inst_))   {   printf("Error: Error changing display mode\n");   }

    // sprintf(inst_, "DISPLAY:TEXT \"TURNING OFF SCREEN\"\n");
    // if(Write(inst_))   {   printf("Error: Error changing display mode\n");   }
    // mwait(200);

    // sprintf(inst_, "DISPLAY:STATE OFF\n");
    // if(Write(inst_))   {   printf("Error: Error changing display mode\n");   }

    double tmp = SetCurrentRange(fabs(max_current));
    if(tmp > 4.0)
    {
        printf("Current set to 5A range\n");
    }
    else if(tmp > 0)
    {
        printf("Current set to 20mA range\n");
    }
    else if(tmp < 0)
    {
        printf("Error setting current range\n");
    }

	time(&t0);
	printf("HP66332Interface (V%.2f): started at %s", HP66332_INTERFACE_VERSION, ctime(&t0));

    #if DEBUG
 	    printf("End of HP66332 Interface Constructor\n");
    #endif // DEBUG

    //ctor
}

HP66332Interface::~HP66332Interface()
{
 	#if DEBUG
        printf("Start of HP66332 Interface Destructor\n");
    #endif // DEBUG

    sprintf(inst_, ":OUTP:REL:STAT OFF\n");
    if(Write(inst_))   {   printf("Error: Error setting output relay off\n");   }

    sprintf(inst_, ":OUTP:STAT OFF\n");
    if(Write(inst_))   {   printf("Error: Error setting output state off\n");   }

    printf("Closing HP66332Interface Device\n");
    #if DEBUG
        printf("End of HP66332 Interface Destructor\n");
    #endif // DEBUG

    //dtor
}

int HP66332Interface::SetOutput(double V, double I)
{
    #if DEBUG
	    printf("Call to HP66332Interface::SetOutput\n");
        printf("V: %f, I:%f\n", V, I);
    #endif // DEBUG

    // no longer set range at each measurement for this device, set once during initialisation
    // SetVoltageRange(fabs(V));
    // SetCurrentRange(fabs(I));
    sprintf(inst_, ":SOUR:VOLT %1.3f\n", V);
    if(Write(inst_))   {   printf("Error: Error setting output voltage \n");    }
    sprintf(inst_, ":SOUR:CURR %1.3f\n", fabs(I));
    if(Write(inst_))   {   printf("\nError: Error setting output current: %1.3f\n", I);    }

    mwait(20);
    return 0.0f;

    // return SMUVoltage(V,I);
}

int HP66332Interface::GetOutput(double *V, double *I)
{
    int err_voltage = 0;
    int err_current = 0;

    #if DEBUG
	    printf("Call to HP66332Interface::GetOutput\n");
    #endif // DEBUG

    sprintf(inst_, ":MEAS:CURR:DC?");
    err_current = Query(inst_, val_);
    if (err_current)   {   printf("Error reading output current\n");    }
    #if DEBUG
        printf("QUERY: %s", val_);
    #endif // DEBUG

    *I = atof(val_);

    sprintf(inst_, ":MEAS:VOLT:DC?");
    err_voltage = Query(inst_, val_);
    if (err_voltage)    {  printf("Error reading output voltage\n");    }
    #if DEBUG
        printf("QUERY: %s", val_);
    #endif // DEBUG
    *V = atof(val_);

    if (*I > 1.1*this->max_current_)
    {
        // #if DEBUG
        printf("\nWarning Faulty Current Detected: %fA\n", *I);
        // #endif // DEBUG
        err_current = 1;
    }

    if(!(err_voltage | err_current))
    {
        this->current_now_m = *I;
    }
    
    return err_voltage | err_current;
}

int HP66332Interface::SMUVoltage(double V, double I)
{
    #if DEBUG
	    printf("Call to HP66332Interface::SMUVoltage\n");
        printf("V: %f, I: %f\n", V, I);
    #endif // DEBUG

    return SetOutput(V,I);

}

int HP66332Interface::SMUCurrent(double voltage_max, double voltage_min, double current_t)
{
    #if DEBUG
	    printf("Call to HP66332Interface::SMUCurrent\n");
        printf("V: %f, I: %f\n", voltage_max, current_t);
    #endif // DEBUG

    if (current_t >= 0)
    {
        return SetOutput(voltage_max,current_t);
    }
    else
    {
        return SetOutput(voltage_min,current_t);
    }
}

double HP66332Interface::SetVoltageRange(double V)
{
    #if DEBUG
	    printf("Call to HP66332Interface::SetVoltageRange\n");
    #endif // DEBUG

    return 0.0f;
}

double HP66332Interface::SetCurrentRange(double I)
{
    #if DEBUG
	    printf("Call to HP66332Interface::SetCurrentRange\n");
    #endif // DEBUG

    if( fabs(I) >= 0.02) // maxiumum range
    {
        sprintf(inst_, ":SENS:CURR:RANG %1.3f\n", I);
        if(Write(inst_))   { printf("Error: Error setting current range to MAX"); }
        return 5.0f;
    }
    else
    {
        // sprintf(inst_, ":SENS:CURR:RANG 0.019\n");
        sprintf(inst_, ":SENS:CURR:RANG %1.3f\n", I);
        if(Write(inst_))   { printf("Error: Error setting current range to MIN"); }
        return 0.02f;
    }
    return -1.0f;

}

int HP66332Interface::IsCurrentLimited(void)
{
    #if DEBUG
        printf("\nFUNCTION: E5270Interface::IsCurrentLimited\n");
    #endif // DEBUG

    uint32_t status = 0;

    sprintf(inst_, ":STAT:OPER:COND?");
    if(Query(inst_, val_))    {   printf("ERROR: Error reading current trip limit\n"); }

    status = atoi(val_);
    #if DEBUG
        printf("Operaton Status Condition Register: \n");
        printf("0x%04x %i\n", status, status);
    #endif // DEBUG

    bool enumfault = false;
    if (fabs(this->current_now_m) < 0.98*fabs(this->max_current_))
    {
        enumfault = true;
    }   

    return ((status & 0x0800) || (status & 0x0400) || !enumfault);

}

int HP66332Interface::OutputOn()
{
    sprintf(inst_, ":OUTP:STAT ON\n"); // turn on output
    return Write(inst_);
}

int HP66332Interface::OutputOff()
{
    sprintf(inst_, ":OUTP:STAT OFF\n"); // turn off output
    return Write(inst_);
}

int HP66332Interface::ClearErrors()
{
    char buff[256];
    int error = 0;

    do // clear last errors if any remain
    {
        //visa::wbstr(device, "SYST:ERR?");
        //visa::rbstr(device, buff, 255);
        sprintf(buff, ":SYST:ERR?");
        device_->Write(buff);
        device_->Read(buff);
    } while (atoi(buff) != 0);

    return 0;
}

int HP66332Interface::CheckErrors()
{
    char buff[256];
    int error = 0;
    int error_last = 0;
    do
    {
        sprintf(buff, ":SYST:ERR?");
        device_->Write(buff);
        device_->Read(buff);
        error = atoi(buff);
        if (error)
        {
            if(error_last == 0)
            {
                printf("\n");
            }
            printf("%s\n", buff);
            error_last = error;
        }
    } while(error != 0);

    return error_last;
}
