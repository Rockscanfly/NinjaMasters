#include "HP66332Interface.hpp"

HP66332Interface::HP66332Interface(int id,
                       int addr,
                       int channel,
                       double Vmax,
                       double Vmin,
                       double Imax,
                       const char filestring[255]):
                       PSUInterface::PSUInterface(
                                       id,
                                       addr,
                                       channel,
                                       Vmax,
                                       Vmin,
                                       Imax,
                                       filestring)
{
    #if DEBUG
     	printf("Start of HP66332 Interface Constructor\n");
    #endif // DEBUG

    sprintf(m_inst, "*IDN?\n");
    Query(m_inst, m_val);
    printf(m_val);
    printf("\n");

    if(strncmp("HEWLETT-PACKARD,66332A", m_val, 22))
    {
        printf("Device at address GPIB%i::%i::INSTR was not the expected device, exiting...\n", id, addr);
        exit(1);
    }

    sprintf(m_inst, "*RST\n"); // reset
    if(Write(m_inst))   {   printf("Error: Error during initial reset\n");    }

    sprintf(m_inst, "OUTP:PON:STAT RST\n"); // reset on power failure
    if(Write(m_inst))   {   printf("Error: Error setting reset state\n");   }

    sprintf(m_inst, "OUTP:STAT OFF\n"); // turn off output (not the same as output on)
    if(Write(m_inst))   {   printf("Error: Error output state off\n");   }

    sprintf(m_inst, "OUTP:REL:STAT ON\n"); // connect output relay (not the same as output on)
    if(Write(m_inst))   {   printf("Error: Error setting output relay state on\n");   }

	time(&t0);
	printf("HP66332Interface (V%.2f): VISA address %s, started at %s", 1.0, busname, ctime(&t0));

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

    sprintf(m_inst, "OUTP:REL:STAT OFF\n");
    if(Write(m_inst))   {   printf("Error: Error setting output relay off\n");   }

    sprintf(m_inst, "OUTP:STAT OFF\n"); // connect output relay (not the same as output on)
    if(Write(m_inst))   {   printf("Error: Error setting output state off\n");   }

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

    SetVoltageRange(fabs(V));
    SetCurrentRange(fabs(I));
    sprintf(m_inst, "SOUR:VOLT %1.3f\n", V);
    if(Write(m_inst))   {   printf("Error: Error setting output voltage \n");    }
    sprintf(m_inst, "SOUR:CURR %1.3f\n", fabs(I));
    if(Write(m_inst))   {   printf("\nError: Error setting output current: %1.3f\n", I);    }

    visa::mwait(200);
    return 0.0f;

    // return SMUVoltage(V,I);
}

int HP66332Interface::GetOutput(double *V, double *I)
{
    int err = 0;
    #if DEBUG
	    printf("Call to HP66332Interface::GetOutput\n");
    #endif // DEBUG

    sprintf(m_inst, "MEAS:CURR:DC?");
    err = Query(m_inst, m_val);
    if (err)   {   printf("Error reading output current\n");    }
    #if DEBUG
        printf("QUERY: %s", m_val);
    #endif // DEBUG

    *I = atof(m_val);

    sprintf(m_inst, "MEAS:VOLT:DC?");
    err = Query(m_inst, m_val);
    if (err)    {  printf("Error reading output voltage\n");    }
    #if DEBUG
        printf("QUERY: %s", m_val);
    #endif // DEBUG
    *V = atof(m_val);

    if (*I > 1.1*this->iMax)
    {
        #if DEBUG
        printf("\nWarning Faulty Current Detected: %fA\n", *I);
        #endif // DEBUG
        err = 1;
    }else    {
        // WriteData(*V, *I);
    }
    return err;
}

int HP66332Interface::SMUVoltage(double V, double I)
{
    #if DEBUG
	    printf("Call to HP66332Interface::SMUVoltage\n");
        printf("V: %f, I: %f\n", V, I);
    #endif // DEBUG

    return SetOutput(V,I);

}

int HP66332Interface::SMUCurrent(double t_voltage_max, double t_voltage_min, double t_current)
{
    #if DEBUG
	    printf("Call to HP66332Interface::SMUCurrent\n");
        printf("V: %f, I: %f\n", t_voltage_max, t_current);
    #endif // DEBUG

    if (t_current >= 0)
    {
        return SetOutput(t_voltage_max,t_current);
    }
    else
    {
        return SetOutput(t_voltage_min,t_current);
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

    sprintf(m_inst, "SENS:CURR:RANG %.6f", I);
    if(Write(m_inst))   { printf("Error: Error setting current range"); }

    return 0.0f;
}

int HP66332Interface::IsCurrentLimited(void)
{
    #if DEBUG
        printf("\nFUNCTION: E5270Interface::IsCurrentLimited\n");
    #endif // DEBUG

    uint32_t status = 0;

    sprintf(m_inst, "STAT:OPER:COND?");
    if(Query(m_inst, m_val))    {   printf("ERROR: Error reading current trip limit\n"); }

    status = atoi(m_val);
    #if DEBUG
        printf("Operaton Status Condition Register: \n");
        printf("0x%04x %i\n", status, status);
    #endif // DEBUG

    return ((status & 0x0800) || (status & 0x0400));

}

int HP66332Interface::OutputOn()
{
    sprintf(m_inst, "OUTP:STAT ON\n"); // turn off output (not the same as output on)
    return Write(m_inst);
}

int HP66332Interface::OutputOff()
{
    sprintf(m_inst, "OUTP:STAT OFF\n"); // turn off output (not the same as output on)
    return Write(m_inst);
}

int HP66332Interface::ClearErrors()
{
    char buff[256];
    int error = 0;

    do // clear last errors if any remain
    {
        visa::wbstr(device, "SYST:ERR?");
        visa::rbstr(device, buff, 255);
    } while (atoi(buff) != 0);

    return 0;
}

int HP66332Interface::CheckErrors()
{
    char buff[256];
    int error = 0;

    visa::wbstr(device, "SYST:ERR?");
    visa::rbstr(device, buff, 255);

    error = atoi(buff);
    if (error)
    {
        printf("\n");
        printf(buff);
    }
    return error;
}