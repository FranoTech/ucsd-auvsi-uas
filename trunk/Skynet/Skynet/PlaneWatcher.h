#pragma once

#include "Comport.h"
#include "AutopilotComport.h"

using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;

#define NUM_PLANE_DATA 100

namespace Communications
{

	ref class PlaneWatcher {
	public:
		PlaneWatcher(Object ^ theParent);

		void updateAutopilotInfo( AutopilotState *data);
		void updateGimbalInfo( GimbalInfo *data);

		float gimbalRollInDegrees();
		float gimbalPitchInDegrees();

		AutopilotState *predictLocationAtTime( float timeOffset );

	private:
		Object ^ parent;
		array<AutopilotState *> ^ autopilotInfo;
		int autopilotInfoIndex;

		GimbalInfo *gimbalInfo;

		AutopilotState *copyAutopilotState(AutopilotState *state);
		void incrementAutopilotInfoIndex();
		AutopilotState *getClosestAutopilotState( float timeOffset );  //time offset in seconds
		__int32 getTimeUTC(AutopilotState *state);
	};

}