

#include "PsuInterface.hpp"
// #include "GpibDevice.hpp"
#include "LinuxSerialDevice.hpp"

PsuInterface::PsuInterface(char serial_mode[256], char serial_value[256],
            double max_voltage, double min_voltage, double max_current, char filestring[255]):
max_voltage_(max_voltage), min_voltage_(min_voltage), max_current_(max_current)
{
    #if DEBUG
        printf("Start of PsuInterface Constructor\n");
    #endif // DEBUG

    // Setup data and log files
	if (filestring[0] != '\0') {
        for(int i = 0; i < 256; i++)
            this->filestring_[i] = filestring[i];
    }

	if (filestring[0] != '\0') {
        sprintf(val_, "%s.tvi", filestring);
    } else {
        sprintf(val_, "%s.tvi", "DataFile");
    }
    if (ChangeDataFile(val_)){
        printf("Failed to open data file\n");
        exit(1);
	}

	if (filestring[0] != '\0') {
        sprintf(val_, "%s.log", filestring);
    } else {
        sprintf(val_, "%s.log", "LogFile");
    }
    if (ChangeLogFile(val_)) {
        printf("Failed to open log file\n");
        exit(1);
    }


    if(strncmp(serial_mode, "/dev/", 5) == 0)
    {
    #ifdef __linux__
        device_ = new LinuxSerialDevice(serial_mode, atoi(serial_value));
    #else
        printf("Error: Unable to use linux serial devices on a non-linux system\n");
        exit(1);
    #endif
    }

    if(strncmp(serial_mode, "COM", 3) == 0)
    {
    #ifdef _WIN64
        printf("Error: WindowsSerialDevice not defined yet\n");
        exit(1);
        // device_ = new WindowsSerialDevice(serial_mode, atoi(serial_value));
    #else
        printf("Error: Unable to use windows COM port on a non-windows system\n");
        exit(1);
    #endif
    }


    // if(strncmp(serial_mode, "GPIB", 4) == 0)
    // {
    //     device_ = new GpibDevice(serial_mode, atoi(serial_value));
    // }
    // if(isdigit(serial_mode[0]))
    // {
    //     device_ = new GpibDevice(atoi(serial_mode), atoi(serial_value));
    // }





    // start time for timed operations
	time(&t0);
	clock_initial_ = clock();

    #if DEBUG
    	printf("End of PsuInterface Constructor\n");
    #endif // DEBUG
    // Connected to device, inherited constructors to take over remainder of handshake

} // PsuInterface ctor

PsuInterface::~PsuInterface()
{
    // close log files and exit
    fclose(p_data);
    fclose(p_log);
    delete(device_);

    #if DEBUG
    	printf("End of PsuInterface Destructor\n");
    #endif // DEBUG

}


