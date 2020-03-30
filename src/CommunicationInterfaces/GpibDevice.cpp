#include "GpibDevice.hpp"

GpibDevice::GpibDevice( int major, int minor )
{
    #if DEBUG
        fprintf( stderr, "Start of GpibDevice Constructor\n" );
    #endif // DEBUG

    char busname[256];

    strcpy( busname, "GPIB" );
    sprintf( busname, "%s%d::%d::INSTR", busname, major, minor ); //open session to device
    printf( busname );
    printf( "\n" );
    if( major > 255 || major < 0 ) visa::err( "Bad GPIB major number given" );
    if( minor > 32 || minor < 0 )  visa::err( "Bad GPIB minor number given" );

    visa::OpenVISA( &resourceManager ); //create VISA session (PC -> VISA)
    device = visa::OpenDevice( resourceManager, busname ); //connect session identifier to Psu (VISA -> device)
    visa::setTMO( device, 9000 );

    #if DEBUG
        fprintf( stderr, "End of GpibDevice Constructor\n" );
    #endif // DEBUG
}

GpibDevice::~GpibDevice( void )
{
    #if DEBUG
        fprintf( stderr, "Start of GpibDevice Destructor\n" );
    #endif // DEBUG

    #if DEBUG
        fprintf( stderr, "End of GpibDevice Destructor\n" );
    #endif // DEBUG
}



int GpibDevice::Read( char *data )
{
    #if DEBUG
        fprintf( stderr, "GpibDevice::Read\n" );
    #endif // DEBUG

    int count = 0;
    count = visa::rbstr( device, data, 255 );

    return count;
}

int GpibDevice::Write( char *data )
{
    #if DEBUG
        fprintf( stderr, "GpibDevice::Write\n" );
    #endif // DEBUG
    int count = 0;
    count = visa::wbstr( device, data );
    return count;
}


