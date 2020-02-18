#include "KeithleyInterface.hpp"
#include <cmath>

KeithleyInterface::KeithleyInterface(int id,
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
     	printf("Start of Keithley Interface Constructor\n");
    #endif // DEBUG

    Beep(210, 0.25);
    Beep(400, 0.25);
    Beep(750, 0.25);
    Beep(750, 0.25);
    Beep(600, 0.25);

    sprintf(cmd, "*IDN?");
    Query(cmd, cmd);
    printf("%s", cmd);

    // if(cmd != KeithleyIDNSTR)
    // {
    //     printf("Device at address GPIB%i::%i::INSTR was not the expected device, exiting...\n", id, addr);
    //     exit(1);
    // }

    sprintf(cmd, "OUTPUT:STATE OFF");
    if (Write(cmd))    {   printf("ERROR: Error setting output off\n"); }

    sprintf(cmd, "*CLS");
    if (Write(cmd))    {   printf("ERROR: Error clearing error buffer\n"); }

    sprintf(cmd, "*RST");
    if (Write(cmd))    {   printf("ERROR: Error setting reseting device\n"); }

    visa::mwait(1000);

    sprintf(cmd, "OUTPUT1:CURRENT:SMODE HIMPEDANCE");
    if (Write(cmd))     {   printf("Failed to set current output off mode to High Impedance");  }
    sprintf(cmd, "OUTPUT1:VOLTAGE:SMODE HIMPEDANCE");
    if (Write(cmd))     {   printf("Failed to set voltage output off mode to High Impedance");  }
    sprintf(cmd, "OUTPUT:STATE OFF");
    if (Write(cmd))    {   printf("ERROR: Error setting output off\n"); }

    sprintf(cmd, "SENSE:CURRENT:RSENSE 1");
    if (Write(cmd))    {   printf("ERROR: Error setting current sense to 4 wire method\n"); }
    sprintf(cmd, "SENSE:VOLTAGE:RSENSE 1");
    if (Write(cmd))    {   printf("ERROR: Error setting voltage sense to 4 wire method\n"); }
    sprintf(cmd, "SOUR:CURR:HIGH:CAP ON");
    if (Write(cmd))    {   printf("ERROR: Error setting output high capacitance mode on\n"); }

    do
    {
        sprintf(cmd, "SYSTem:ERRor:NEXT?");
        Query(cmd, cmd);
        printf("%s\n", cmd);
    } while (atoi(cmd) != 0);

	printf("KeithleyInterface (V%.2f): VISA address %s, started at %s", 1.0, busname, ctime(&t0));

    #if DEBUG
        printf("End of Keithley Interface Constructor\n");
    #endif // DEBUG

    //ctor
}

KeithleyInterface::~KeithleyInterface()
{
 	#if DEBUG
        printf("Start of Keithley Interface Destructor\n");
    #endif // DEBUG

    visa::mwait(850);
    Beep(750, 0.25);
    Beep(500, 0.25);
    Beep(210, 0.25);
    Beep(210, 0.25);
    Beep(300, 0.25);

    sprintf(cmd, "OUTPUT:STATE OFF");
    Write(cmd);

    printf("Closing KeithleyInterface Device\n");
    #if DEBUG
        printf("End of Keithley Interface Destructor\n");
    #endif // DEBUG

    //dtor
}

int KeithleyInterface::SetOutput(double V, double I)
{
    #if DEBUG
	    printf("Call to KeithleyInterface::SetOutput\n");
    #endif // DEBUG
    return SMUVoltage(V,I);
}

