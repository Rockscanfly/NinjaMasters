#include "E5270Interface.hpp"

E5270Interface::E5270Interface(
                       const char filestring[255]):
                       PsuInterface::PsuInterface(
                                       filestring)
{
    #if DEBUG
     	printf("Start of E5270 Interface Constructor\n");
    #endif // DEBUG

    sprintf(m_inst, "*IDN?\n");
    Query(m_inst, m_val);
    printf("%s", m_val);
    printf("\n");

    if(strncmp("Agilent Technologies,E5270B", m_val, 27))
    {
        printf("Device at address GPIB%i::%i::INSTR was not the expected device, exiting...\n", 1, 1);
        exit(1);
    }

    sprintf(m_inst, "*RST\n");
    Write(m_inst);

    mwait(2000);
    // sprintf(m_inst, "CLR");
    // Write(m_inst);

    sprintf(m_inst, "UNT?\n");
    Query(m_inst, m_val);
    #if DEBUG
    printf("%s", m_val);
    printf("\n");
    #endif // DEBUG

	time(&t0);
	printf("E5270Interface (V%.2f): started at %s", E5270_INTERFACE_VERSION, ctime(&t0));

    // //visa::wbstr(device, "SU1:02.22"); // engage remote control mode

    sprintf(m_inst, "CN %i\n", this->channel);
    Write(m_inst);

    sprintf(m_inst, "CN %i\n", E5270_INTERFACE_CHANNEL_GND);
    Write(m_inst);

    sprintf(m_inst, "RED 1\n");
    Write(m_inst);

    sprintf(m_inst, "SCH %i\n", this->channel);
    Write(m_inst);

    sprintf(m_inst, "MCH %i\n", this->channel);
    Write(m_inst);

    // sprintf(m_inst, "AV 256, 1\n");
    // Write(m_inst);

    sprintf(m_inst, "FMT  21\n");
    Write(m_inst);

    mwait(1000);


    #if DEBUG
 	    printf("End of E5270 Interface Constructor\n");
    #endif // DEBUG

    //ctor
}

E5270Interface::~E5270Interface()
{
 	#if DEBUG
        printf("Start of E5270Interface Interface Destructor\n");
    #endif // DEBUG

    sprintf(m_inst, "CL %i\n", this->channel);
    Write(m_inst);
    sprintf(m_inst, "CL %i\n", E5270_INTERFACE_CHANNEL_GND);
    Write(m_inst);

    printf("Closing E5270Interface Device\n");
    #if DEBUG
        printf("End of E5270 Interface Destructor\n");
    #endif // DEBUG

    //dtor
}

int E5270Interface::SetOutput(double V, double I)
{
    #if DEBUG
        printf("Call to E5270Interface::SetOutput\n");
        printf(">> %f, %f\n", V, I);
    #endif // DEBUG

    return SMUVoltage(V, I);
}

