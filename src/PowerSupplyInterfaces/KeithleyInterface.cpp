#include "KeithleyInterface.hpp"
#include <cmath>

KeithleyInterface::KeithleyInterface(char serial_mode[256], char serial_value[256],
            double max_voltage, double min_voltage, double max_current, char filestring[255]):
               PsuInterface::PsuInterface(serial_mode, serial_value, max_voltage, min_voltage, max_current, filestring)
{
    #if DEBUG
     	printf("Start of Keithley Interface Constructor\n");
    #endif // DEBUG



    sprintf(inst_, "*IDN?");
    Query(inst_, inst_);
    printf("IDN string\n");
    printf("%s", inst_);
    printf("\n");

    char kstring[64] = "KEITHLEY INSTRUMENTS,MODEL 2460";
    if(strncmp("KEITHLEY INSTRUMENTS,MODEL 2460", inst_, 5))
    {
        printf("\n");
        for(int i = 0; i < 30; i++)
        {
            printf("%x", inst_[i]);
        }
        printf("\n");
        for(int i = 0; i < 30; i++)
        {
            printf("%x", kstring[i]);
        }
        printf("\n");

        printf("Device at address GPIB%i::%i::INSTR was not the expected device, exiting...\n", 1, 1);
        exit(1);
    }

    Beep(210, 0.25);
    Beep(400, 0.25);
    Beep(750, 0.25);
    Beep(750, 0.25);
    Beep(600, 0.25);

    sprintf(inst_, "OUTPUT:STATE OFF");
    if (Write(inst_))    {   printf("ERROR: Error setting output off\n"); }

    sprintf(inst_, "*CLS");
    if (Write(inst_))    {   printf("ERROR: Error clearing error buffer\n"); }

    sprintf(inst_, "*RST");
    if (Write(inst_))    {   printf("ERROR: Error setting reseting device\n"); }

    mwait(1000);

    sprintf(inst_, "OUTPUT1:CURRENT:SMODE HIMPEDANCE");
    if (Write(inst_))     {   printf("Failed to set current_t output off mode to High Impedance");  }
    sprintf(inst_, "OUTPUT1:VOLTAGE:SMODE HIMPEDANCE");
    if (Write(inst_))     {   printf("Failed to set voltage output off mode to High Impedance");  }
    sprintf(inst_, "OUTPUT:STATE OFF");
    if (Write(inst_))    {   printf("ERROR: Error setting output off\n"); }

    sprintf(inst_, "SENSE:CURRENT:RSENSE 1");
    if (Write(inst_))    {   printf("ERROR: Error setting current_t sense to 4 wire method\n"); }
    sprintf(inst_, "SENSE:VOLTAGE:RSENSE 1");
    if (Write(inst_))    {   printf("ERROR: Error setting voltage sense to 4 wire method\n"); }
    sprintf(inst_, "SOUR:CURR:HIGH:CAP ON");
    if (Write(inst_))    {   printf("ERROR: Error setting output high capacitance mode on\n"); }

    do
    {
        sprintf(inst_, "SYSTem:ERRor:NEXT?");
        Query(inst_, inst_);
        printf("%s\n", inst_);
    } while (atoi(inst_) != 0);

	printf("KeithleyInterface (V%.2f): started at %s", KEITHLEY_INTERFACE_VERSION, ctime(&t0));

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

    mwait(850);
    Beep(750, 0.25);
    Beep(500, 0.25);
    Beep(210, 0.25);
    Beep(210, 0.25);
    Beep(300, 0.25);

    sprintf(inst_, "OUTPUT:STATE OFF");
    Write(inst_);

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
    if (Ierr)   {   printf("Error reading output current_t\n");    }
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

    sprintf(inst_, "SOUR:FUNC VOLT");
    if(Write(inst_))   {printf("Error Setting output to voltage\n"); }
    SetVoltageRange(fabs(V));
    SetCurrentRange(fabs(I*10));
    sprintf(inst_, "SOUR:VOLT:ILIM %1.3e", I);
    if(Write(inst_))   {printf("Error Setting voltage current_t limit\n"); }
    sprintf(inst_, "SOUR:VOLT %2.3e", V);
    if(Write(inst_))   {printf("Error Setting voltage\n"); }

    sprintf(inst_, "*WAI");
    if(Write(inst_))   {printf("Error sending wait command\n"); }

    mwait(2);
    return 0.0f;
}

