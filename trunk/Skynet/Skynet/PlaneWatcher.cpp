#include "StdAfx.h"
#include "PlaneWatcher.h"
#include "Form1.h"

using namespace Communications;

PlaneWatcher::PlaneWatcher(Object ^ theParent)
{ 
	parent = theParent; 

    gimbalInfo = gcnew array<GimbalInfo *>(NUM_GIMBAL_DATA);  //create a new array with num entities
	autopilotGPSInfo = gcnew array<PlaneGPSPacket ^>(NUM_GPS_DATA);
	autopilotTelemInfo = gcnew array<PlaneTelemPacket ^>(NUM_TELEM_DATA);

	gimbalInfoIndex  = 0;  //point it to the front of the array
	autopilotGPSInfoIndex = 0;
	autopilotTelemInfoIndex = 0;

	for(int a = 0; a < NUM_GIMBAL_DATA; a++){
		gimbalInfo[a] = 0;
	}

	for(int a = 0; a < NUM_GPS_DATA; a++){
		autopilotGPSInfo[a] = nullptr;
	}

	for(int a = 0; a < NUM_TELEM_DATA; a++){
		autopilotTelemInfo[a] = nullptr;
	}
}
		
void PlaneWatcher::updateGimbalInfo( GimbalInfo *data)
{
	incrementGimbalInfoIndex();

	delete gimbalInfo[gimbalInfoIndex];

	gimbalInfo[gimbalInfoIndex] = data;
	
	((Skynet::Form1 ^)parent)->reloadTable();
}

void PlaneWatcher::updatePlaneGPSInfo( PlaneGPSPacket ^ data)
{
	incrementGPSInfoIndex();

	delete autopilotGPSInfo[autopilotGPSInfoIndex];

	autopilotGPSInfo[autopilotGPSInfoIndex] = data;

	((Skynet::Form1 ^)parent)->reloadTable();
}

void PlaneWatcher::updatePlaneTelemInfo( PlaneTelemPacket ^ data)
{
	incrementTelemInfoIndex();

	delete autopilotTelemInfo[autopilotTelemInfoIndex];

	autopilotTelemInfo[autopilotTelemInfoIndex] = data;
	
	((Skynet::Form1 ^)parent)->reloadTable();
}

		
/*PlaneState * PlaneWatcher::predictLocationAtTime( float timeOffset )
{
	//TODO:  that's right, something else for Todo to do.
	//System::Diagnostics::Trace::WriteLine("PlaneWatcher::predictLocationAtTime()");
	return getClosestAutopilotState(timeOffset);
}*/

