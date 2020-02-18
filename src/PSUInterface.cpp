#include "PSUInterface.hpp"

PSUInterface::PSUInterface(int id, int addr, int channel, double Vmax, double Vmin, double Imax, const char filestring[255]):
//PSUInterface::PSUInterface(id, addr, channel, Vmax, Vmin, Imax, capacity):
visaID(id),
subAddr(addr),
channel(channel),
vMax(Vmax),
vMin(Vmin),
iMax(Imax)
{
    #if DEBUG
        printf("Start of PSU Interface Constructor\n");
    #endif // DEBUG

    // Setup data and log files
	if (filestring[0] != '\0') {
        for(int i = 0; i < 256; i++)
            this->filestring[i] = filestring[i];
    }

	if (filestring[0] != '\0') {
        sprintf(cmd, "%s.tvi", filestring);
    } else {
        sprintf(cmd, "%s.tvi", "DataFile");
    }
    if (ChangeDataFile(cmd)){
        printf("Failed to open data file\n");
        exit(1);
	}

	if (filestring[0] != '\0') {
        sprintf(cmd, "%s.log", filestring);
    } else {
        sprintf(cmd, "%s.log", "LogFile");
    }
    if (ChangeLogFile(cmd)) {
        printf("Failed to open log file\n");
        exit(1);
    }

    // check visa device information
	if(visaID>255 || visaID<0) visa::err("Bad GPIB number given");
    strcpy(busname,"GPIB");
	sprintf(cmd, "%d", visaID);
	strcat(busname, cmd);

    sprintf(busname, "%s::%d::INSTR", busname, subAddr); //open session to device
    printf(busname);
    printf("\n");

   	visa::OpenVISA(&resourceManager); //create VISA session (PC -> VISA)
	device = visa::OpenDevice(resourceManager, busname); //connect session identifier to psu (VISA -> device)
	visa::setTMO(device, 9000);

    // start time for timed operations
	time(&t0);
	m_clock_initial = clock();

    #if DEBUG
    	printf("End of PSU Interface Constructor\n");
    #endif // DEBUG
    // Connected to device, inherited constructors to take over remainder of handshake

} // PSUInterface ctor

PSUInterface::~PSUInterface()
{
    // close log files and exit
    fclose(p_data);
    fclose(p_log);
    #if DEBUG
    	printf("End of PSU Interface Destructor\n");
    #endif // DEBUG

}


