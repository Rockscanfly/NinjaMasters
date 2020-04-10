/* Author: Vance Farrow
 * Edited VGF Sept 2019
 *
 * Based on bc8143.h by Jonathan Scott
*/

#ifndef VISAWRAPPER_H_INCLUDED
#define VISAWRAPPER_H_INCLUDED

#include <string>
#include    "../VISA/WinNT/Include/visa.h"

namespace visa
{
    // logs a message
    void msg(std::string message);

    // prints an error message and ends the program
    void err(std::string errorString);

    // wait some milliseconds in real-time work
    void mwait(int msecs);

    // open access to the VISA library on the computer
    void OpenVISA(ViPSession resmngr);

    // get device handle on a bus via VISA
    ViSession OpenDevice(ViSession defRM, char* addrstr);

    // set period in ms for bus to time out if stuck
    void setTMO(ViSession dev, int msecs);

    // send a message to a device
    ViUInt32 wbstr(ViSession dev, const char* cmd);

    // get message from an instrument/device
    ViUInt32 rbstr(ViSession dev, char* buf, ViUInt32 maxlen);
}

#endif // VISAWRAPPER_H_INCLUDED
