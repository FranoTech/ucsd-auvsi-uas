#pragma once

#include "Comport.h"
#include "AutopilotComport.h"

using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;

#define NUM_GPS_DATA 100
#define NUM_TELEM_DATA 100
#define NUM_GIMBAL_DATA 100

namespace Communications
{


	ref class PlaneWatcher {
	public:
		PlaneWatcher(Object ^ theParent);

		void updatePlaneGPSInfo(PlaneGPSPacket * data);
		void updatePlaneTelemInfo(PlaneTelemPacket * data);
		void updateGimbalInfo( GimbalInfo *data);

		float gimbalRollInDegrees();
		float gimbalPitchInDegrees();

		AutopilotState *predictLocationAtTime( float timeOffset );

	private:
		Object ^ parent;

		array<GimbalInfo *> ^ gimbalInfo;
		array<PlaneGPSPacket *> ^ autopilotGPSInfo;
		array<PlaneTelemPacket *> ^ autopilotTelemInfo;
		int gimbalInfoIndex;
		int autopilotGPSInfoIndex;
		int autopilotTelemInfoIndex;
		
		void incrementGimbalInfoIndex();
		void incrementGPSInfoIndex();
		void incrementTemelInfoIndex();

		AutopilotState *getClosestAutopilotState( float timeOffset );  //time offset in seconds
		__int32 getTimeUTC(PlaneGPSPacket *state);
		__int32 getTimeUTC(PlaneTelemPacket *state);
	};

}