double PSUInterface::CycleBattery(int t_number_cycles, double t_voltage_max, double t_voltage_min,
                                      double t_current_max, double t_current_end, double t_charge_end, double t_timeout, double t_relax_time)
{
    int cycle_count = 0;
    double charge_moved = 0;
    double discharge_q = 0;
    double charge_q = 0;
    clock_t clock_function_start = clock();
    clock_t clock_now = 0;
    double time_now = 0;
    double time_last = 0;
    m_current_cycle = 0;
    char function_data[2048];

    // print and log the function call details, push scope for single use large char array
    {
        sprintf(function_data, "CycleBattery: VISA address %s, started\n"
                "number_cycles: %i \nvoltage_max: %f \nvoltage_min: %f \ncurrent_max: %f"
                "\ncurrent_end: %f \ncharge_end: %f \ntimeout: %f \nrelax_time: %f \n",
                busname, t_number_cycles, t_voltage_max, t_voltage_min,
                t_current_max, t_current_end, t_charge_end, t_timeout, t_relax_time);
        printf(function_data);
        WriteLog(function_data);
    }

    // ensure that the end charge value is less than 1 so we don't over charge the battery
    while(t_charge_end > 1) {
        t_charge_end = t_charge_end/100;
    }

    // initial charge of the battery
    charge_moved = GetToVoltage(t_voltage_max, t_current_max, t_current_end, t_timeout);
    printf("End of inital charge\n");
    m_clock_now = clock();
    sprintf(function_data, "CycleBattery: initial charge completed\nCharge Moved: %f, time taken: %f\n",
                charge_moved, (float)(m_clock_now-clock_function_start)/CLOCKS_PER_SEC);
    WriteLog(function_data);
    sprintf(function_data, "Cycle %i Charge Moved: %f", 0,  charge_moved);
    MarkData(function_data);

    // Start cycling the battery
    while(cycle_count++ < t_number_cycles)
    {
        m_current_cycle = cycle_count;
        // discharge battery to minimum voltage
        charge_moved = GetToVoltage(t_voltage_min, t_current_max, t_current_end, t_timeout);
        discharge_q = charge_moved;
        m_clock_now = clock();
        sprintf(function_data, "CycleBattery: discharge %i completed\nCharge Moved: %f, time taken: %f\n",
                cycle_count, discharge_q, (float)(m_clock_now-clock_function_start)/CLOCKS_PER_SEC);
        WriteLog(function_data);
        sprintf(function_data, "Cycle %i Charge Moved: %f", cycle_count,  charge_moved);
        MarkData(function_data);

        // charge battery to maximum voltage
        charge_moved = GetToVoltage(t_voltage_max, t_current_max, t_current_end, t_timeout);
        charge_q = charge_moved;
        m_clock_now = clock();
        sprintf(function_data, "CycleBattery: charge %i completed\nCharge Moved: %f, time taken: %f\n",
                cycle_count, charge_q, (float)(m_clock_now-clock_function_start)/CLOCKS_PER_SEC);
        WriteLog(function_data);
        sprintf(function_data, "Cycle %i Charge Moved: %f", cycle_count,  charge_moved);
        MarkData(function_data);
        
    }

    // move the excess charge from the battery
    charge_moved = MoveCharge(t_voltage_max, t_voltage_min, t_current_max, discharge_q*(1-t_charge_end));

    sprintf(function_data, "CycleBattery: cycling completed\n"
            "Last full charge Q: %f\nLast full discharge Q: %f\nEnd charge depleted: %f\ntime taken: %f\n",
            charge_q, discharge_q, charge_moved, (float)(m_clock_now-clock_function_start)/CLOCKS_PER_SEC);
    WriteLog(function_data);
    sprintf(function_data, "Final Charge Moved: %f", charge_moved);
    MarkData(function_data);

    // wait out the relaxation time, continue to measure the voltage and current on the battery
    // required to have output on to read with high imedance mode on, give vmax to set measurement range to appropriate value
    SMUCurrent(t_voltage_max, t_voltage_min, 0.0f);
    OutputOn();

    clock_now = clock();
    time_now = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;
    time_last = time_now;
    while((time_now - time_last) < t_relax_time) {
        double voltage_now = 0;
        double current_now = 0;
        // GetOutput(&voltage_now, &current_now);
        clock_now = clock();
        time_now = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;
        if(!GetOutput(&voltage_now, &current_now))
        {
            WriteData(voltage_now, current_now);
        }
        // WriteData(voltage_now, current_now);
        #ifndef DEBUG
        printf("Voltage: %f, Current: %f, wait time: %.2fs\r", voltage_now, current_now, time_now - time_last);
        #endif // DEBUG
    }
    sprintf(function_data, "Cycling End");
    MarkData(function_data);

    OutputOff();

    return 0.0f; // TODO return a sensible value
}