int KeithleyInterface::GetOutput(double * V, double * I)
{
    int Ierr = 0;
    int Verr = 0;
    char inst[256];
    char val[256];

    #if DEBUG
	    printf("Call to KeithleyInterface::GetOutput\n");
    #endif // DEBUG

    sprintf(inst, "MEASURE:CURRENT:DC?");
    Ierr = Query(inst, val);
    if (Ierr)   {   printf("Error reading output current\n");    }
    #if DEBUG
        printf("QUERY: %s", val);
    #endif // DEBUG

    *I = atof(val);
    if (*I == 9.9e37)
    {
        printf("\n###########\n");
        sprintf(inst, "SENSE:CURRENT:RANGE?");
        Query(inst, val);
        printf("IRange: %s\n", val);
        Ierr = 1;
        
    }

    sprintf(inst, "MEASURE:VOLTAGE:DC?");
    Verr = Query(inst, val);
    if (Verr)    {  printf("Error reading output voltage\n");    }
    #if DEBUG
        printf("QUERY: %s", val);
    #endif // DEBUG
    *V = atof(val);

    // WriteData(*V, *I);
    return Verr|Ierr;
}

int KeithleyInterface::SMUVoltage(double V, double I)
{

    #if DEBUG
	    printf("Call to KeithleyInterface::SMUVoltage\n");
        printf("V: %f, I: %f\n", V, I);
    #endif // DEBUG

    // sprintf(cmd, "SOUR:FUNC VOLT");
    // visa::wbstr(device, cmd);
    // SetVoltageRange(fabs(V));
    // SetCurrentRange(fabs(I*10));
    // sprintf(cmd, "SOUR:VOLT:ILIM %1.3e", I);
    // visa::wbstr(device, cmd);
    // sprintf(cmd, "SOUR:VOLT %2.3e", V);
    // visa::wbstr(device, cmd);

    // sprintf(cmd, "*WAI");
    // visa::wbstr(device, cmd);
    sprintf(m_inst, "SOUR:FUNC VOLT");
    if(Write(m_inst))   {printf("Error Setting output to voltage\n"); }
    SetVoltageRange(fabs(V));
    SetCurrentRange(fabs(I*10));
    sprintf(m_inst, "SOUR:VOLT:ILIM %1.3e", I);
    if(Write(m_inst))   {printf("Error Setting voltage current limit\n"); }
    sprintf(m_inst, "SOUR:VOLT %2.3e", V);
    if(Write(m_inst))   {printf("Error Setting voltage\n"); }

    sprintf(m_inst, "*WAI");
    if(Write(m_inst))   {printf("Error sending wait command\n"); }

    visa::mwait(200);
    return 0.0f;
}

int KeithleyInterface::SMUCurrent(double t_voltage_max, double t_voltage_min, double t_current)
{
    #if DEBUG
	    printf("Call to KeithleyInterface::SMUCurrent\n");
        printf("V: %f, I: %f\n", t_voltage_max, t_current);
    #endif // DEBUG

    // sprintf(cmd, "SOUR:FUNC CURR");
    // visa::wbstr(device, cmd);
    // SetVoltageRange(fabs(V));
    // SetCurrentRange(fabs(I*10));
    // sprintf(cmd, "SOUR:CURR:VLIM %2.3e", V);
    // visa::wbstr(device, cmd);
    // sprintf(cmd, "SOUR:CURR %1.3e", I);
    // visa::wbstr(device, cmd);

    // sprintf(cmd, "*WAI");
    // visa::wbstr(device, cmd);
    sprintf(m_inst, "SOUR:FUNC CURR");
    if(Write(m_inst))   {printf("Error Setting output to current\n"); }
    SetVoltageRange(fabs(t_voltage_max));
    SetCurrentRange(fabs(t_current));
    sprintf(m_inst, "SOUR:CURR:VLIM %2.3e", t_voltage_max);
    if(Write(m_inst))   {printf("Error Setting current voltage limit\n"); }
    sprintf(m_inst, "SOUR:CURR %1.3e", t_current);
    if(Write(m_inst))   {printf("Error Setting current\n"); }

    sprintf(m_inst, "*WAI");
    if(Write(m_inst))   {printf("Error sending wait command\n"); }

    visa::mwait(200);
    return 0.0f;
}

