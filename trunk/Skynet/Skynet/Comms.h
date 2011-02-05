#pragma once

#include "Comport.h"
#include "AutopilotComport.h"
#include "RabbitComport.h"

#define BOTH_CONNECTED			3
#define RABBIT_CONNECTED		1
#define AUTOPILOT_CONNECTED		2
#define BOTH_FAILED				0

using namespace System::IO::Ports;
using namespace System;
using namespace System::Runtime::InteropServices;

namespace Communications
{

	
	delegate void comportUpdateDelegate( ComportDownstream * data );
	delegate void tellGUIAboutConnection( array<Int32> ^ retArr );

	ref class Comms {
	public:
		Comms(Object ^ telSimulator, Object ^ newDelegate);

		// connecting
		void connectAll();
		void disconnectAll();

		bool connectAutopilot();
		bool connectRabbit();
		
		void disconnectAutopilot();
		void disconnectRabbit();

		// sending
		void gotoLatLon(float lat, float lon);

		// receiving
		void receiveRabbitPacket(ComportDownstream * packet);

		// instance variables
		AutopilotComport ^ autopilot;
		RabbitComport ^ rabbit;

	private:

		TelemetrySimulator ^ theTelSimulator;
		Object ^ theDelegate;
		comportUpdateDelegate ^ comDelegate;

		String ^ autopilotPortname;
		String ^ rabbitPortname;
 	};

}