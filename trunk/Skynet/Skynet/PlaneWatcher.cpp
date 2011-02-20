#include "StdAfx.h"
#include "PlaneWatcher.h"
//#include "Form1.h"

using namespace Communications;

PlaneWatcher::PlaneWatcher(Object ^ theParent)
{ 
	parent = theParent; 

	array<GimbalInfo *> ^ gimbalInfo;
		array<PlaneGPSPacket *> ^ autopilotGPSInfo;
		array<PlaneTelemPacket *> ^ autopilotTelemInfo;
		int gimbalInfoIndex;
		int autopilotGPSInfoIndex;
		int autopilotTelemInfoIndex;

    gimbalInfo = gcnew array<GimbalInfo *>(NUM_GIMBAL_DATA);  //create a new array with num entities
	autopilotGPSInfo = gcnew array<PlaneGPSPacket *>(NUM_GPS_DATA);
	autopilotTelemInfo = gcnew array<PlaneTelemPacket *>(NUM_TELEM_DATA);

	gimbalInfoIndex  = 0;  //point it to the front of the array
	autopilotGPSInfoIndex = 0;
	autopilotTelemInfoIndex = 0;

	for(int a = 0; a < NUM_GIMBAL_DATA; a++){
		gimbalInfo[a] = 0;
	}

	for(int a = 0; a < NUM_GPS_DATA; a++){
		autopilotGPSInfo[a] = 0;
	}

	for(int a = 0; a < NUM_TELEM_DATA; a++){
		autopilotTelemInfo[a] = 0;
	}
}
		
void PlaneWatcher::updateGimbalInfo( GimbalInfo *data)
{
	incrementGimbalInfoIndex();

	delete gimbalInfo[gimbalInfoIndex];

	gimbalInfo[gimbalInfoIndex] = data;
}

void PlaneWatcher::updatePlaneGPSInfo( PlaneGPSPacket * data)
{
	incrementGPSInfoIndex();

	delete autopilotGPSInfo[autopilotGPSInfoIndex];

	autopilotGPSInfo[autopilotGPSInfoIndex] = data;
}

void PlaneWatcher::updatePlaneTelemInfo( PlaneTelemPacket * data)
{
	incrementTemelInfoIndex();

	delete autopilotTelemInfo[autopilotTelemInfoIndex];

	autopilotTelemInfo[autopilotTelemInfoIndex] = data;
}

		
AutopilotState * PlaneWatcher::predictLocationAtTime( float timeOffset )
{
	//TODO:  that's right, something else for Todo to do.
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

void PlaneWatcher::incrementGimbalInfoIndex()
{
	gimbalInfoIndex ++;  //increment
	if(gimbalInfoIndex >= NUM_GIMBAL_DATA){
		gimbalInfoIndex = 0;  //reset to zero
	}
}

void PlaneWatcher::incrementGPSInfoIndex()
{
	autopilotGPSInfoIndex ++;  //increment
	if(autopilotGPSInfoIndex >= NUM_GPS_DATA){
		autopilotGPSInfoIndex = 0;  //reset to zero
	}
}

void PlaneWatcher::incrementTelemInfoIndex()
{
	autopilotTelemInfoIndex ++;  //increment
	if(autopilotTelemInfoIndex >= NUM_TELEM_DATA){
		autopilotTelemInfoIndex = 0;  //reset to zero
	}
}

PlaneState *PlaneWatcher::getClosestAutopilotState( float timeOffset )
{
	
	System::DataTime targetTime = System::DateTime::Now.AddSeconds(timeOffset);

	__int32 target = targetTime.Millisecond + 1000*targetTime.Second + 60000*targetTime.Minute + 3600000*targetTime.Hour;  //time in __int32 which we want to compare to
	
	/*
	 * cases: 
	 * (1) empty list, return 0
	 * (2) list of size 1, do 1 point prediction
	 * (3) before the most recent GPS location
	 * (4) within the list of points
	 * (5) after the oldest GPS location
	 *
	 * remember to discount any array pointer values of 0, as these have not been instantiated 
	 */

	//case 1 for GPS and Telem
	if(autopilotGPSInfoIndex == 0 && autopilotGPSInfo[autopilotGPSInfoIndex] == 0 || autopilotTelemInfoIndex == 0 && autopilotTelemInfo[autopilotTelemInfoIndex] == 0){
		return 0;
	}

	PlaneState * retval = new PlaneState();

	//we know that we can get prediction data for both GPS and telem
	//do GPS prediction

	//for now, just returning the two closest values
	__int32 smallestDistance = 3600000;  //arbitrarily large
	int smallestIndex = 0;
	for(int i = 0; i < NUM_GPS_DATA; i++){
		if(autopilotGPSInfo[i] ! = 0){
			__int32 dist = abs(timeOffset - getTimeUTC(autopilotGPSInfo[i]));
			if(dist < smallestDistace){
				smallestDistance = dist;
				smallestIndex = i;
			}
		}
	}

	retval->gpsData->gpsVelocity = autopilotGPSInfo[smallestIndex]->gpsVelocity;
	retval->gpsData->gpsAltitude = autopilotGPSInfo[smallestIndex]->gpsAltitude;
	retval->gpsData->gpsHeading = autopilotGPSInfo[smallestIndex]->gpsHeading;
	retval->gpsData->gpsLatitude = autopilotGPSInfo[smallestIndex]->gpsLatitude;
	retval->gpsData->gpsLongitude = autopilotGPSInfo[smallestIndex]->gpsLongitude;

	smallestDistance = 3600000;  //arbitrarily large
	smallestIndex = 0;
	for(int i = 0; i < NUM_TELEM_DATA; i++){
		if(autopilotTelemInfo[i] ! = 0){
			__int32 dist = abs(timeOffset - getTimeUTC(autopilotTelemInfo[i]));
			if(dist < smallestDistace){
				smallestDistance = dist;
				smallestIndex = i;
			}
		}
	}

	retval->telemData->roll = autopilotTelemInfo[smallestIndex]->roll;
	retval->telemData->pitch = autopilotTelemInfo[smallestIndex]->pitch;
	retval->telemData->heading = autopilotTelemInfo[smallestIndex]->heading;

	return retval;
}

__int32 PlaneWatcher::getTimeUTC(PlaneGPSPacket *state)
{
	__int32 time = 0;
	time += state->UTCmillisecond;
	time += state->UTCmin * 60000;
	time += state->UTChour * 3600000;

	return time;
}

__int32 PlaneWatcher::getTimeUTC(PlaneTelemPacket *state)
{
	__int32 time = 0;
	time += state->UTCmillisecond;
	time += state->UTCmin * 60000;
	time += state->UTChour * 3600000;

	return time;
}