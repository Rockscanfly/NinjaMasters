#include "HamegInterface.hpp"

HamegInterface::HamegInterface(
                       const char filestring[255]):
                       PsuInterface::PsuInterface(
                                       filestring)
{
    #if DEBUG
     	printf("Start of Hameg Interface Constructor\n");
    #endif // DEBUG

    sprintf(m_inst, "*IDN?\n");
    Query(m_inst, m_val);
    printf("%s", m_val);
    printf("\n");

    if(strncmp("HAMEG Instruments,HM8143",m_val, 24))
    {
        printf("Device at address GPIB%i::%i::INSTR was not the expected device, exiting...\n", 1, 1);
        exit(1);
    }

    // clear old state from the machine
    sprintf(m_inst, "RM1");
    Write(m_inst);

    sprintf(m_inst, "CLR");
    Write(m_inst);

    sprintf(m_inst, "STA");
    Query(m_inst, m_val);
    #if DEBUG
    printf("%s", m_val);
    printf("\n");
    #endif // DEBUG

	time(&t0);
	printf("HamegInterface (V%.2f): started at %s", HAMEG_INTERFACE_VERSION, ctime(&t0));

    #if DEBUG
 	    printf("End of Hameg Interface Constructor\n");
    #endif // DEBUG

    //ctor
}

HamegInterface::~HamegInterface()
{
 	#if DEBUG
        printf("Start of HamegInterface Interface Destructor\n");
    #endif // DEBUG

    // turn off output and disengage remote mode
    sprintf(m_inst, "OP0");
    Write(m_inst);

    sprintf(m_inst, "RM0");
    Write(m_inst);

    printf("Closing HamegInterface Device\n");
    #if DEBUG
        printf("End of Hameg Interface Destructor\n");
    #endif // DEBUG

    //dtor
}

int HamegInterface::SetOutput(double V, double I)
{
    #if DEBUG
        printf("Call to HamegInterface::SetOutput\n");
        printf(">> %f, %f\n", V, I);
    #endif // DEBUG
    sprintf(m_inst, "SU%i:%2.2f\n", this->channel, V);
    if(Write(m_inst))   {   printf("Error: Error setting output voltage \n");    }
    sprintf(m_inst, "SI%i:%1.3f\n", this->channel, fabs(I));
    if(Write(m_inst))   {   printf("\nError: Error setting output current_t: %1.3f\n", I);    }

    mwait(2);
    return 0;
}

int HamegInterface::GetOutput(double *V, double *I)
{
    #if DEBUG
        printf("Call to HamegInterface::GetOutput\n");
    #endif // DEBUG

    mwait(2);

    sprintf(m_inst, "MU%i\n", this->channel);
    if(Query(m_inst, m_val))   {   printf("Error: Error getting output voltage \n");    }
    *V = atof(&m_val[3]);

    sprintf(m_inst, "MI%i\n", this->channel);
    if(Query(m_inst, m_val))   {   printf("Error: Error getting output current_t \n");    }
    *I = atof(&m_val[3]);

    return 0;
}


int HamegInterface::SMUVoltage(double V, double I)
{
    #if DEBUG
	    printf("Call to HamegInterface::SMUVoltage\n");
        printf("V: %f, I: %f\n", V, I);
    #endif // DEBUG

    SetOutput(V, I);

    mwait(20);
    return 0;
}

int HamegInterface::SMUCurrent(double voltage_max, double voltage_min, double current_t)
{
    #if DEBUG
	    printf("Call to HamegInterface::SMUCurrent\n");
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

double HamegInterface::SetVoltageRange(double V)
{
    #if DEBUG
	    printf("Call to HamegInterface::SetVoltageRange\n");
    #endif // DEBUG

    return 0.0f;
}

double HamegInterface::SetCurrentRange(double I)
{
    #if DEBUG
	    printf("Call to HamegInterface::SetCurrentRange\n");
    #endif // DEBUG

    return 0.0f;
}

int HamegInterface::IsCurrentLimited(void)
{
    #if DEBUG
        printf("\nFUNCTION: HamegInterface::IsCurrentLimited\n");
    #endif // DEBUG

    uint32_t status = 0;
    sprintf(m_inst, "STA?");
    if(Query(m_inst, m_val))    {   printf("ERROR: Error reading current_t trip limit\n"); }
    #if DEBUG
        printf("%s\n", m_val);
    #endif // DEBUG

    if(this->channel == 1)
    {
        #if DEBUG
            printf("%i\n", (strncmp(&m_val[4], "CC1", 3) == 0));
        #endif // DEBUG
        return (strncmp(&m_val[4], "CC1", 3) == 0);
    }
    else if(this->channel == 2)
    {
        #if DEBUG
            printf("%i\n", (strncmp(&m_val[8], "CC2", 3) == 0));
        #endif // DEBUG
        return (strncmp(&m_val[8], "CC2", 3) == 0);
    }
    else
    {
        printf("Error: Invalid channel selected");
        exit(1);
    }
}

int HamegInterface::OutputOn()
{
    int err = 0;
    sprintf(m_inst, "OP1\n"); // turn off output (not the same as output on)
    err = Write(m_inst);
    mwait(1000);
    return err;
}

int HamegInterface::OutputOff()
{
    sprintf(m_inst, "OP0\n"); // turn off output (not the same as output on)
    return Write(m_inst);
}

int HamegInterface::ClearErrors()
{
    return 0;
}

int HamegInterface::CheckErrors()
{
    return 0;
}
