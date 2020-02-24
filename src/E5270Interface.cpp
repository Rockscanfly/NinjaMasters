#include "E5270Interface.hpp"

E5270Interface::E5270Interface(int id,
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
     	printf("Start of E5270 Interface Constructor\n");
    #endif // DEBUG

    sprintf(m_inst, "*IDN?\n");
    Query(m_inst, m_val);
    printf(m_val);
    printf("\n");

    if(strncmp("Agilent Technologies,E5270B,0,B.01.08", m_val, 29))
    {
        printf("Device at address GPIB%i::%i::INSTR was not the expected device, exiting...\n", id, addr);
        exit(1);
    }

    sprintf(m_inst, "*RST\n");
    Write(m_inst);

    visa::mwait(2000);
    // sprintf(m_inst, "CLR");
    // Write(m_inst);

    sprintf(m_inst, "UNT?\n");
    Query(m_inst, m_val);
    #if DEBUG 
        printf(m_val);
        printf("\n");
    #endif // DEBUG

	time(&t0);
	printf("E5270Interface (V%.2f): VISA address %s, channel %d started at %s", 1.0, busname, this->channel, ctime(&t0));

    // visa::wbstr(device, "SU1:02.22"); // engage remote control mode
	
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

    visa::mwait(1000);
    

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
    
    // sprintf(m_inst, "*OPC?");
    // Query(m_inst, m_val);

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
    
    if(fabs(*I+I_GND) > fabs(0.05*I_GND) && fabs(0.05*I_GND) > 1e-5 )
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


int E5270Interface::SMUCurrent(double t_voltage_max, double t_voltage_min, double t_current)
{
    #if DEBUG
        printf("\nFUNCTION: E5270Interface::SMUCurrent\n");
        printf("V: %f-%f, I: %f\n", t_voltage_max, t_voltage_min, t_current);
    #endif // DEBUG
    // DV chnum,vrange,voltage[,Icomp[,comp_polarity[,irange]]
    sprintf(m_inst, "DV %i, 0, %.2e, %.2e, 1 \n", E5270_INTERFACE_CHANNEL_GND, 0.0, copysign(this->iMax, t_current));
    Write(m_inst);
    // DI chnum,irange,current[,Vcomp[,comp_polarity[,vrange]]]
    if(t_current >=0)
    {   
        sprintf(m_inst, "DI %i, 0, %.4e, %.4e, 1 \n", this->channel, t_current, t_voltage_max);
    }else{
        sprintf(m_inst, "DI %i, 0, %.4e, %.4e, 1 \n", this->channel, t_current, t_voltage_min);
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
    visa::wbstr(device, buff);
    visa::rbstr(device, buff, 255);

    return 0;
}

int E5270Interface::CheckErrors()
{
    int error = 0;
    char buff[256];

    sprintf(buff, "ERR?\n");
    visa::wbstr(device, buff);
    visa::rbstr(device, buff, 255);

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
int E5270Interface::DataDecode(const char t_data[256] , double *t_return)
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
    *t_return = atof(&t_data[5]);
    #if DEBUG
        printf("t_return %e\n", *t_return);
    #endif // DEBUG
    int rtype = 0;


    if((int)t_data[4] == 90 || (int)t_data[4] == 122) // Invalid Data Returned, Z or z
    {
        #if DEBUG
            printf("Invalid Data, type Z\n");
        #endif // DEBUG
        if(!rtype)
            rtype = 1;
    }
    uint8_t status = (((int)t_data[0]) -'0')*100 + (((int)t_data[1]) -'0')*10 + ((int)t_data[2]) -'0';
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
    // if (status & 0x80)
    //     return 9;

    return rtype;
}

// double E5270Interface::CycleBattery(int t_number_cycles, double t_voltage_max, double t_voltage_min,
//                         double t_current_max, double t_current_end, double t_charge_end, double t_timeout, double t_relax_time)
// {
 
//     #if DEBUG
//         printf("Call to E5270Interface::CycleBattery\n");
//         printf(">> %d, %f, %f, %f\n", t_number_cycles, t_voltage_max, t_voltage_min, t_current_max);
//     #endif // DEBUG


//     return 0.0f;
// }

// double E5270Interface::Waveform(double t_voltage_max, double t_voltage_min, double t_current_max, double t_charge_max, int t_number_cycles, double t_frequency[16])
// {
//     #if DEBUG
//         printf("Call to E5270Interface::Waveform\n");
//         printf("Vmax: %f, Vmin: %f, I: %f, Q: %f, F: %e\n", t_voltage_max, t_voltage_min, t_current_max, t_charge_max, t_frequency[0]);
//     #endif // DEBUG
//     printf("Vmax: %f, Vmin: %f, I: %f, Q: %f, F: %e\n", t_voltage_max, t_voltage_min, t_current_max, t_charge_max, t_frequency[0]);

//     clock_t clock_function_start = clock();
//     clock_t clock_now = 0;
//     double time_now = 0;
//     double time_start = 0;
//     double time_end = 0;

//     double voltage_now = 0;
//     double current_now = 0;
//     // int cycle_count = 0;
//     int vlimcount = 0;
//     int err = 0;
    
//     double max_dq = 0;
//     double max_fq = 0;
//     double fmin = t_frequency[0];
//     double fmax = t_frequency[0];
//     double frequency_max_current[16] = {0};

//     if(t_frequency[0] == 0.0f)
//     {
//         printf("Error invalid frequency 0Hz given as first frequency\n");
//         return 1.0;
//     }


//     int nfrequencies = 1;   
//     for(int i = 1; i < 16; i++)
//     {
//         if(t_frequency[i]!= 0)
//         {
//             if(t_frequency[i]<fmin)
//             {
//                 fmin = t_frequency[i];
//             }
//             if(t_frequency[i]>fmax)
//             {
//                 fmax = t_frequency[i];
//             }
//             nfrequencies++;
//         }
//             else
//         {
//             break;
//         }    
//     }
//     #if DEBUG
//         printf("Lowest Frequency: %f\n", fmin);
//     #endif // DEBUG

//     // what q will be moved by the slowest frequency in Ah
//     max_dq = t_current_max/(M_PI*fmin*3600);
//     if (fabs(t_charge_max) < fabs(max_dq))
//     {
//         max_dq = t_charge_max;
//     }
//     #if DEBUG
//         printf("I qmax: %g\n", t_current_max/(M_PI*fmin*3600));
//         printf("Given qmax: %g\n", t_charge_max);
//         printf("dQ: %f\n", max_dq);
//     #endif // DEBUG

//     printf("\n");
//     for(int i = 0; i < nfrequencies; i++)
//     {
//         // divide amplitude of each sine wave to produce equal charge movement for each
//         // dQ_f = -2*nfreq*I_f/(2*pi*f)
//         // dI_f = Q_f*(2*pi*f)/2*nfreq
//         // dI_f = Q_f*(pi*f)/nfreq
//         frequency_max_current[i] = max_dq * M_PI * t_frequency[i]*3600/(nfrequencies);
//         if( fabs(frequency_max_current[i]) > fabs(t_current_max/nfrequencies))
//         {
//             frequency_max_current[i] = t_current_max/nfrequencies;
//         }
//         // #if DEBUG
//             printf("Maximum current for frequency %e Hz: %e A\n", t_frequency[i], frequency_max_current[i]);
//         // #endif // DEBUG
//     }
    
//     time_end = t_number_cycles/fmin;

//     SMUCurrent(t_voltage_min, 1e-9);
//     err = GetOutput(&voltage_now, &current_now);
//     WriteData(voltage_now, current_now);

//     clock_function_start = clock();
//     time_start = time_now = (double)(clock_function_start)/CLOCKS_PER_SEC;
//     time_end += time_now;

//     // cycle_count = 0;
//     while(time_now < time_end)
//     {
//         current_now = 0;
//         clock_t clock_now = clock();

//         // cycle_count = 0;
//         time_now = (double)(clock_now)/CLOCKS_PER_SEC;
//         for(int i = 0; i < nfrequencies; i++)
//         {
//             current_now += frequency_max_current[i]*sin(2*M_PI*t_frequency[i]*(time_now - time_start));
//             #if DEBUG
//                 printf("F: %e, T: %e, C: %e\n",t_frequency[i], time_now - time_start, frequency_max_current[i]*sin(2*M_PI*t_frequency[i]*(time_now - time_start)));
//             #endif // DEBUG 
//         }

//         if(current_now > 0)
//         {
//             SMUCurrent(t_voltage_max, current_now);
//         }
//         else
//         {
//             SMUCurrent(t_voltage_min, current_now);
//         }
//         visa::mwait(200);
//         err = GetOutput(&voltage_now, &current_now);
//         if((!err) && (time_now - time_start > 1.0))
//         {
//             WriteData(voltage_now, current_now);
//         }
//         #ifndef DEBUG
//         printf("Voltage: %2.4fV, Current: %2.4eA, Cycle Number: %i, time remaining: %5.2fs\r", voltage_now, current_now, 1, time_end - time_now);
//         #endif // DEBUG

//         if((!err) && (voltage_now >= t_voltage_max))
//         {
//             if(time_now - time_start > 1.0)
//             {
//                 vlimcount ++;
//                 if(vlimcount > 10)
//                 {
//                     printf("Fatal Error: Voltage Reached Upper Limit %fV, exiting...\n", t_voltage_max);
//                     return 1.0;
//                 }
//             }
//         }
//         else if((!err) && (voltage_now <= t_voltage_min))
//         {
//             if(time_now - time_start > 1.0)
//             {
//                 vlimcount ++;
//                 if(vlimcount > 10)
//                 {
//                 printf("Fatal Error: Voltage Reached Lower Limit %fV, exiting...\n", t_voltage_min);
//                 return 1.0;
//                 }
//             }
//         }
//         else 
//         {
//             vlimcount = 0;
//         }
//     }
//     printf("\nEnd of Waveform\n");


//     return 0.0f;
// }


// double E5270Interface::GetToVoltage(double t_voltage_target, double t_current_max, double t_current_end, double t_timeout)
// {
//     #if DEBUG
//         printf("\nFUNCTION: E5270Interface::GetToVoltage\n");
//         printf(">> %f, %f, %f, %f\n", t_voltage_target, t_current_max, t_current_end, t_timeout);
//     #endif // DEBUG
//     return 0;
// }

// double E5270Interface::MoveCharge(double t_voltage_limit, double t_current_max, double t_charge_to_move)
// {
//     #if DEBUG
//         printf("\nFUNCTION: E5270Interface::MoveCharge\n");
//         printf(">> %f, %f, %f\n", t_voltage_limit, t_current_max, t_charge_to_move);
//     #endif // DEBUG

//     return 0;
// }

// int E5270Interface::Write(char *inst)
// {
//     #if DEBUG
//         printf("FUNCTION: E5270Interface::Write\n");
//         printf(">> %s", inst);
//     #endif // DEBUG
    
//     CheckError(0);

// 	visa::wbstr(device, inst);

//     return CheckError(1);
// }

// int E5270Interface::Query(char *inst, char *val)
// {
//     #if DEBUG
//         printf("FUNCTION: E5270Interface::Query\n");
//         printf(">> %s", inst);
//     #endif // DEBUG
   
//     CheckError(0);
 
// 	visa::wbstr(device, inst);
//     visa::rbstr(device, val, 255);
//     #if DEBUG
//         printf(val);
//     #endif // DEBUG 

//     return CheckError(1);
// }

// inline int E5270Interface::CheckError(int t_debug)
// {
//     int error = 0;
//     char buff[256];

//     sprintf(buff, "ERR?\n");
//     visa::wbstr(device, buff);
//     visa::rbstr(device, buff, 255);
//     #if DEBUG
//     if (t_debug)
//         printf(buff);
//     #endif // DEBUG 
//     error = strncmp(buff, "0,0,0,0", 7);
//     return error;
// }


// double E5270Interface::Test(double V, double I)
// {
//     #if DEBUG
//         printf("\nFUNCTION: E5270Interface::Test\n");
//         printf(">> %f, %f\n", V, I);
//     #endif // DEBUG

//     char filename[256]; 
//     double flist[16] = {500e-3, 200e-3, 100e-3, 50e-3, 20e-3, 10e-3, 5e-3, 2e-3, 1e-3, 500e-6, 200e-6, 100e-6, 50e-6, 20e-6, 0};


//     double frequency[16] = {0};
    
//     for(int i = 0; i < 15; i += 3)
//     {
//         sprintf(filename, "Sweeptest_3Tone%iuHz.tvi", (int)(flist[i]*1e6)); 
//         ChangeDataFile(filename);
//         frequency[0] = flist[i];
//         frequency[1] = flist[i+1];
//         frequency[2] = flist[i+2];
//         Waveform(4.2, 3.0, 0.1, 0.042, 6, frequency);
//     }

//     visa::mwait(1000);

//     return 0.0f;
// }