double KeithleyInterface::SetVoltageRange(double V)
{
    #if DEBUG
	    printf("Call to KeithleyInterface::SetVoltageRange\n");
    #endif // DEBUG

    // sprintf(m_inst, "SOURce:VOLTage:RANGe:AUTO ON");
    // if (Write(m_inst))    {   printf("Error setting voltage SOURCE range\n");    }
    // sprintf(m_inst, "SENSe:VOLTage:RANGe:AUTO ON");
    // if (Write(m_inst))    {   printf("Error setting voltage SOURCE range\n");    }

    // sprintf(m_inst, "*OPC?");
    // Query(m_inst, m_val);
    // while(m_val[0] != '1')
    // {
    //     Query(m_inst, m_val);
    // }
    // return 0;
    double range = -1;
    if(V < 100e-6)
    {
        printf("Invalid voltage %f, unable to determine range\n", V);
        range = -1;
    }
    if (V >=100e-9)
        range = 1e-6;
    if (V >= 0.21)
        range = 2;
    if (V >= 2.1)
        range = 7;
    if (V >= 7.35)
        range = 10;
    if (V >= 10.5)
        range = 20;
    if (V >= 21)
        range = 100;
    if (V >= 105)
    {
        range = -1;
        printf("Invalid voltage %f, unable to determine range\n", V);
    }

    if (range != -1)
    {

        sprintf(val, "SOURCE:VOLTAGE:RANGE %f", range);
        if (Write(val))    {   printf("Error setting voltage SOURCE range\n");    }


        sprintf(inst, "SOUR:FUNC?");
        if (Query(inst,val))    {   printf("Error reading voltage SOURCE type\n");    }

        if(abs(strncmp(val, "VOLT", 4)))
        {
            sprintf(val, "SENSE:VOLTAGE:RANGE %f", range);
            if (Write(val))    {   printf("Error setting voltage SENSE range\n");    }
        }
    }
    
    sprintf(cmd, "*WAI");
    visa::wbstr(device, cmd);
    return range;
}

double KeithleyInterface::SetCurrentRange(double I)
{

    // sprintf(m_inst, "SOURce:CURRent:RANGe:AUTO ON");
    // if (Write(m_inst))    {   printf("Error setting current SOURCE range\n");    }
    // sprintf(m_inst, "SENSe:CURRent:RANGe:AUTO ON");
    // if (Write(m_inst))    {   printf("Error setting current SOURCE range\n");    }
    
    // sprintf(m_inst, "*OPC?");
    // Query(m_inst, m_val);
    // while(m_val[0] != '1')
    // {
    //     Query(m_inst, m_val);
    // }
    // return 0;
    double range = -1;
    if((I < 100e-9) && (I != 0.0f))
    {
        printf("Invalid current %f, unable to determine range\n", I);
        range = -1;
    }
    if ((fabs(I) >=100e-9) || (I == 0.0f))
        range = 1e-6;
    if (fabs(I) >= 1.05e-6)
        range = 10e-6;
    if (fabs(I) >= 10.5e-6)
        range = 100e-6;
    if (fabs(I) >= 105e-6)
        range = 1e-3;
    if (fabs(I) >= 1.05e-3)
        range = 10e-3;
    if (fabs(I) >= 10.5e-3)
        range = 100e-3;
    if (fabs(I) >= 105e-3)
        range = 1.0;
    if (fabs(I) >= 1.05)
        range = 4.0;
    if (fabs(I) >= 4.2)
        range = 5.0;
    if (fabs(I) >= 5.25)
        range = 7.0;
    if (fabs(I) >= 7.35)
        range = -1;

    if (range != -1)
    {
        sprintf(val, "SOURCE:CURRENT:RANGE %f", range);
        if (Write(val))    {   printf("Error setting current SOURCE range\n");    }

        sprintf(inst, "SOUR:FUNC?");
        if (Query(inst,val))    {   printf("Error reading current SOURCE type\n");    }

        if(abs(strncmp(val, "CURR", 4)))
        {
            sprintf(val, "SENSE:CURRENT:RANGE %f", range);
            if (Write(val))    {   printf("Error setting current SENSE range\n");    }
        }
    }
    
    sprintf(cmd, "*WAI");
    visa::wbstr(device, cmd);
    return range;
}