double PSUInterface::Waveform(double t_voltage_max, double t_voltage_min, double t_current_max, double t_charge_max, int t_number_cycles, double t_frequency[16])
{
    #if DEBUG
        printf("Call to Waveform\n");
        printf("Vmax: %f, Vmin: %f, I: %f, Q: %f, F: %e\n", t_voltage_max, t_voltage_min, t_current_max, t_charge_max, t_frequency[0]);
    #endif // DEBUG
    printf("Vmax: %f, Vmin: %f, I: %f, Q: %f, F: %e\n", t_voltage_max, t_voltage_min, t_current_max, t_charge_max, t_frequency[0]);

    clock_t clock_function_start = clock();
    clock_t clock_now = 0;
    double time_now = 0;
    double time_start = 0;
    double time_end = 0;

    double voltage_now = 0;
    double current_now = 0;
    // int cycle_count = 0;
    int vlimcount = 0;
    int err = 0;
    
    double max_dq = 0;
    double max_fq = 0;
    double fmin = t_frequency[0];
    double fmax = t_frequency[0];
    double frequency_max_current[16] = {0};

    // check if valid frequencies are given
    if(t_frequency[0] == 0)
    {
        printf("Error invalid frequency 0Hz given as first frequency\n");
        return 1.0;
    }

    // find highest and lowest frequecies given
    int nfrequencies = 1;   
    for(int i = 1; i < 16; i++)
    {
        if(t_frequency[i]!= 0)
        {
            if(t_frequency[i]<fmin)
            {
                fmin = t_frequency[i];
            }
            if(t_frequency[i]>fmax)
            {
                fmax = t_frequency[i];
            }
            nfrequencies++;
        }
            else
        {
            break;
        }    
    }
    #if DEBUG
        printf("Lowest Frequency: %f\n", fmin);
    #endif // DEBUG

    // what q will be moved by the slowest frequency in Ah
    max_dq = t_current_max/(M_PI*fmin*3600);
    if (fabs(t_charge_max) < fabs(max_dq))
    {
        max_dq = t_charge_max;
    }
    #if DEBUG
        printf("I qmax: %g\n", t_current_max/(M_PI*fmin*3600));
        printf("Given qmax: %g\n", t_charge_max);
        printf("dQ: %f\n", max_dq);
    #endif // DEBUG

    printf("\n");
    for(int i = 0; i < nfrequencies; i++)
    {
        // divide amplitude of each sine wave to produce equal charge movement for each
        // dQ_f = -2*nfreq*I_f/(2*pi*f)
        // dI_f = Q_f*(2*pi*f)/2*nfreq
        // dI_f = Q_f*(pi*f)/nfreq
        frequency_max_current[i] = max_dq * M_PI * t_frequency[i]*3600/(nfrequencies);
        if( fabs(frequency_max_current[i]) > fabs(t_current_max/nfrequencies))
        {
            frequency_max_current[i] = t_current_max/nfrequencies;
        }
        // #if DEBUG
            printf("Maximum current for frequency %e Hz: %e A\n", t_frequency[i], frequency_max_current[i]);
        // #endif // DEBUG
    }
    
    // set maximum time to run
    time_end = t_number_cycles/fmin;

    SMUCurrent(t_voltage_max, t_voltage_min, 0);
    OutputOn();
    if(!GetOutput(&voltage_now, &current_now))
    {
        WriteData(voltage_now, current_now);
    }
    
    clock_function_start = clock();
    time_start = time_now = (double)(clock_function_start)/CLOCKS_PER_SEC;
    time_end += time_now;

    // cycle_count = 0;
    while(time_now < time_end)
    {
        current_now = 0;
        clock_t clock_now = clock();

        // cycle_count = 0;
        time_now = (double)(clock_now)/CLOCKS_PER_SEC;
        for(int i = 0; i < nfrequencies; i++)
        {
            current_now += frequency_max_current[i]*sin(2*M_PI*t_frequency[i]*(time_now - time_start));
            #if DEBUG
                printf("F: %e, T: %e, C: %e\n",t_frequency[i], time_now - time_start, frequency_max_current[i]*sin(2*M_PI*t_frequency[i]*(time_now - time_start)));
            #endif // DEBUG 
        }
          
        SMUCurrent(t_voltage_max, t_voltage_min, current_now);
        visa::mwait(200);

        err = GetOutput(&voltage_now, &current_now);
        if((!err) && (time_now - time_start > 1.0))
        {
            WriteData(voltage_now, current_now);
        }
        #ifndef DEBUG
        printf("Voltage: %2.4fV, Current: %2.4eA, Cycle Number: %i, time remaining: %5.2fs\r", voltage_now, current_now, 1, time_end - time_now);
        #endif // DEBUG

        if((!err) && (voltage_now >= t_voltage_max))
        {
            if(time_now - time_start > 1.0)
            {
                vlimcount ++;
                if(vlimcount > 10)
                {
                    printf("Fatal Error: Voltage Reached Upper Limit %fV, exiting...\n", t_voltage_max);
                    return 1.0;
                }
            }
        }
        else if((!err) && (voltage_now <= t_voltage_min))
        {
            if(time_now - time_start > 1.0)
            {
                vlimcount ++;
                if(vlimcount > 10)
                {
                printf("Fatal Error: Voltage Reached Lower Limit %fV, exiting...\n", t_voltage_min);
                return 1.0;
                }
            }
        }
        else 
        {
            vlimcount = 0;
        }
    }

    OutputOff();
    printf("\nEnd of Waveform\n");
    return 0.0f;
}

