#include "StdAfx.h"
#include "PlaneWatcher.h"

using namespace Communications;

PlaneWatcher::PlaneWatcher(Object ^ theParent)
{ 
	 parent = theParent; 
	// gpsInfo = new GPSInfo();
	 gimbalInfo = new GimbalInfo();
}

void PlaneWatcher::updateGPSInfo( GPSInfo *data)
{
	// copy attributes over (do NOT do gpsInfo=data)

}
		
void PlaneWatcher::updateGimbalInfo( GimbalInfo *data)
{
	gimbalInfo->roll = data->roll;
	gimbalInfo->pitch = data->pitch;
}

		
GPSInfo * PlaneWatcher::predictLocationAtTime( float timeOffset )
{
	// TODO: Tim, implement this
	return 0;
}

float PlaneWatcher::gimbalRollInDegrees()
{
	
	//System::Diagnostics::Trace::WriteLine("gimbalRollInDegrees rollIn: " + gimbalInfo->roll + " rollOut: " + ((((float)gimbalInfo->roll) - 3000.0f)/20.0f));

	return (((float)gimbalInfo->roll) - 3000.0f)/20.0f;
}


float PlaneWatcher::gimbalPitchInDegrees()
{
	return (((float)gimbalInfo->pitch) - 3000.0f)/20.0f;

}