double PsuInterface::CycleBattery(const int number_cycles, const double voltage_max, const double voltage_min,
                          const double current_max, const double current_end, const double charge_end, const double timeout, const double relax_time)
{
    int cycle_count = 0;
    double charge_moved = 0;
    double discharge_q = 0;
    double charge_q = 0;
    clock_t clock_function_start = clock();
    clock_t clock_now = 0;
    double time_now = 0;
    double time_last = 0;
    current_cycle_ = 0;
    char function_data[2048];

    // print and log the function call details, push scope for single use large char array
    sprintf(function_data, "CycleBattery: VISA address %s, started\n"
            "number_cycles: %i \nvoltage_max: %f \nvoltage_min: %f \ncurrent_max: %f"
            "\ncurrent_end: %f \ncharge_end: %f \ntimeout: %f \nrelax_time: %f \n",
            val_, number_cycles, voltage_max, voltage_min,
            current_max, current_end, charge_end, timeout, relax_time);
    printf("%s", function_data);
    WriteLog(function_data);

    // ensure that the end charge value is less than 1 so we don't over charge the battery
    if(charge_end > 1) {
        printf("Error, end charge percent must be less than 100\n");
        sprintf(function_data, "Error, end charge percent must be less than 100\n");
        WriteLog(function_data);
        return -1;
    }

    // initial charge of the battery
    charge_moved = GetToVoltage(voltage_max, current_max, current_end, timeout);
    printf("End of inital charge\n");
    clock_now_ = clock();
    sprintf(function_data, "CycleBattery: initial charge completed\nCharge Moved: %f, time taken: %f\n",
                charge_moved, (float)(clock_now_-clock_function_start)/CLOCKS_PER_SEC);
    WriteLog(function_data);
    sprintf(function_data, "Cycle %i Charge Moved: %f", 0,  charge_moved);
    MarkData(function_data);

    // Start cycling the battery
    while(cycle_count++ < number_cycles)
    {
        current_cycle_ = cycle_count;
        // discharge battery to minimum voltage
        charge_moved = GetToVoltage(voltage_min, current_max, current_end, timeout);
        discharge_q = charge_moved;
        clock_now_ = clock();
        sprintf(function_data, "CycleBattery: discharge %i completed\nCharge Moved: %f, time taken: %f\n",
                cycle_count, discharge_q, (float)(clock_now_-clock_function_start)/CLOCKS_PER_SEC);
        WriteLog(function_data);
        sprintf(function_data, "Cycle %i Charge Moved: %f", cycle_count,  charge_moved);
        MarkData(function_data);

        // charge battery to maximum voltage
        charge_moved = GetToVoltage(voltage_max, current_max, current_end, timeout);
        charge_q = charge_moved;
        clock_now_ = clock();
        sprintf(function_data, "CycleBattery: charge %i completed\nCharge Moved: %f, time taken: %f\n",
                cycle_count, charge_q, (float)(clock_now_-clock_function_start)/CLOCKS_PER_SEC);
        WriteLog(function_data);
        sprintf(function_data, "Cycle %i Charge Moved: %f", cycle_count,  charge_moved);
        MarkData(function_data);

    }

    // move the excess charge from the battery
    charge_moved = MoveCharge(voltage_max, voltage_min, current_max, discharge_q*(1-charge_end));

    sprintf(function_data, "CycleBattery: cycling completed\n"
            "Last full charge Q: %f\nLast full discharge Q: %f\nEnd charge depleted: %f\ntime taken: %f\n",
            charge_q, discharge_q, charge_moved, (float)(clock_now_-clock_function_start)/CLOCKS_PER_SEC);
    WriteLog(function_data);
    sprintf(function_data, "Final Charge Moved: %f", charge_moved);
    MarkData(function_data);

    // wait out the relaxation time, continue to measure the voltage and current_t on the battery
    // required to have output on to read with high imedance mode on, give vmax to set measurement range to appropriate value
    SMUCurrent(voltage_max, voltage_min, 0.0f);
    OutputOn();

    clock_now = clock();
    time_now = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;
    time_last = time_now;
    while((time_now - time_last) < relax_time) {
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

double PsuInterface::Waveform(const double voltage_max, const double voltage_min, const double current_max,
                                const double charge_max, const int number_cycles, const double frequency[16])
{
    #if DEBUG
        printf("Call to Waveform\n");
        printf("Vmax: %f, Vmin: %f, I: %f, Q: %f, F: %e\n", voltage_max, voltage_min, current_max, charge_max, frequency[0]);
    #endif // DEBUG
    printf("Vmax: %f, Vmin: %f, I: %f, Q: %f, F: %e\n", voltage_max, voltage_min, current_max, charge_max, frequency[0]);

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
    double fmin = frequency[0];
    double fmax = frequency[0];
    double frequency_max_current_t[16] = {0};

    // check if valid frequencies are given
    if(frequency[0] == 0)
    {
        printf("Error invalid frequency 0Hz given as first frequency\n");
        return 1.0;
    }

    // find highest and lowest frequecies given
    int nfrequencies = 1;
    for(int i = 1; i < 16; i++)
    {
        if(frequency[i]!= 0)
        {
            if(frequency[i]<fmin)
            {
                fmin = frequency[i];
            }
            if(frequency[i]>fmax)
            {
                fmax = frequency[i];
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
    max_dq = current_max/(M_PI*fmin*3600);
    if (fabs(charge_max) < fabs(max_dq))
    {
        max_dq = charge_max;
    }
    #if DEBUG
        printf("I qmax: %g\n", current_max/(M_PI*fmin*3600));
        printf("Given qmax: %g\n", charge_max);
        printf("dQ: %f\n", max_dq);
    #endif // DEBUG

    printf("\n");
    for(int i = 0; i < nfrequencies; i++)
    {
        // divide amplitude of each sine wave to produce equal charge movement for each
        // dQ_f = -2*nfreq*I_f/(2*pi*f)
        // dI_f = Q_f*(2*pi*f)/2*nfreq
        // dI_f = Q_f*(pi*f)/nfreq
        frequency_max_current_t[i] = max_dq * M_PI * frequency[i]*3600/(nfrequencies);
        if(fabs(frequency_max_current_t[i]) > fabs(current_max/nfrequencies))
        {
            frequency_max_current_t[i] = current_max/nfrequencies;
        }
        // #if DEBUG
            printf("Maximum current_t for frequency %e Hz: %e A\n", frequency[i], frequency_max_current_t[i]);
        // #endif // DEBUG
    }

    // set maximum time to run
    time_end = number_cycles/fmin;

    SMUCurrent(voltage_max, voltage_min, 0);
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
            current_now += frequency_max_current_t[i]*sin(2*M_PI*frequency[i]*(time_now - time_start));
            #if DEBUG
                printf("F: %e, T: %e, C: %e\n",frequency[i], time_now - time_start, frequency_max_current_t[i]*sin(2*M_PI*frequency[i]*(time_now - time_start)));
            #endif // DEBUG
        }

        SMUCurrent(voltage_max, voltage_min, current_now);
        mwait(100);

        err = GetOutput(&voltage_now, &current_now);
        if((!err) && (time_now - time_start > 1.0))
        {
            WriteData(voltage_now, current_now);
        }
        #ifndef DEBUG
        printf("Voltage: %2.4fV, Current: %2.4eA, Cycle Number: %i, time remaining: %5.2fs\r", voltage_now, current_now, 1, time_end - time_now);
        #endif // DEBUG

        if((!err) && (voltage_now >= voltage_max))
        {
            if(time_now - time_start > 1.0)
            {
                vlimcount ++;
                if(vlimcount > 10)
                {
                    printf("Fatal Error: Voltage Reached Upper Limit %fV, exiting...\n", voltage_max);
                    return 1.0;
                }
            }
        }
        else if((!err) && (voltage_now <= voltage_min))
        {
            if(time_now - time_start > 1.0)
            {
                vlimcount ++;
                if(vlimcount > 10)
                {
                printf("Fatal Error: Voltage Reached Lower Limit %fV, exiting...\n", voltage_min);
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

double PsuInterface::GetToVoltage(const double voltage_target, const double current_max, const double current_end, const double timeout)
{
    double voltage_now = 0;
    double current_now = 0;
    double charge_moved = 0;
    clock_t clock_function_start = 0;
    clock_t clock_now = 0;
    double time_now = 0;
    double time_last = 0;
    double time_timeoustart = 0;
    char function_data[2048];

    OutputOff();

    SMUVoltage(voltage_target, current_max);

    OutputOn();

    clock_function_start = clock();
    clock_now = clock();
    time_last = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;
    time_now = time_last;
    time_timeoustart = time_now;
    do { // integrate the current_t while waiting for the current_t to drop to the minimum level or the timeout condition
        // GetOutput(&voltage_now, &current_now);
        clock_now = clock();
        time_now = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;
        charge_moved += (time_now-time_last) * current_now;
        time_last = time_now;
        if (!GetOutput(&voltage_now, &current_now))
        {
            WriteData(voltage_now, current_now);
        }
        // Query if supply is current_t limited, val is 1 if true. Reset timeout if true
        // printf("\nCurrent Limited: %i, %f\n", IsCurrentLimited(), time_timeoustart);
        if (IsCurrentLimited()) {
            time_timeoustart = time_now;
        }
        #ifndef DEBUG
        printf("Voltage: %f, Current: %f, Cycle Number: %i, Charge Moved: %.0f mAh, wait time: %.2fs\r", voltage_now, current_now, current_cycle_, charge_moved/3.6, time_now - time_timeoustart);
        #endif // DEBUG
    } while(fabs(current_now) > current_end && (time_now - time_timeoustart) < timeout);
    printf("\n");

    OutputOff();

    clock_now_ = clock();
    sprintf(function_data, "GetToVoltage completed\nCharge Moved: %f, time taken: %f\n",
            charge_moved, (float)(clock_now_-clock_function_start)/CLOCKS_PER_SEC);
    WriteLog(function_data);

    return charge_moved;
}

double PsuInterface::MoveCharge(const double voltage_max, const double voltage_min, const double current_max, const double charge_to_move)
{
    double voltage_now = 0;
    double current_now = 0;
    double charge_moved = 0;
    clock_t clock_function_start = 0;
    clock_t clock_now = 0;
    double time_now = 0;
    double time_last = 0;
    char function_data[2048];

    sprintf(function_data, "Call to MoveCharge\nI: %f \nQ: %f\n", current_max, charge_to_move);
    WriteLog(function_data);

    OutputOff();
    // set output to voltage limited current_t source of correct polarity
    SMUCurrent(voltage_max, voltage_min , copysign(current_max, charge_to_move));
    OutputOn();

    clock_function_start = clock();
    clock_now = clock();
    time_last = (float)(clock_now - clock_function_start)/CLOCKS_PER_SEC;

    // integrate current_t while charge moved is less than target
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

    } while(fabs(charge_moved) < fabs(charge_to_move));

    OutputOff();

    clock_now_ = clock();
    sprintf(function_data, "MoveCharge completed\nCharge Moved: %f, time taken: %f\n",
            charge_moved, (float)(clock_now_-clock_function_start)/CLOCKS_PER_SEC);
    WriteLog(function_data);

    return charge_moved;
}

int PsuInterface::Write(char *inst)
{
    #if DEBUG
        printf("\nFUNCTION: Write\n");
        printf(">> %s\n", inst);
    #endif // DEBUG

    ClearErrors();

    device_->Write(inst);

    return CheckErrors();
}

int PsuInterface::Query(char *inst, char *val)
{
    #if DEBUG
        printf("FUNCTION: Query\n");
        printf(">> %s\n", inst);
    #endif // DEBUG

    char buff[256];
    int error = 0;

    ClearErrors();

    device_->Write(inst);
    device_->Read(val);

    return CheckErrors();
}

int PsuInterface::WriteData(double voltage, double current)
{
    clock_now_ = clock();
    fprintf(p_data, "%f\t%.8f\t%.8f\n", (float)(clock_now_-clock_initial_)/CLOCKS_PER_SEC, voltage, current);
    return 0;
}

int PsuInterface::MarkData(const char *str)
{
    fprintf(p_data, "#%s \n", str);
    return 0;
}

int PsuInterface::WriteLog(const char *str)
{
    time(&t);
    clock_now_ = clock();
    fprintf(p_log, "%li\t%s %s\n", (int)(clock_now_-clock_initial_)/CLOCKS_PER_SEC, ctime(&t), str);
    return 0;
}

int PsuInterface::ChangeDataFile(const char *str)
{
    if(p_data!=nullptr) {
        fclose(p_data);
    }
    p_data = fopen(str, "w+");
    setbuf (p_data , NULL);

    if(p_data!=nullptr)
        return 0;
    return 1;
}
int PsuInterface::ChangeLogFile(const char *str)
{
    if(p_log!=nullptr) {
        fclose(p_log);
    }
    p_log = fopen(str, "w+");
    setbuf (p_log , NULL);

    if(p_log!=nullptr)
        return 0;
    return 1;

}


void PsuInterface::mwait(int msecs) // wait some milliseconds in real-time work
{
    clock_t fin = clock() + (clock_t)((CLOCKS_PER_SEC * (long)msecs)/1000.0L);
    if(msecs==0) return;
    while (fin - clock() > 1L) {};
    return;
}


/*
double PsuInterface::Test(double V, double I)
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

    // mwait(1000);

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

    mwait(1000);

    return 0.0f;
}
*/
