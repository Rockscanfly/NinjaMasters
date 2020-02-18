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

    if(strncmp("HEWLETT-PACKARD,66332A,0,A.01.03", m_val, 32))
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

    // SetVoltageRange(fabs(V));
    // SetCurrentRange(fabs(I));
    // sprintf(m_inst, "SOUR:VOLT %1.3f\n", V);
    // if(Write(m_inst))   {   printf("Error: Error setting output voltage \n");    }  
    // sprintf(m_inst, "SOUR:CURR %1.3f\n", fabs(I));
    // if(Write(m_inst))   {   printf("\nError: Error setting output current: %1.3f\n", I);    }  

    // visa::mwait(200);
    // return 0.0f;
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
    
//     if(fabs(I) < 0.001)
//     {
//         I = 0;
//     }
//     SetVoltageRange(fabs(V));
//     SetCurrentRange(fabs(I));
//     sprintf(m_inst, "SOUR:VOLT %1.3f\n", V);
//     if(Write(m_inst))   {   printf("Error: Error setting output voltage \n");    }  
//     sprintf(m_inst, "SOUR:CURR %1.3f\n", fabs(I));
//     if(Write(m_inst))   {   printf("\nError: Error setting output current: %1.3f\n", I);    }  

//     sprintf(m_inst, "*WAI\n");
//     if(Write(m_inst))   {   printf("\nError: Error waiting for command completion\n");    }  

//     return 0.0f;
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

// double HP66332Interface::CycleBattery(int t_number_cycles, double t_voltage_max, double t_voltage_min,
//                         double t_current_max, double t_current_end, double t_charge_end, double t_timeout, double t_relax_time)
// {
 
//     #if DEBUG
//         printf("Call to HP66332Interface::CycleBattery\n");
//         printf(">> %d, %f, %f, %f\n", t_number_cycles, t_voltage_max, t_voltage_min, t_current_max);
//     #endif // DEBUG

//     return 0.0f;
// }

// double HP66332Interface::Waveform(double t_voltage_max, double t_voltage_min, double t_current_max, double t_charge_max, int t_number_cycles, double t_frequency[16])
// {
//     #if DEBUG
//         printf("Call to E5270Interface::Waveform\n");
//         printf("Vmax: %f, Vmin: %f, I: %f, Q: %f, F: %e\n", t_voltage_max, t_voltage_min, t_current_max, t_charge_max, t_frequency[0]);
//     #endif // DEBUG
//     printf("Vmax: %f, Vmin: %f, I: %f, Q: %f, F: %e\n", t_voltage_max, t_voltage_min, t_current_max, t_charge_max, t_frequency[0]);



//     return 0.0f;
// }


// double HP66332Interface::GetToVoltage(double t_voltage_target, double t_current_max, double t_current_end, double t_timeout)
// {
//     #if DEBUG
//         printf("\nFUNCTION: HP66332Interface::GetToVoltage\n");
//         printf(">> %f, %f, %f, %f\n", t_voltage_target, t_current_max, t_current_end, t_timeout);
//     #endif // DEBUG
 
//     double voltage_now = 0;
//     double current_now = 0;
//     double charge_moved = 0;
//     clock_t clock_function_start = 0;
//     clock_t clock_now = 0;
//     double time_now = 0;
//     double time_last = 0;
//     double time_timeout_start = 0;


//     sprintf(m_inst, "OUTPUT:STATE OFF");
//     if (Write(m_inst))    {   printf("ERROR: Error setting output OFF\n"); }

//     GetOutput(&voltage_now, &current_now);

//     if(voltage_now > t_voltage_target)
//     {
//         SMUVoltage(t_voltage_target, -1*t_current_max);
//     }else
//     {
//         SMUVoltage(t_voltage_target, t_current_max);
//     }

//     sprintf(m_inst, "OUTPUT:STATE ON");
//     if (Write(m_inst))    {   printf("ERROR: Error setting output ON\n"); }

//     clock_function_start = clock();
//     clock_now = clock();
//     time_last = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;

//     do { // integrate the current while waiting for the current to drop to the minimum level or the timeout condition
//         GetOutput(&voltage_now, &current_now);
//         clock_now = clock();
//         time_now = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;
//         charge_moved += (time_now-time_last) * current_now;
//         time_last = time_now;

//         // Query if supply is current limited, val is 1 if true. Reset timeout if true
//         if (IsCurrentLimited()) {
//             time_timeout_start = time_now;
//         }
//         #ifndef DEBUG
//         printf("Voltage: %f, Current: %f, Cycle Number: %i, Charge Moved: %.0f mAh, wait time: %.2fs\r", voltage_now, current_now, m_current_cycle, charge_moved/3.6, time_now - time_timeout_start);
//         #endif // DEBUG
//     } while(fabs(current_now) > t_current_end && (time_now - time_timeout_start) < t_timeout);
//     printf("\n");
//     sprintf(m_inst, "OUTPUT:STATE OFF");
//     if (Write(m_inst))    {   printf("ERROR: Error setting output OFF\n"); }


//     m_clock_now = clock();
//     sprintf(cmd, "HP66332Interface (V%.2f) GetToVoltage completed\nCharge Moved: %f, time taken: %f\n",
//             HP66332_INTERFACE_VERSION, charge_moved, (float)(m_clock_now-clock_function_start)/CLOCKS_PER_SEC);
//     WriteLog(cmd);

//     return charge_moved;
// }

// double HP66332Interface::MoveCharge(double t_voltage_limit, double t_current_max, double t_charge_to_move)
// {
//     #if DEBUG
//         printf("\nFUNCTION: HP66332Interface::MoveCharge\n");
//         printf(">> %f, %f, %f\n", t_voltage_limit, t_current_max, t_charge_to_move);
//     #endif // DEBUG

//     return 0;
// }




// bool HP66332Interface::NComplete(void)
// {
//     char inst[256];
//     bool error = false;
//     sprintf(inst, "*OPC");
//     if (Write(inst))    {   printf("ERROR: Error checking command completion \n"); error = true;    }

//     sprintf(inst, "*OPC?");
//     if (Query(inst, inst))  {   printf("ERROR: Error checking command completion \n"); error = true;    }
//     #if DEBUG
//         printf("QUERY: %s", inst);
//     #endif // DEBUG
//     return error;
// }

// double HP66332Interface::Test(double V, double I)
// {
//     #if DEBUG
//         printf("\nFUNCTION: E5270Interface::Test\n");
//         printf(">> %f, %f\n", V, I);
//     #endif // DEBUG

//     sprintf(m_inst, "OUTP:STAT ON");
//     if(Write(m_inst))   {   printf("Error: Error setting output state on\n");    }  
    
//     GetToVoltage(3.9, 0.1, 0.01, 3);
//     visa::mwait(200);
//     GetToVoltage(3.89, 0.1, 0.01, 3);
//     visa::mwait(200);
//     GetToVoltage(3.91, 0.1, 0.01, 3);
//     visa::mwait(200);

//     return 0.0f;
// }

