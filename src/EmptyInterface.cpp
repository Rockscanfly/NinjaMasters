#include "EmptyInterface.hpp"

EMPTYInterface::EMPTYInterface(int id,
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
     	printf("Start of EMPTYInterface Constructor\n");
    #endif // DEBUG

    sprintf(m_inst, "*IDN?\n");
    Query(m_inst, m_val);
    printf(m_val);
    printf("\n");

    if(strncmp("Device IDN string",m_val, 29))
    {
        printf("Device at address GPIB%i::%i::INSTR was not the expected device, exiting...\n", id, addr);
        exit(1);
    }

    sprintf(m_inst, "*RST\n");
    Write(m_inst);

    // sprintf(m_inst, "CLR");
    // Write(m_inst);

    sprintf(m_inst, "UNT?\n");
    Query(m_inst, m_val);
    #if DEBUG
        printf(m_val);
        printf("\n");
    #endif // DEBUG

	time(&t0);
	printf("EMPTYInterface (V%.2f): VISA address %s, started at %s", 1.0, busname, ctime(&t0));

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

    sprintf(m_inst, "OUTP:REL:STAT OFF\n");
    if(Write(m_inst))   {   printf("Error: Error setting output relay off\n");   }

    sprintf(m_inst, "OUTP:STAT OFF\n"); // connect output relay (not the same as output on)
    if(Write(m_inst))   {   printf("Error: Error setting output state off\n");   }

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
    sprintf(m_inst, "SOUR:VOLT %1.3f\n", V);
    if(Write(m_inst))   {   printf("Error: Error setting output voltage \n");    }
    sprintf(m_inst, "SOUR:CURR %1.3f\n", fabs(I));
    if(Write(m_inst))   {   printf("\nError: Error setting output current: %1.3f\n", I);    }

    visa::mwait(2);
    return 0.0f;
}

int EMPTYInterface::SMUCurrent(double t_voltage_max, double t_voltage_min, double t_current)
{
    #if DEBUG
	    printf("Call to EMPTYInterface::SMUCurrent\n");
        printf("V: %f-%f, I: %f\n", t_voltage_max, t_voltage_min, t_current);
    #endif // DEBUG

    // if(fabs(I) < 0.001)
    // {
    //     I = 0;
    // }
    SetVoltageRange(fabs(t_voltage_max));
    SetCurrentRange(fabs(t_current));
    sprintf(m_inst, "SOUR:VOLT %1.3f\n", t_voltage_max);
    if(Write(m_inst))   {   printf("Error: Error setting output voltage \n");    }
    sprintf(m_inst, "SOUR:CURR %1.3f\n", fabs(t_current));
    if(Write(m_inst))   {   printf("\nError: Error setting output current: %1.3f\n", t_current);    }

    sprintf(m_inst, "*WAI\n");
    if(Write(m_inst))   {   printf("\nError: Error waiting for command completion\n");    }

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

    sprintf(m_inst, "SENS:CURR:RANG %.6f", I);
    if(Write(m_inst))   { printf("Error: Error setting current range"); }

    return 0.0f;
}

int EMPTYInterface::IsCurrentLimited(void)
{
    #if DEBUG
        printf("\nFUNCTION: EMPTYInterface::IsCurrentLimited\n");
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

int EMPTYInterface::OutputOn()
{
    sprintf(m_inst, "OUTP:STAT ON\n"); // turn off output (not the same as output on)
    return Write(m_inst);
}

int EMPTYInterface::OutputOff()
{
    sprintf(m_inst, "OUTP:STAT OFF\n"); // turn off output (not the same as output on)
    return Write(m_inst);
}

int EMPTYInterface::ClearErrors()
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

int EMPTYInterface::CheckErrors()
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