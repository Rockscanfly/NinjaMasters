#include <Windows.h>
#include <stdio.h>
#include <string.h>

#define _countof(array) (sizeof(array) / sizeof(array[0]))

static HANDLE hComm;  // Handle for the current opened serial port.

static void SerialPort_Open()
{
    char portName[10] = { 0 };
    char portNum[20] = { 0 };

    printf("Please enter the COM port to connect to and press the ENTER key.\nThe format is ""COMX"", where X is the number of the COM port:");
    scanf("%s", portName, (unsigned)_countof(portName));
    snprintf(portNum, 20,"\\\\.\\%s", portName);

    hComm = CreateFile(portNum,             // Friendly name
        GENERIC_READ | GENERIC_WRITE,       // Read/Write Access
        0,                                  // No Sharing, ports can't be shared
        NULL,                               // No Security
        OPEN_EXISTING,                      // Open existing port only
        0,                                  // Non Overlapped I/O
        NULL);                              // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE)
    {
        printf("\nError, cannot open COM port.\n\n");
        system("pause");
    }
}

static void SerialPort_SetParams()
{
    BOOL status;
    DCB dcbSerialParams = { 0 };

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    status = GetCommState(hComm, &dcbSerialParams); // Get the current settings.
    if (status == FALSE)
    {
        printf("\nError getting the current COM port state.\n\n");
        CloseHandle(hComm);
    }

    // TODO: Adjust these for the device connected to the serial port.
    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    status = SetCommState(hComm, &dcbSerialParams);
    if (status == FALSE)
    {
        printf("\nError setting COM port state.\n\n");
        CloseHandle(hComm);
    }
}

static void SerialPort_SetTimeouts()
{
    BOOL status;
    COMMTIMEOUTS timeouts = { 0 };

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    status = SetCommTimeouts(hComm, &timeouts);

    if (status == FALSE)
    {
        printf("\nError setting COM port timeouts.\n\n");
        CloseHandle(hComm);
    }
}

static void SerialPort_TxTask()
{
    BOOL status;
    char txBuffer[256] = { 0 };
    DWORD bytesWritten = 0;
    DWORD dwEventMask;

    printf("\n\nEnter message to send: ");
    scanf("%s", txBuffer, (unsigned)_countof(txBuffer));

    // NOTE: Adds a new line character to the end of every string sent.
    strcat(txBuffer, "\n");

    // Writing data out to serial port.
    status = WriteFile(hComm, txBuffer, strlen(txBuffer), &bytesWritten, NULL);
    if (status == FALSE)
    {
        printf("\nError failed to write to COM port.");
        CloseHandle(hComm);
    }

    printf("\nNumber of bytes written = %d bytes.\n\n", (int)bytesWritten);

    // Setting Rx mask
    status = SetCommMask(hComm, EV_RXCHAR);
    if (status == FALSE)
    {
        printf("\nError setting the Rx mask.\n\n");
        CloseHandle(hComm);
    }

    // Setting wait event
    status = WaitCommEvent(hComm, &dwEventMask, NULL); // Wait for the character to be received
    if (status == FALSE)
    {
        printf("\nError setting Rx wait event.\n\n");
        CloseHandle(hComm);
    }
}

static void SerialPort_RxTask()
{
    BOOL status;
    int i = 0;
    char rxBuffer[256] = { 0 };
    char  rxChar;
    DWORD bytesRead;

    do
    {
        status = ReadFile(hComm, &rxChar, sizeof(rxChar), &bytesRead, NULL);
        rxBuffer[i] = rxChar;
        ++i;
    } while (bytesRead > 0);
    --i;

    printf("\nNumber of bytes received = %d bytes.\n\n", i);

    // Print the received data to the console.
    printf("\n\n");
    printf("Got reply:\n");
    int index = 0;
    for (index = 0; index < i; ++index)
    {
        printf("%c", rxBuffer[index]);
    }
    printf("\n\n");
}

int main(void)
{
    SerialPort_Open();
    SerialPort_SetParams();
    SerialPort_SetTimeouts();

    while (1)
    {
        SerialPort_TxTask();
        SerialPort_RxTask();
    }

    return 0;
}