PlaneState ^ PlaneWatcher::predictLocationAtTime( float timeOffset )
{
	System::DateTime targetTime = System::DateTime::Now.AddSeconds(timeOffset);

	__int32 target = targetTime.Millisecond + 1000*targetTime.Second + 60000*targetTime.Minute + 3600000*targetTime.Hour;  //time in __int32 which we want to compare to
	
	/*
	 * cases: 
	 * (1) empty list, set defaults
	 * (2) list of size 1, do 1 point prediction
	 * (3) before the most recent GPS location
	 * (4) within the list of points
	 * (5) after the oldest GPS location
	 *
	 * remember to discount any array pointer values of 0, as these have not been instantiated 
	 */
	
	PlaneState ^ retval = gcnew PlaneState();

	//we know that we can get prediction data for both GPS and telem
	//do GPS prediction
	
	//////////////////// GPS info: lat, lon, alt /////////////////////////

	//for now, just returning the two closest values
	__int32 smallestDistance = 3600000;  //arbitrarily large
	int smallestIndex = -1;
	int numValidGPSPackets = 0;
	for(int i = 0; i < NUM_GPS_DATA; i++) {
		if(autopilotGPSInfo[i] != nullptr){
			numValidGPSPackets ++;
			__int32 dist = abs(target - getTimeUTC(autopilotGPSInfo[i]));
			//System::Diagnostics::Trace::WriteLine("PlaneWatcher::getClosestAutopilotState() dist:" + dist + " target:" + target);
			if(dist < smallestDistance){
				smallestDistance = dist;
				smallestIndex = i;
			}
		}
	}

	// if no packet found
	if (smallestIndex == -1) 
	{
		retval->gpsData->gpsVelocity = 20.0f;
		retval->gpsData->gpsAltitude = 37;
		retval->gpsData->gpsHeading = 0.0f;
		retval->gpsData->gpsLatitude = 32.0f;
		retval->gpsData->gpsLongitude = -117.0f;
	}

	// if packet found, but only one
	else if(numValidGPSPackets == 1)
	{
		retval->gpsData->gpsVelocity = autopilotGPSInfo[smallestIndex]->gpsVelocity;
		retval->gpsData->gpsAltitude = autopilotGPSInfo[smallestIndex]->gpsAltitude;
		retval->gpsData->gpsHeading = autopilotGPSInfo[smallestIndex]->gpsHeading;
		retval->gpsData->gpsLatitude = autopilotGPSInfo[smallestIndex]->gpsLatitude;
		retval->gpsData->gpsLongitude = autopilotGPSInfo[smallestIndex]->gpsLongitude;
	}

	//if we have at least two packets to chose from
	else
	{ 
		//if we have another packet, check to see which one is closer, the one before this or the one after
		int indexAfter = smallestIndex + 1;
		if(indexAfter >= NUM_GPS_DATA){
			indexAfter = 0;
		}

		int indexBefore = smallestIndex - 1;
		if(indexBefore < 0){
			indexBefore = NUM_GPS_DATA - 1;
		}

		__int32 distAfter = abs(target - getTimeUTC(autopilotGPSInfo[indexAfter]));
		__int32 distBefore = abs(target - getTimeUTC(autopilotGPSInfo[indexBefore]));

		PlaneGPSPacket ^ olderGPSPacket;
	    PlaneGPSPacket ^ newerGPSPacket;

		if(autopilotGPSInfo[indexAfter] == nullptr || distAfter > distBefore)  //use the olderPacket
		{  
			olderGPSPacket = autopilotGPSInfo[indexBefore];
			newerGPSPacket = autopilotGPSInfo[smallestIndex];
		}
		else  //use the newerPacket
		{
			olderGPSPacket = autopilotGPSInfo[smallestIndex];
			newerGPSPacket = autopilotGPSInfo[indexAfter];
		}

		//now use the two packets and do a linear math thingy

		__int32 olderTime =  getTimeUTC(olderGPSPacket);
		__int32 newerTime =  getTimeUTC(newerGPSPacket);

		retval->gpsData->gpsVelocity  = linearInterpolation(olderGPSPacket->gpsVelocity, newerGPSPacket->gpsVelocity, olderTime, newerTime, target);
		retval->gpsData->gpsAltitude  = linearInterpolation(olderGPSPacket->gpsAltitude, newerGPSPacket->gpsAltitude, olderTime, newerTime, target);
		retval->gpsData->gpsHeading   = linearInterpolation(olderGPSPacket->gpsHeading, newerGPSPacket->gpsHeading, olderTime, newerTime, target);
		retval->gpsData->gpsLatitude  = linearInterpolation(olderGPSPacket->gpsLatitude, newerGPSPacket->gpsLatitude, olderTime, newerTime, target);
		retval->gpsData->gpsLongitude = linearInterpolation(olderGPSPacket->gpsLongitude, newerGPSPacket->gpsLongitude, olderTime, newerTime, target);
	}
	
	//////////////////// Telemetry: roll, pitch, heading, etc /////////////////////////
	smallestDistance = 3600000;  //arbitrarily large
	smallestIndex = -1;
	int numValidTelemPackets = 0;
	for(int i = 0; i < NUM_TELEM_DATA; i++){
		if(autopilotTelemInfo[i] != nullptr){
			numValidTelemPackets ++;
			__int32 dist = abs(target - getTimeUTC(autopilotTelemInfo[i]));
			if(dist < smallestDistance){
				smallestDistance = dist;
				smallestIndex = i;
			}
		}
	}

	// if no packet found
	if (smallestIndex == -1) 
	{
		retval->telemData->roll = 0;
		retval->telemData->pitch = 0;
		retval->telemData->heading = 0;
	}

	// if packet found, but only one
	else if(numValidTelemPackets == 1)
	{
		retval->telemData->roll    = autopilotTelemInfo[smallestIndex]->roll;
		retval->telemData->pitch   = autopilotTelemInfo[smallestIndex]->pitch;
		retval->telemData->heading = autopilotTelemInfo[smallestIndex]->heading;
	}

	//if we have at least two packets to chose from
	else
	{ 
		//if we have another packet, check to see which one is closer, the one before this or the one after
		int indexAfter = smallestIndex + 1;
		if(indexAfter >= NUM_TELEM_DATA){
			indexAfter = 0;
		}

		int indexBefore = smallestIndex - 1;
		if(indexBefore < 0){
			indexBefore = NUM_TELEM_DATA - 1;
		}

		__int32 distAfter = abs(target - getTimeUTC(autopilotTelemInfo[indexAfter]));
		__int32 distBefore = abs(target - getTimeUTC(autopilotTelemInfo[indexBefore]));

		PlaneTelemPacket ^ olderGPSPacket;
	    PlaneTelemPacket ^ newerGPSPacket;

		if(autopilotTelemInfo[indexAfter] == nullptr || distAfter > distBefore)  //use the olderPacket
		{  
			olderGPSPacket = autopilotTelemInfo[indexBefore];
			newerGPSPacket = autopilotTelemInfo[smallestIndex];
		}
		else  //use the newerPacket
		{
			olderGPSPacket = autopilotTelemInfo[smallestIndex];
			newerGPSPacket = autopilotTelemInfo[indexAfter];
		}

		//now use the two packets and do a linear math thingy

		__int32 olderTime =  getTimeUTC(olderGPSPacket);
		__int32 newerTime =  getTimeUTC(newerGPSPacket);

		retval->telemData->roll    = linearInterpolation(olderGPSPacket->roll, newerGPSPacket->roll, olderTime, newerTime, target);
		retval->telemData->pitch   = linearInterpolation(olderGPSPacket->pitch, newerGPSPacket->pitch, olderTime, newerTime, target);
		retval->telemData->heading = linearInterpolation(olderGPSPacket->heading, newerGPSPacket->heading, olderTime, newerTime, target);
	}

	
	//////////////////// Gimbal Info /////////////////////////
	smallestDistance = 3600000;  //arbitrarily large
	smallestIndex = -1;
	int numValidGimbalPackets = 0;
	for(int i = 0; i < NUM_TELEM_DATA; i++){
		if(gimbalInfo[i] != 0){
			numValidGimbalPackets ++;
			__int32 dist = abs(target - getTimeUTC(gimbalInfo[i]));
			if(dist < smallestDistance){
				smallestDistance = dist;
				smallestIndex = i;
			}
		}
	}

	// if no packet found
	if (smallestIndex == -1) 
	{
		retval->gimbalInfo->zoom = 1;
		retval->gimbalInfo->pitch = 0;
	}

	// if packet found, but only one
	else if(numValidTelemPackets == 1)
	{
		retval->gimbalInfo->zoom = gimbalInfo[gimbalInfoIndex]->zoom;
		retval->gimbalInfo->pitch = gimbalInfo[gimbalInfoIndex]->pitch;
	}

	//if we have at least two packets to chose from
	else
	{ 
		//if we have another packet, check to see which one is closer, the one before this or the one after
		int indexAfter = smallestIndex + 1;
		if(indexAfter >= NUM_GPS_DATA){
			indexAfter = 0;
		}

		int indexBefore = smallestIndex - 1;
		if(indexBefore < 0){
			indexBefore = NUM_GPS_DATA - 1;
		}

		__int32 distAfter = abs(target - getTimeUTC(gimbalInfo[indexAfter]));
		__int32 distBefore = abs(target - getTimeUTC(gimbalInfo[indexBefore]));

		GimbalInfo * olderGPSPacket;
	    GimbalInfo * newerGPSPacket;

		if(autopilotTelemInfo[indexAfter] == nullptr || distAfter > distBefore)  //use the olderPacket
		{  
			olderGPSPacket = gimbalInfo[indexBefore];
			newerGPSPacket = gimbalInfo[smallestIndex];
		}
		else  //use the newerPacket
		{
			olderGPSPacket = gimbalInfo[smallestIndex];
			newerGPSPacket = gimbalInfo[indexAfter];
		}

		//now use the two packets and do a linear math thingy

		__int32 olderTime =  getTimeUTC(olderGPSPacket);
		__int32 newerTime =  getTimeUTC(newerGPSPacket);

		retval->gimbalInfo->roll    = linearInterpolation(olderGPSPacket->roll, newerGPSPacket->zoom, olderTime, newerTime, target);
		retval->gimbalInfo->pitch   = linearInterpolation(olderGPSPacket->pitch, newerGPSPacket->pitch, olderTime, newerTime, target);
	}

	return retval;
}