int KeithleyInterface::IsCurrentLimited(void)
{
        // Query if supply is current limited, val is 1 if true. Reset timeout if true
    sprintf(m_inst, "SOUR:VOLT:ILIM:TRIP?");
    if(Query(m_inst, m_val))    {   printf("ERROR: Error reading current trip limit\n"); }
    #if DEBUG
	    printf("%s", m_val);
    #endif // DEBUG
    if(m_val[0] == '1')
    {
        return 1;
    }
    return 0;
}

int KeithleyInterface::OutputOn(void)
{
    sprintf(m_inst, "OUTPUT:STATE ON");
    return Write(m_inst);
}

int KeithleyInterface::OutputOff(void)
{
    sprintf(m_inst, "OUTPUT:STATE OFF");
    return Write(m_inst);
}

int KeithleyInterface::ClearErrors()
{
    char buff[256];
    int error = 0;

    do // clear last errors if any remain
    {
        visa::wbstr(device, ":SYSTem:ERRor:NEXT?");
        visa::rbstr(device, buff, 255);
    } while (atoi(buff) != 0);

    return 0;
}

int KeithleyInterface::CheckErrors()
{
    char buff[256];
    int error = 0;

    visa::wbstr(device, ":SYSTem:ERRor:NEXT?");
    visa::rbstr(device, buff, 255);

    error = (atoi(buff) != 0);
    return error;

}

// f is the frequency of the beeper in Hz
// t is the time for the beep to last in seconds
void KeithleyInterface::Beep(int f, double t)
{
    char beep[255];
    sprintf(beep, ":SYSTem:BEEPer:IMMediate %i, %.3f", f, t);
    visa::wbstr(device, beep);
    return;
}

// int KeithleyInterface::Write(char *inst)
// {
//     #if DEBUG
//         printf("\nFUNCTION: KeithleyInterface::Write\n");
//         printf(">> %s\n", inst);
//     #endif // DEBUG

//     char buff[256];
//     int error = 0;

//     do // clear last errors if any remain
//     {
//         visa::wbstr(device, ":SYSTem:ERRor:NEXT?");
//         visa::rbstr(device, buff, 255);
//     } while (atoi(buff) != 0);


//     visa::wbstr(device, inst);

//     visa::wbstr(device, ":SYSTem:ERRor:NEXT?");
//     visa::rbstr(device, buff, 255);

//     error = (atoi(buff) != 0);
//     return error;
// }

// int KeithleyInterface::Query(char *inst, char *val)
// {
//     #if DEBUG
//         printf("\nFUNCTION: KeithleyInterface::Query\n");
//         printf(">> %s\n", inst);
//     #endif // DEBUG

//     char buff[256];
//     int error = 0;

//     do // clear last errors if any remain
//     {
//         visa::wbstr(device, ":SYSTem:ERRor:NEXT?");
//         visa::rbstr(device, buff, 255);
//     } while (atoi(buff) != 0);


//     visa::wbstr(device, inst);
//     visa::rbstr(device, val, 255);

//     visa::wbstr(device, ":SYSTem:ERRor:NEXT?");
//     visa::rbstr(device, buff, 255);

//     error = (atoi(buff) != 0);
//     return error;
// }

// // bool KeithleyInterface::NComplete(void)
// // {
// //     char inst[256];
// //     bool error = false;
// //     sprintf(inst, "*OPC");
// //     if (Write(inst))    {   printf("ERROR: Error checking command completion \n"); error = true;    }

// //     sprintf(inst, "*OPC?");
// //     if (Query(inst, inst))  {   printf("ERROR: Error checking command completion \n"); error = true;    }
// //     #if DEBUG
// //         printf("QUERY: %s", inst);
// //     #endif // DEBUG
// //     return error;
// // }