int E5270Interface::GetOutput(double *V, double *I)
{
    #if DEBUG
        printf("Call to E5270Interface::GetOutput\n");
    #endif // DEBUG

    int err = 0;
    double V_TMP = 0;
    double I_TMP = 0;
    double V_GND = 0;
    double I_GND = 0;

    int data_return = 0;
    // read once and throw away
    sprintf(m_inst, "TV %i, 0\n", E5270_INTERFACE_CHANNEL_GND);
    Query(m_inst, m_val);

    sprintf(m_inst, "TV %i, 0\n", E5270_INTERFACE_CHANNEL_GND);
    Query(m_inst, m_val);
    data_return = DataDecode(m_val, &V_TMP) ;
    if (!(data_return == 0 || data_return == 4 || data_return == 5))
    {
        printf("\nBad Data %i\n", data_return);
        err = 1;
    }
    else
    {
        V_GND = V_TMP;
    }
    #if DEBUG
        printf("GND V: ");
        printf("%f\n", V_GND);
        printf("\n");
    #endif // DEBUG

    // read once and throw away
    sprintf(m_inst, "TV %i, 0\n", this->channel);
    Query(m_inst, m_val);

    sprintf(m_inst, "TV %i, 0\n", this->channel);
    Query(m_inst, m_val);
    data_return = DataDecode(m_val, &V_TMP) ;
    if (!(data_return == 0 || data_return == 4 || data_return == 5))
    {
        printf("\nBad Data %i\n", data_return);
        err = 1;
    }
    else
    {
        *V = V_TMP;
    }
    *V = *V-V_GND;
    #if DEBUG
        printf("Channel V: ");
        printf("%f\n", *I);
        printf("\n");
        printf("Load V: %e\n", *V);
    #endif // DEBUG

    // read once and throw away
    sprintf(m_inst, "TI %i, 0\n", E5270_INTERFACE_CHANNEL_GND);
    Query(m_inst, m_val);

    sprintf(m_inst, "TI %i, 0\n", E5270_INTERFACE_CHANNEL_GND);
    Query(m_inst, m_val);
    data_return = DataDecode(m_val, &I_TMP) ;
    if (!(data_return == 0 || data_return == 4 || data_return == 5))
    {
        printf("\nBad Data %i\n", data_return);
        err = 1;
    }
    else
    {
        I_GND = I_TMP;
    }
    #if DEBUG
        printf("GND I: ");
        printf("%f\n", I_GND);
        printf("\n");
    #endif // DEBUG

    // read once and throw away
    sprintf(m_inst, "TI %i, 0\n", this->channel);
    Query(m_inst, m_val);

    sprintf(m_inst, "TI %i, 0\n", this->channel);
    Query(m_inst, m_val);
    data_return = DataDecode(m_val, &I_TMP) ;
    if (!(data_return == 0 || data_return == 4 || data_return == 5))
    {
        printf("\nBad Data %i\n", data_return);
        err = 1;
    }
    else
    {
        *I = I_TMP;
    }
    #if DEBUG
        printf("Channel I: ");
        printf("%f\n", *I);
        printf("\n");
    #endif // DEBUG

    if(fabs(*I+I_GND) > fabs(0.05*I_GND) && fabs(0.05*I_GND) > 1e-5)
    {
        fprintf(stderr, "\nCurrent leakage detected\n");
        #ifndef DEBUG
            printf("I variation: %e\n", *I+I_GND);
        #endif
    }
    #if DEBUG
        printf("I variation: %e\n", *I-I_GND);
    #endif // DEBUG
    return err;
}

int E5270Interface::SMUVoltage(double V, double I)
{
    #if DEBUG
        printf("\nFUNCTION: E5270Interface::SMUVoltage\n");
        printf(">> %f, %f\n", V, I);
    #endif // DEBUG
    // DV chnum,vrange,voltage[,Icomp[,comp_polarity[,irange]]
    sprintf(m_inst, "DV %i, 0, %.4f, %.4f \n", E5270_INTERFACE_CHANNEL_GND, 0.0, -I);
    Write(m_inst);
    sprintf(m_inst, "DV %i, 0, %.4f, %.4f \n", this->channel, V, I);
    Write(m_inst);

    return 0;
}


int E5270Interface::SMUCurrent(double voltage_max, double voltage_min, double current_t)
{
    #if DEBUG
        printf("\nFUNCTION: E5270Interface::SMUCurrent\n");
        printf("V: %f-%f, I: %f\n", voltage_max, voltage_min, current_t);
    #endif // DEBUG
    // DV chnum,vrange,voltage[,Icomp[,comp_polarity[,irange]]
    sprintf(m_inst, "DV %i, 0, %.2e, %.2e, 1 \n", E5270_INTERFACE_CHANNEL_GND, 0.0, copysign(this->iMax, current_t));
    Write(m_inst);
    // DI chnum,irange,current_t[,Vcomp[,comp_polarity[,vrange]]]
    if(current_t >=0)
    {
        sprintf(m_inst, "DI %i, 0, %.4e, %.4e, 1 \n", this->channel, current_t, voltage_max);
    }else{
        sprintf(m_inst, "DI %i, 0, %.4e, %.4e, 1 \n", this->channel, current_t, voltage_min);
    }

    Write(m_inst);

    return 0;
}