float PlaneWatcher::gimbalRollInDegrees()
{
	// TODO: store time of GimbalInfo packets, and access it when needed. include in PlaneState.
	
	if (gimbalInfo[gimbalInfoIndex] == 0)
		return 0;

	return (((float)gimbalInfo[gimbalInfoIndex]->roll) - 3000.0f)/20.0f;
}


float PlaneWatcher::gimbalPitchInDegrees()
{
	// TODO: store time of GimbalInfo packets, and access it when needed. include in PlaneState.
	
	if (gimbalInfo[gimbalInfoIndex] == 0)
		return 0;

	return (((float)gimbalInfo[gimbalInfoIndex]->pitch) - 3000.0f)/20.0f;

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

__int32 PlaneWatcher::getTimeUTC(PlaneGPSPacket ^ state)
{
	if (state == nullptr)
		return 0;

	__int32 time = 0;
	time += (__int32)state->UTCmillisecond;
	time += (__int32)(state->UTCmin) * 60000;
	time += (__int32)(state->UTChour) * 3600000;
	
	//System::Diagnostics::Trace::WriteLine("PlaneWatcher::getTimeUTC() time:" + time);
			

	return time;
}

__int32 PlaneWatcher::getTimeUTC(PlaneTelemPacket ^ state)
{
	if (state == nullptr)
		return 0;

	__int32 time = 0;
	time += (__int32)state->UTCmillisecond;
	time += (__int32)(state->UTCmin) * 60000;
	time += (__int32)(state->UTChour) * 3600000;
	
	//System::Diagnostics::Trace::WriteLine("PlaneWatcher::getTimeUTC() time:" + time + " mill:" + state->UTCmillisecond + " min:" + state->UTCmin + " hr:" + state->UTChour);

	return time;
}

__int32 PlaneWatcher::getTimeUTC(GimbalInfo *state){
	if (state == nullptr)
		return 0;

	__int32 time = 0;
	time += (__int32)state->UTCmillisecond;
	time += (__int32)(state->UTCmin) * 60000;
	time += (__int32)(state->UTChour) * 3600000;
	
	return time;
}

float PlaneWatcher::linearInterpolation(float A, float B, __int32 timeA, __int32 timeB, __int32 timeActual)
{
	return A + (B - A) * (timeActual - timeA) / (timeB - timeA);
}