// void KeithleyInterface::ReadRange(void)
// {
//     char val[256];
//     sprintf(val, "SOURCE:CURRENT:RANGE?");
//     if (Query(val, val))    {   printf("Error reading current SOURCE range\n");    }
//     #if DEBUG
//         printf("QUERY: %s", val);
//     #endif // DEBUG

//     sprintf(val, "SENSE:CURRENT:RANGE?");
//     if (Query(val, val)){   printf("Error reading current SENSE range\n");    }
//     #if DEBUG
//         printf("QUERY: %s", val);
//     #endif // DEBUG

//     sprintf(val, "SOURCE:VOLTAGE:RANGE?");
//     if (Query(val, val))    {   printf("Error reading voltage SOURCE range\n");    }
//     #if DEBUG
//         printf("QUERY: %s", val);
//     #endif // DEBUG

//     sprintf(val, "SENSE:VOLTAGE:RANGE?");
//     if (Query(val, val))    {   printf("Error reading voltage SENSE range\n");    }
//     #if DEBUG
//         printf("QUERY: %s", val);
//     #endif // DEBUG

// }







// // voltage target is voltage the output will try to assert
// // current max is the maximum current limit at short circuit
// // current end is the minimum value the current will drop to before the function ends
// // timeout is the max time spent after the current draw drops below current max
// double KeithleyInterface::GetToVoltage(double t_voltage_target, double t_current_max, double t_current_end, double t_timeout)
// {
//     double voltage_now = 0;
//     double current_now = 0;
//     double charge_moved = 0;
//     clock_t clock_function_start = 0;
//     clock_t clock_now = 0;
//     double time_now = 0;
//     double time_last = 0;
//     double time_timeout_start = 0;


//     sprintf(inst, "OUTPUT:STATE OFF");
//     if (Write(inst))    {   printf("ERROR: Error setting output OFF\n"); }

//     SMUVoltage(t_voltage_target, t_current_max);

//     sprintf(inst, "OUTPUT:STATE ON");
//     if (Write(inst))    {   printf("ERROR: Error setting output ON\n"); }

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
//         sprintf(inst, "SOUR:VOLT:ILIM:TRIP?");
//         if(Query(inst, val))    {   printf("ERROR: Error reading current trip limit\n"); }
//         #if DEBUG
//             printf("QUERY: %s", val);
//         #endif // DEBUG

//         if (val[0] == '1') {
//             time_timeout_start = time_now;
//         }
//         #ifndef DEBUG
//         printf("Voltage: %f, Current: %f, Cycle Number: %i, Charge Moved: %.0f mAh, wait time: %.2fs\r", voltage_now, current_now, m_current_cycle, charge_moved/3.6, time_now - time_timeout_start);
//         #endif // DEBUG
//     } while(fabs(current_now) > t_current_end && (time_now - time_timeout_start) < t_timeout);
//     printf("\n");
//     sprintf(inst, "OUTPUT:STATE OFF");
//     if (Write(inst))    {   printf("ERROR: Error setting output OFF\n"); }


//     m_clock_now = clock();
//     sprintf(cmd, "KeithleyInterface (V%.2f) GetToVoltage completed\nCharge Moved: %f, time taken: %f\n",
//             KEITHLEY_INTERFACE_VERSION, charge_moved, (float)(m_clock_now-clock_function_start)/CLOCKS_PER_SEC);
//     WriteLog(cmd);

//     return charge_moved;
// }

// double KeithleyInterface::MoveCharge(double t_voltage_limit, double t_current_max, double t_charge_to_move)
// {
//     double voltage_now = 0;
//     double current_now = 0;
//     double charge_moved = 0;
//     clock_t clock_function_start = 0;
//     clock_t clock_now = 0;
//     double time_now = 0;
//     double time_last = 0;

