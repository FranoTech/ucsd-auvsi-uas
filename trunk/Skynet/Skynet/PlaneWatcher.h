#pragma once

#include "Comport.h"
#include "AutopilotComport.h"

using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;

#define NUM_GPS_DATA 100
#define NUM_TELEM_DATA 100
#define NUM_GIMBAL_DATA 100

#define CAMERA_LATENCY 125

namespace Communications
{


	public ref class PlaneWatcher {
	public:
		PlaneWatcher(Object ^ theParent);

		void updatePlaneGPSInfo(PlaneGPSPacket ^ data);
		void updatePlaneTelemInfo(PlaneTelemPacket ^ data);
		void updateGimbalInfo( GimbalInfo ^ data);

		float gimbalRollInDegrees();
		float gimbalPitchInDegrees();
		float rawToDegrees(unsigned __int16 input);
		float rawZoomToFloat(int theZoom);
		unsigned __int16 gimbalDegreesToRaw(float input);

		PlaneState ^ predictLocationAtTime( float timeOffset );
		PlaneState ^ stateOfCurrentImage();
		PlaneState ^ currentState() { return predictLocationAtTime(0); }

		void requiredRollPitchForGPS( unsigned __int16 & roll, unsigned __int16 & pitch, float lat, float lon);

		unsigned __int16 gimbalRoll;
		unsigned __int16 gimbalPitch;
		//unsigned __int32 zoom;
		int zoomLevel;
	private:
		Object ^ parent;

		array<GimbalInfo ^> ^ gimbalInfo;
		array<PlaneGPSPacket ^> ^ autopilotGPSInfo;
		array<PlaneTelemPacket ^> ^ autopilotTelemInfo;
		int gimbalInfoIndex;
		int autopilotGPSInfoIndex;
		int autopilotTelemInfoIndex;
		
		void incrementGimbalInfoIndex();
		void incrementGPSInfoIndex();
		void incrementTelemInfoIndex();

		__int32 getTimeUTC(PlaneGPSPacket ^ state);
		__int32 getTimeUTC(PlaneTelemPacket ^ state);
		__int32 getTimeUTC(GimbalInfo ^state);

		float linearInterpolation(float A, float B, __int32 timeA, __int32 timeB, __int32 timeActual);
	};

}