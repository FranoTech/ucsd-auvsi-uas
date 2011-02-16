#pragma once

#include "Comport.h"
#include "AutopilotComport.h"
#include "RabbitComport.h"
#include "ColorRef.h"

#define BOTH_CONNECTED			3
#define RABBIT_CONNECTED		1
#define AUTOPILOT_CONNECTED		2
#define BOTH_FAILED				0

using namespace System::IO::Ports;
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Drawing;

namespace Communications
{

	
	delegate void rabbitUpdateDelegate( GimbalInfo * data );
	delegate void tellGUIAboutConnection( array<Int32> ^ retArr );
	delegate void guiConsoleDelegate( array<Object ^> ^ retArr );

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
		void receiveRabbitPacket(GimbalInfo * packet);

		void printToConsole( String ^ message, Color col );
		void updateUIAboutCommsStatus(bool status, String ^ type);

		// instance variables
		AutopilotComport ^ autopilot;
		RabbitComport ^ rabbit;

	private:

		TelemetrySimulator ^ theTelSimulator;
		Object ^ theDelegate;
		rabbitUpdateDelegate ^ rabbitDelegate;
		guiConsoleDelegate ^ consoleDelegate;

		String ^ autopilotPortname;
		String ^ rabbitPortname;
		bool autopilotConnected;
		bool rabbitConnected;
 	};

}