#pragma once

#include "Comport.h"

using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;

namespace Communications
{


	ref class PlaneWatcher {
	public:
		PlaneWatcher(Object ^ theParent);

		void updateGPSInfo( GPSInfo *data);
		void updateGimbalInfo( GimbalInfo *data);

		float gimbalRollInDegrees();
		float gimbalPitchInDegrees();

		GPSInfo *predictLocationAtTime( float timeOffset );

	private:
		Object ^ parent;
		GPSInfo *gpsInfo;
		GimbalInfo *gimbalInfo;


	};

}