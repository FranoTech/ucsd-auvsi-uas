#include "StdAfx.h"
#include "PlaneWatcher.h"

using namespace Communications;

PlaneWatcher::PlaneWatcher(Object ^ theParent)
{ 
	parent = theParent; 
	//gpsInfo = new AutopilotInfo();
	gimbalInfo = new GimbalInfo();
	 
	autopilotInfo = gcnew array<AutopilotState *>(NUM_PLANE_DATA);  //create a new array with num entities
	autopilotInfoIndex = 0;  //point it to the front of the array
}

void PlaneWatcher::updateAutopilotInfo( AutopilotState *data)
{
	incrementAutopilotInfoIndex();  //increment it
	autopilotInfo[autopilotInfoIndex] = copyAutopilotState(data);

	// TODO: fix memory leak here. what happens to old struct? its just floating out in space, all alone, never to be used again ... :(

	// a good way to handle that would be to make a copyStruct function that took source struct and destination struct (just like memcpy())
	//	and copied one to the other. then, you would have alloc each element in the array, but after that, no more allocations. thats faster.
}
		
void PlaneWatcher::updateGimbalInfo( GimbalInfo *data)
{
	gimbalInfo->roll = data->roll;
	gimbalInfo->pitch = data->pitch;
}

		
AutopilotState * PlaneWatcher::predictLocationAtTime( float timeOffset )
{
	
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

AutopilotState *PlaneWatcher::copyAutopilotState(AutopilotState *state)
{
	AutopilotState *AI = new AutopilotState();  //create a new autopilot state

	memcpy(AI, state, sizeof(struct AutopilotState));

	return AI;
}

void PlaneWatcher::incrementAutopilotInfoIndex()
{
	autopilotInfoIndex ++;  //increment
	if(autopilotInfoIndex >= NUM_PLANE_DATA){
		autopilotInfoIndex = 0;  //reset to zero
	}
}

AutopilotState *PlaneWatcher::getClosestAutopilotState( float timeOffset )
{
	// TIM: does the UTC time of the packet refer to time gps was updated, or time the packet was sent?

	__int32 millisOffset = (__int32)(timeOffset * 1000.0);  //get the time in millis
	__int32 destTime = getTimeUTC(autopilotInfo[autopilotInfoIndex]) + millisOffset;  //get the current time, plus offset
	int i = autopilotInfoIndex;
	__int32 smallestDifference = 2147483646;  //maximum size of int32
	//XXX Todo: this stuff

	return 0;
}

__int32 PlaneWatcher::getTimeUTC(AutopilotState *state)
{
	__int32 time = 0;
	time += state->UTCmillisecond;
	time += state->UTCmin * 60000;
	time += state->UTChour * 3600000;
	time += state->UTCday * 86400000;

	return time;
}