double PSUInterface::GetToVoltage(double t_voltage_target, double t_current_max, double t_current_end, double t_timeout)
{
    double voltage_now = 0;
    double current_now = 0;
    double charge_moved = 0;
    clock_t clock_function_start = 0;
    clock_t clock_now = 0;
    double time_now = 0;
    double time_last = 0;
    double time_timeout_start = 0;
    char function_data[2048];

    OutputOff();

    SMUVoltage(t_voltage_target, t_current_max);

    OutputOn();

    clock_function_start = clock();
    clock_now = clock();
    time_last = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;
    time_now = time_last;
    time_timeout_start = time_now;
    do { // integrate the current while waiting for the current to drop to the minimum level or the timeout condition
        // GetOutput(&voltage_now, &current_now);
        clock_now = clock();
        time_now = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;
        charge_moved += (time_now-time_last) * current_now;
        time_last = time_now;
        if (!GetOutput(&voltage_now, &current_now))
        {
            WriteData(voltage_now, current_now);
        }
        // Query if supply is current limited, val is 1 if true. Reset timeout if true
        // printf("\nCurrent Limited: %i, %f\n", IsCurrentLimited(), time_timeout_start);
        if (IsCurrentLimited()) {
            time_timeout_start = time_now;
        }
        #ifndef DEBUG
        printf("Voltage: %f, Current: %f, Cycle Number: %i, Charge Moved: %.0f mAh, wait time: %.2fs\r", voltage_now, current_now, m_current_cycle, charge_moved/3.6, time_now - time_timeout_start);
        #endif // DEBUG
    } while(fabs(current_now) > t_current_end && (time_now - time_timeout_start) < t_timeout);
    printf("\n");
  
    OutputOff();

    m_clock_now = clock();
    sprintf(function_data, "GetToVoltage completed\nCharge Moved: %f, time taken: %f\n",
            charge_moved, (float)(m_clock_now-clock_function_start)/CLOCKS_PER_SEC);
    WriteLog(function_data);

    return charge_moved;
}

double PSUInterface::MoveCharge(double t_voltage_max, double t_voltage_min, double t_current_max, double t_charge_to_move)
{
    double voltage_now = 0;
    double current_now = 0;
    double charge_moved = 0;
    clock_t clock_function_start = 0;
    clock_t clock_now = 0;
    double time_now = 0;
    double time_last = 0;
    char function_data[2048];

    sprintf(function_data, "Call to MoveCharge\nI: %f \nQ: %f\n", t_current_max, t_charge_to_move);
    WriteLog(function_data);

    OutputOff();
    // set output to voltage limited current source of correct polarity
    SMUCurrent(t_voltage_max, t_voltage_min , copysign(t_current_max, t_charge_to_move));
    OutputOn();

    clock_function_start = clock();
    clock_now = clock();
    time_last = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;

    // integrate current while charge moved is less than target
    do {
        // GetOutput(&voltage_now, &current_now);
        clock_now = clock();
        time_now = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;
        charge_moved += (time_now-time_last) * current_now;
        time_last = time_now;
        if(!GetOutput(&voltage_now, &current_now))
        {
            WriteData(voltage_now, current_now);
        }

        #ifndef DEBUG
        printf("Voltage: %f, Current: %f, Charge Moved: %.0f mAh, wait time: %.2fs\r", voltage_now, current_now, charge_moved/3.6, time_now);
        #endif //DEBUG

    } while(fabs(charge_moved) < fabs(t_charge_to_move));

    OutputOff();

    m_clock_now = clock();
    sprintf(function_data, "MoveCharge completed\nCharge Moved: %f, time taken: %f\n",
            charge_moved, (float)(m_clock_now-clock_function_start)/CLOCKS_PER_SEC);
    WriteLog(function_data);

    return charge_moved;
}