//     sprintf(cmd, "Call to  KeithleyInterface::MoveCharge\nV: %f \nI: %f \nQ: %f\n", t_voltage_limit, t_current_max, t_charge_to_move);
//     WriteLog(cmd);

//     sprintf(inst, "OUTPUT:STATE OFF");
//     if (Write(inst))    {   printf("ERROR: Error setting output OFF\n"); }

//     // set output to voltage limited current source of correct polarity
//     if (t_charge_to_move > 0) {
//         SMUCurrent(t_voltage_limit, t_current_max);
//     } else if (t_charge_to_move < 0) {
//         SMUCurrent(t_voltage_limit, -1*t_current_max);
//     } else {
//         return 0.0f;
//     }

//     sprintf(inst, "OUTPUT:STATE ON");
//     if (Write(inst))    {   printf("ERROR: Error setting output ON\n"); }


//     clock_function_start = clock();
//     clock_now = clock();
//     time_last = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;

//     // integrate current while charge moved is less than target
//     do {
//         GetOutput(&voltage_now, &current_now);
//         clock_now = clock();
//         time_now = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;
//         charge_moved += (time_now-time_last) * current_now;
//         time_last = time_now;

//     } while(fabs(charge_moved) < fabs(t_charge_to_move));

//     sprintf(inst, "OUTPUT:STATE OFF");
//     if (Write(inst))    {   printf("ERROR: Error setting output OFF\n"); }

//     m_clock_now = clock();
//     sprintf(cmd, "KeithleyInterface (V%.2f) MoveCharge completed\nCharge Moved: %f, time taken: %f\n",
//             KEITHLEY_INTERFACE_VERSION, charge_moved, (float)(m_clock_now-clock_function_start)/CLOCKS_PER_SEC);
//     WriteLog(cmd);

//     return charge_moved;
// }

// double KeithleyInterface::CycleBattery(int t_number_cycles, double t_voltage_max, double t_voltage_min,
//                                       double t_current_max, double t_current_end, double t_charge_end, double t_timeout, double t_relax_time)
// {
//     int cycle_count = 0;
//     double charge_moved = 0;
//     double discharge_q = 0;
//     double charge_q = 0;
//     clock_t clock_function_start = clock();
//     clock_t clock_now = 0;
//     double time_now = 0;
//     double time_last = 0;
//     m_current_cycle = 0;
//     // print and log the function call details, push scope for single use large char array
//     {
//         char function_data[2048];
//         sprintf(function_data, "KeithleyInterface (V%.2f) CycleBattery: VISA address %s, started\n"
//                 "number_cycles: %i \nvoltage_max: %f \nvoltage_min: %f \ncurrent_max: %f"
//                 "\ncurrent_end: %f \ncharge_end: %f \ntimeout: %f \nrelax_time: %f \n",
//                 KEITHLEY_INTERFACE_VERSION, busname, t_number_cycles, t_voltage_max, t_voltage_min,
//                 t_current_max, t_current_end, t_charge_end, t_timeout, t_relax_time);
//         printf(function_data);
//         WriteLog(function_data);
//     }

//     // ensure that the end charge value is less than 1 so we don't over charge the battery
//     while(t_charge_end > 1) {
//         t_charge_end = t_charge_end/100;
//     }

//     // initial charge of the battery
//     charge_moved = GetToVoltage(t_voltage_max, t_current_max, t_current_end, t_timeout);
//     printf("End of inital charge\n");
//     m_clock_now = clock();
//     sprintf(cmd, "KeithleyInterface (V%.2f) CycleBattery: initial charge completed\nCharge Moved: %f, time taken: %f\n",
//                 KEITHLEY_INTERFACE_VERSION, charge_moved, (float)(m_clock_now-clock_function_start)/CLOCKS_PER_SEC);
//     WriteLog(cmd);
//     sprintf(cmd, "Cycle %i Charge Moved: %f", 0,  charge_moved);
//     MarkData(cmd);

