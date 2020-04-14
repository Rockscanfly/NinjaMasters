/* Author: Vance Farrow
 * Edited VGF Sept 2019
 *
 * Based on bc8143.h by Jonathan Scott
*/
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <time.h>
#include "visawrapper.h"
#include
namespace visa
{

#if DEBUG
const int kVisaDebug = false;
#else
const int kVisaDebug = false;
#endif // DEBUG

void msg(std::string message)
{
    std::clog << message;
//    std::cerr <<< std::endl;
}

void err(std::string errorString) // prints an error message and ends the program
{
    std::cerr << errorString;
//	viClose(defaultRM);
	exit(1);
}

void mwait(int msecs) // wait some milliseconds in real-time work
{
    clock_t fin = clock() + (clock_t)((CLOCKS_PER_SEC * (long)msecs)/1000.0L);
	if(msecs==0) return;
    while ( fin - clock() > 1L ) {};
	return;
}

void OpenVISA(ViPSession resmngr) // open access to the VISA library on the computer
{
	msg("Opening VISA session... ");
    if (VI_SUCCESS != viOpenDefaultRM(resmngr)) {
	    fprintf(stderr,"Cannot open resource for VISA session... aborting.\n");
		exit(1);
        }

	fprintf(stdout, "VISA session open\n");
	return;
}

ViSession OpenDevice(ViSession defRM, char* addrstr) // get device handle on a bus via VISA
{
	ViSession device;
	int returnval;

	// open VISA session to let us talk to device
	returnval = viOpen(defRM, addrstr, VI_NULL, VI_NULL, &device);
    if (returnval!=VI_SUCCESS) {
        fprintf(stderr,"\nProblem opening \"%s\":\n", addrstr);
		if (returnval==VI_ERROR_INV_SESSION)
			err( "Invalid SESSION.\n");
		else if (returnval==VI_ERROR_NSUP_OPER)
			err( "That operation not supported in this session.\n");
		else if (returnval==VI_ERROR_INV_RSRC_NAME)
			err( "Invalid resource reference %s.\n");
		else if (returnval==VI_ERROR_INV_ACC_MODE)
			err( "Invalid ACCESS mode.\n");
		else if (returnval==VI_ERROR_RSRC_NFOUND)
			err( "Insufficient location info/no resource found.\n");
		else if (returnval==VI_ERROR_ALLOC)
			err( "Insufficient system resources.\n");
		else
        	fprintf(stderr,"\nUnknown Error: %i\n", returnval);
		exit(1);
        }

	return(device);
}

void setTMO(ViSession dev, int msecs) // set perion in ms for bus to time out if stuck
{
	int returnval;
	returnval = viSetAttribute(dev, VI_ATTR_TMO_VALUE, (ViAttrState) msecs);

    if (returnval==VI_SUCCESS) return;

	fprintf(stderr,"Failure establishing timeout value (%d)...    \n", msecs);
	std::cerr << "Failure establishing timeout value" << msecs;
	if (returnval==VI_ERROR_INV_SESSION)
		err( "Invalid SESSION reference.\n");
	if (returnval==VI_ERROR_NSUP_ATTR)
		err( "This attribute is not supported.\n");
	if (returnval==VI_ERROR_NSUP_ATTR_STATE)
		err( "This attribute cannot be set to this value!\n");
	if (returnval==VI_ERROR_RSRC_LOCKED)
		err( "The session is LOCKED.\n");
	if (returnval==VI_ERROR_ATTR_READONLY)
		err( "The attribute is read only.\n");
	exit(1);
	return;
}

ViUInt32 wbstr(ViSession dev, const char* cmd) // send a message to a device
{
	ViUInt32 cnt=0L;
	ViStatus returnval;
	char msgst[64];

	returnval = viWrite(dev, (ViByte*)cmd, (ViUInt32)(strlen(cmd)), &cnt);
    if (returnval==VI_SUCCESS) {
		if (kVisaDebug) fprintf(stderr, "%s\n", cmd); // debugging only
		return(cnt);
	}
	// there was a problem
	strncpy(msgst, cmd, 60);
    fprintf(stderr,"Failure writing: %s\n", msgst);
	if (returnval==VI_ERROR_INV_SESSION)
		err( "Invalid SESSION reference.\n");
	if (returnval==VI_ERROR_NSUP_OPER)
		err( "This type of session cannot do a viWrite.\n");
	if (returnval==VI_ERROR_RSRC_LOCKED)
		err( "The session is LOCKED.\n");
	if (returnval==VI_ERROR_TMO)
		err( "The session TIMED OUT.\n");
	if (returnval==VI_ERROR_RAW_WR_PROT_VIOL)
		err( "Violaton of RAW Write protocol occurred.\n");
	if (returnval==VI_ERROR_RAW_RD_PROT_VIOL)
		err( "Violaton of RAW Read protocol occurred.\n");
	if (returnval==VI_ERROR_INP_PROT_VIOL)
		err( "Dev reports Violaton of INPUT protocol occurred.\n");
	if (returnval==VI_ERROR_BERR)
		err( "Bus error occurred.\n");
	if (returnval==VI_ERROR_INV_SETUP)
		err( "Setup invalid (inconsistent attributes?).\n");
	if (returnval==VI_ERROR_NCIC)
		err( "I am not the bus controller.\n");
	if (returnval==VI_ERROR_NLISTENERS)
		err( "No listeners....\n");
	if (returnval==VI_ERROR_IO)
		err( "Unknown IO ERROR occurred.\n");
	err("Aborting...\n");
	return(cnt);
}

ViUInt32 rbstr(ViSession dev, char* buf, ViUInt32 maxlen) // get message from an instrument/device
{
	ViUInt32 cnt=0;
	int returnval;
	int tries=0;
	int maxtries=2;
	char deb[64];

	while(++tries<maxtries) {
		returnval = viRead(dev, (ViByte*)buf, maxlen, &cnt);

    	if (returnval==VI_SUCCESS_MAX_CNT) {
			if(maxlen>=16) fprintf(stderr,"Warning: viRead filled %ld byte buffer!\n",maxlen);
			// fprintf(stderr,".");
			// fprintf(stderr, "%s\n", buf);
			buf[cnt]='\0';
			if(kVisaDebug) {
				strncpy(deb,buf,64); deb[63]='\0';
				fprintf(stderr,"Read:%s...", deb);
			}
			break;
		}
    	if (returnval==VI_SUCCESS_TERM_CHAR || returnval==VI_SUCCESS) {
			buf[cnt]='\0';
			if(kVisaDebug) {
				strncpy(deb,buf,64); deb[63]='\0';
				fprintf(stderr,"Read:%s", deb);
			}
			break;
		}

		msg("\nFailure reading in viRead statement...                      \n");
		if (returnval==VI_ERROR_INV_SESSION)
			msg( "Invalid SESSION reference.\n");
		if (returnval==VI_ERROR_NSUP_OPER)
			msg( "This type of session cannot do a viRead.\n");
		if (returnval==VI_ERROR_RSRC_LOCKED)
			msg( "The session is LOCKED.\n");
		if (returnval==VI_ERROR_TMO)
			msg( "The session TIMED OUT.\n");
		if (returnval==VI_ERROR_RAW_WR_PROT_VIOL)
			msg( "Violaton of RAW Write protocol occurred.\n");
		if (returnval==VI_ERROR_RAW_RD_PROT_VIOL)
			msg( "Violaton of RAW Read protocol occurred.\n");
		if (returnval==VI_ERROR_OUTP_PROT_VIOL)
			msg( "Dev reports Violaton of OUTPUT protocol occurred.\n");
		if (returnval==VI_ERROR_BERR)
			msg( "Bus error occurred.\n");
		if (returnval==VI_ERROR_INV_SETUP)
			msg( "Setup invalid (inconsistent attributes?).\n");
		if (returnval==VI_ERROR_NCIC)
			msg( "I am not the bus controller.\n");
		if (returnval==VI_ERROR_NLISTENERS)
			msg( "No listeners....\n");
		if (returnval==VI_ERROR_IO)
			msg( "Unknown IO ERROR occurred.\n");
//		mwait(3000);
	}
	if(tries<maxtries) return(cnt);
	else err("Giving up on viRead.                 \n");
	return(cnt); // should never get here
}

}