int PSUInterface::Write(char *inst)
{
    #if DEBUG
        printf("\nFUNCTION: Write\n");
        printf(">> %s\n", inst);
    #endif // DEBUG
   
    ClearErrors();

    visa::wbstr(device, inst);

    return CheckErrors();
}

int PSUInterface::Query(char *inst, char *val)
{
    #if DEBUG
        printf("FUNCTION: Query\n");
        printf(">> %s\n", inst);
    #endif // DEBUG

    char buff[256];
    int error = 0;

    ClearErrors();

    visa::wbstr(device, inst);
    visa::rbstr(device, val, 255);

    return CheckErrors();

}

int PSUInterface::WriteData(const double v, const double i)
{
    m_clock_now = clock();
    fprintf(p_data, "%f\t%.8f\t%.8f\n", (float)(m_clock_now-m_clock_initial)/CLOCKS_PER_SEC, v, i);
    return 0;
}

int PSUInterface::MarkData(const char* str)
{
    fprintf(p_data, "#%s \n", str);
    return 0;
}

int PSUInterface::WriteLog(const char* str)
{
    time(&t);
    m_clock_now = clock();
    fprintf(p_log, "%i\t%s %s\n", (int)(m_clock_now-m_clock_initial)/CLOCKS_PER_SEC, ctime(&t), str);
    return 0;
}

int PSUInterface::ChangeDataFile(const char* str)
{
    if(p_data!=nullptr) {
        fclose(p_data);
    }
    p_data = fopen(str, "w+");
    setbuf ( p_data , NULL );

    if(p_data!=nullptr)
        return 0;
    return 1;
}
int PSUInterface::ChangeLogFile(const char* str)
{
    if(p_log!=nullptr) {
        fclose(p_log);
    }
    p_log = fopen(str, "w+");
    setbuf ( p_log , NULL );

    if(p_log!=nullptr)
        return 0;
    return 1;

}

double PSUInterface::Test(double V, double I)
{
    #if DEBUG
        printf("\nFUNCTION:Test\n");
        printf(">> %f, %f\n", V, I);
    #endif // DEBUG

    char filename[256]; 
    // double flist[32] = {500e-3, 200e-3, 100e-3, 50e-3, 20e-3, 10e-3, 5e-3, 2e-3, 1e-3, 500e-6, 200e-6, 100e-6, 50e-6, 20e-6, 0};
    // double flist[32] = {500e-3, 200e-3, 100e-3, 80e-3, 50e-3, 20e-3, 10e-3, 8e-3, 5e-3, 2e-3, 1e-3, 800e-6, 500e-6, 200e-6, 0, 0};
    double flist[32] = {500e-3, 200e-3, 100e-3, 80e-3, 50e-3, 20e-3, 10e-3, 8e-3, 5e-3, 2e-3, 1e-3, 800e-6, 500e-6, 200e-6, 100e-6, 80e-6, 50e-6, 20e-6, 0, 0, 0};

    double frequency[16] = {0};
    
    for(int i = 0; i < 18; i += 1)
    {
        sprintf(filename, "%s_%iuHz.tvi", this->filestring, (int)(flist[i]*1e6)); 
        ChangeDataFile(filename);
        frequency[0] = flist[i];
        // frequency[1] = flist[i+1];
        // frequency[2] = flist[i+2];
        // frequency[3] = flist[i+3];
        // frequency[4] = flist[i+4];
        Waveform(this->vMax, this->vMin, this->iMax, 0.042, 6, frequency);
    }

    // visa::mwait(1000);

    // for(int i = 0; i < 20; i += 5)
    // {
    //     sprintf(filename, "%s_%iuHz5tone.tvi", this->filestring, (int)(flist[i]*1e6)); 
    //     ChangeDataFile(filename);
    //     frequency[0] = flist[i];
    //     frequency[1] = flist[i+1];
    //     frequency[2] = flist[i+2];
    //     frequency[3] = flist[i+3];
    //     frequency[4] = flist[i+4];
    //     Waveform(this->vMax, this->vMin, this->iMax, 0.042, 6, frequency);
    // }

    visa::mwait(1000);

    return 0.0f;
}