int KeithleyInterface::SMUCurrent(double voltage_max, double voltage_min, double current_t)
{
    #if DEBUG
	    printf("Call to KeithleyInterface::SMUCurrent\n");
        printf("V: %f, I: %f\n", voltage_max, current_t);
    #endif // DEBUG

    sprintf(inst_, "SOUR:FUNC CURR");
    if(Write(inst_))   {printf("Error Setting output to current_t\n"); }
    SetVoltageRange(fabs(voltage_max));
    SetCurrentRange(fabs(current_t));
    sprintf(inst_, "SOUR:CURR:VLIM %2.3e", voltage_max);
    if(Write(inst_))   {printf("Error Setting current_t voltage limit\n"); }
    sprintf(inst_, "SOUR:CURR %1.3e", current_t);
    if(Write(inst_))   {printf("Error Setting current_t\n"); }

    sprintf(inst_, "*WAI");
    if(Write(inst_))   {printf("Error sending wait command\n"); }

    mwait(2);
    return 0.0f;
}

double KeithleyInterface::SetVoltageRange(double V)
{
    char inst[256];
    char val[256];

    #if DEBUG
	    printf("Call to KeithleyInterface::SetVoltageRange\n");
    #endif // DEBUG

    double range = -1;
    if(V < 100e-6)
    {
        printf("\nInvalid voltage %f, unable to determine range\n", V);
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

    sprintf(inst_, "*WAI");
    //visa::wbstr(device, inst_);
    return range;
}

double KeithleyInterface::SetCurrentRange(double I)
{
    char inst[256];
    char val[256];

    #if DEBUG
        printf("Call to KeithleyInterface::SetVoltageRange\n");
    #endif // DEBUG

    double range = -1;
    if((I < 100e-9) && (I != 0.0f))
    {
        I = 0.0f;
        range = 1e-6;
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
    {
        printf("\nInvalid current_t %f, unable to determine range\n", I);
        range = -1;
    }

    if (range != -1)
    {
        sprintf(val, "SOURCE:CURRENT:RANGE %f", range);
        if (Write(val))    {   printf("Error setting current_t SOURCE range\n");    }

        sprintf(inst, "SOUR:FUNC?");
        if (Query(inst,val))    {   printf("Error reading current_t SOURCE type\n");    }

        if(abs(strncmp(val, "CURR", 4)))
        {
            sprintf(val, "SENSE:CURRENT:RANGE %f", range);
            if (Write(val))    {   printf("Error setting current_t SENSE range\n");    }
        }
    }

    sprintf(inst_, "*WAI");
    //visa::wbstr(device, inst_);
    return range;
}

int KeithleyInterface::IsCurrentLimited(void)
{
        // Query if supply is current_t limited, val is 1 if true.
    sprintf(inst_, "SOUR:VOLT:ILIM:TRIP?");
    if(Query(inst_, val_))    {   printf("ERROR: Error reading current_t trip limit\n"); }
    #if DEBUG
	    printf("%s", val_);
    #endif // DEBUG
    if(val_[0] == '1')
    {
        return 1;
    }
    return 0;
}

int KeithleyInterface::OutputOn(void)
{
    sprintf(inst_, "OUTPUT:STATE ON");
    return Write(inst_);
}

int KeithleyInterface::OutputOff(void)
{
    sprintf(inst_, "OUTPUT:STATE OFF");
    return Write(inst_);
}

int KeithleyInterface::ClearErrors()
{
    char buff[256];
    int error = 0;

    do // clear last errors if any remain
    {
        //visa::wbstr(device, ":SYSTem:ERRor:NEXT?");
        //visa::rbstr(device, buff, 255);
        sprintf(buff, ":SYSTem:ERRor:NEXT?");
        device_->Write(buff);
        device_->Read(buff);
    } while (atoi(buff) != 0);

    return 0;
}

int KeithleyInterface::CheckErrors()
{
    char buff[256];
    int error = 0;

    //visa::wbstr(device, ":SYSTem:ERRor:NEXT?");
    //visa::rbstr(device, buff, 255);

    sprintf(buff, ":SYSTem:ERRor:NEXT?");
    device_->Write(buff);
    device_->Read(buff);

    error = (atoi(buff) != 0);
    return error;

}

void KeithleyInterface::Beep(int frequency, double duration)
{
    char beep[255];
    sprintf(beep, ":SYSTem:BEEPer:IMMediate %i, %.3f", frequency, duration);
    device_->Write(beep);
    //visa::wbstr(device, beep);
    return;
}