//     // Start cycling the battery
//     while(cycle_count++ < t_number_cycles)
//     {
//         m_current_cycle = cycle_count;
//         // discharge battery to minimum voltage
//         charge_moved = GetToVoltage(t_voltage_min, t_current_max, t_current_end, t_timeout);
//         discharge_q = charge_moved;
//         m_clock_now = clock();
//         sprintf(cmd, "KeithleyInterface (V%.2f) CycleBattery: discharge %i completed\nCharge Moved: %f, time taken: %f\n",
//                 KEITHLEY_INTERFACE_VERSION, cycle_count, discharge_q, (float)(m_clock_now-clock_function_start)/CLOCKS_PER_SEC);
//         WriteLog(cmd);
//         sprintf(cmd, "Cycle %i Charge Moved: %f", cycle_count,  charge_moved);
//         MarkData(cmd);

//         // charge battery to maximum voltage
//         charge_moved = GetToVoltage(t_voltage_max, t_current_max, t_current_end, t_timeout);
//         charge_q = charge_moved;
//         m_clock_now = clock();
//         sprintf(cmd, "KeithleyInterface (V%.2f) CycleBattery: charge %i completed\nCharge Moved: %f, time taken: %f\n",
//                 KEITHLEY_INTERFACE_VERSION, cycle_count, charge_q, (float)(m_clock_now-clock_function_start)/CLOCKS_PER_SEC);
//         WriteLog(cmd);
//         sprintf(cmd, "Cycle %i Charge Moved: %f", cycle_count,  charge_moved);
//         MarkData(cmd);
        
//     }

//     // move the excess charge from the battery
//     charge_moved = MoveCharge(t_voltage_max, t_current_max, discharge_q*(1-t_charge_end));

//     sprintf(cmd, "KeithleyInterface (V%.2f) CycleBattery: cycling completed\n"
//             "Last full charge Q: %f\nLast full discharge Q: %f\nEnd charge depleted: %f\ntime taken: %f\n",
//             KEITHLEY_INTERFACE_VERSION, charge_q, discharge_q, charge_moved, (float)(m_clock_now-clock_function_start)/CLOCKS_PER_SEC);
//     WriteLog(cmd);
//     sprintf(cmd, "Final Charge Moved: %f", charge_moved);
//     MarkData(cmd);

//     // wait out the relaxation time, continue to measure the voltage and current on the battery
//     // required to have output on to read with high imedance mode on, give vmax to set measurement range to appropriate value
//     SMUCurrent(t_voltage_max , 0.0f);
//     sprintf(inst, "OUTPUT:STATE ON");
//     if (Write(inst))    {   printf("ERROR: Error setting output ON\n"); }

//     clock_now = clock();
//     time_now = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;
//     time_last = time_now;
//     while((time_now - time_last) < t_relax_time) {
//         double voltage_now = 0;
//         double current_now = 0;
//         GetOutput(&voltage_now, &current_now);
//         clock_now = clock();
//         time_now = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;
//     }
//     sprintf(cmd, "Cycling End");
//     MarkData(cmd);

//     sprintf(inst, "OUTPUT:STATE OFF");
//     if (Write(inst))    {   printf("ERROR: Error setting output OFF\n"); }
//     return 0.0f; // TODO return a sensible value
// }

// double KeithleyInterface::Waveform(double t_voltage_max, double t_voltage_min, double t_current_max, double t_charge_max, int t_number_cycles, double t_frequency[16])
// {
// 	printf("Call to KeithleyInterface::Waveform\n");
// 	printf("Vmax: %f, Vmin: %f, I: %f, F: %f\n", t_voltage_max, t_voltage_min, t_current_max, t_frequency[0]);
//     return 0.0f;
// }




// #if DEBUG
// double KeithleyInterface::Test(double V, double I)
// #else
// double KeithleyInterface::Test(double /*V*/, double /*I*/)
// #endif // DEBUG
// {
//     return 0;
// }