double E5270Interface::SetVoltageRange(double V)
{
    return 0;
}

double E5270Interface::SetCurrentRange(double I)
{
    return 0;
}

int E5270Interface::IsCurrentLimited(void)
{
    return 0;
}

int E5270Interface::OutputOn(void)
{
    sprintf(m_inst, "CN %i\n", this->channel);
    Write(m_inst);
    sprintf(m_inst, "CN %i\n", E5270_INTERFACE_CHANNEL_GND);
    Write(m_inst);
    return 0;
}

int E5270Interface::OutputOff(void)
{
    sprintf(m_inst, "CL %i\n", this->channel);
    Write(m_inst);
    sprintf(m_inst, "CL %i\n", E5270_INTERFACE_CHANNEL_GND);
    Write(m_inst);

    return 0;

}

int E5270Interface::ClearErrors()
{
    int error = 0;
    char buff[256];

    sprintf(buff, "ERR?\n");
    //visa::wbstr(device, buff);
    //visa::rbstr(device, buff, 255);

    return 0;
}

int E5270Interface::CheckErrors()
{
    int error = 0;
    char buff[256];

    sprintf(buff, "ERR?\n");
    //visa::wbstr(device, buff);
    //visa::rbstr(device, buff, 255);

    error = strncmp(buff, "0,0,0,0", 7);
    return error;
}


/*
*   Return Value
*   0 No Status
*   1 Invalid Data
*   2 A/D overflowed
*   3 1 or more units oscilating
*   4 Another unit compliance
*   5 Self unit compliance
*   6 Target not in range
*   7 Search stopped automatically
*   8 Invalid Data Returned
*/
int E5270Interface::DataDecode(const char data[256] , double *return)
{
// Formatting Rules
// A: Status. One character.
// B: Channel number. One character.
// C: Data type. One character.
// D: Data. Twelve digits or 13 digits.
// E: Status. Three digits.
// F: Channel number. One character.
// G: Data type. One character.
//
// Format Selected (21)
// EEEFGDDDDDDDDDDDDD
    *return = atof(&data[5]);
    #if DEBUG
        printf("return %e\n", *return);
    #endif // DEBUG
    int rtype = 0;


    if((int)data[4] == 90 || (int)data[4] == 122) // Invalid Data Returned, Z or z
    {
        #if DEBUG
            printf("Invalid Data, type Z\n");
        #endif // DEBUG
        if(!rtype)
            rtype = 1;
    }
    uint8_t status = (((int)data[0]) -'0')*100 + (((int)data[1]) -'0')*10 + ((int)data[2]) -'0';
    #if DEBUG
        printf("Status: %i\n", status);
    #endif // DEBUG

    if (status & 0x01) // A/D overflow
    {
        #if DEBUG
            printf("A/D overflow\n");
        #endif // DEBUG
        if(!rtype)
            rtype = 2;
    }
    if (status & 0x02) // Oscilating
    {
        #if DEBUG
            printf("Units Oscilating\n");
        #endif // DEBUG
        if(!rtype)
            rtype = 3;
    }
    if (status & 0x10) // Target not in range
    {
        #if DEBUG
            printf("Target not in range\n");
        #endif // DEBUG
        if(!rtype)
            rtype = 6;
    }
    if (status & 0x20) // Search stopped automatically
    {
        #if DEBUG
            printf("Search stopped automatically\n");
        #endif // DEBUG
        if(!rtype)
            rtype = 7;
    }
    if (status & 0x40) // Invalid Data Returned
    {
        #if DEBUG
            printf("Invalid Data Returned\n");
        #endif // DEBUG
        if(!rtype)
            rtype = 8;
    }
    if (status & 0x08) // Self unit compliance
    {
        #if DEBUG
            printf("Self unit compliance\n");
        #endif // DEBUG
        if(!rtype)
            rtype = 5;
    }
    if (status & 0x04) // Another unit compliance
    {
        #if DEBUG
            printf("Another unit compliance\n");
        #endif // DEBUG
        if(!rtype)
            rtype = 4;
    }

    return rtype;
}
