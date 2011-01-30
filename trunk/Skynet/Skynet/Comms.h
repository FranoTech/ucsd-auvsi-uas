#pragma once

#include "AutopilotComport.h"
#include "RabbitComport.h"

using namespace System::IO::Ports;
using namespace System;
using namespace System::Runtime::InteropServices;

namespace Communications
{


	ref class Comms {
	public:
		Comms(Object ^ newDelegate);

		void connectAll();
		void disconnectAll();

		void connectAutopilot();
		void connectRabbit();
		
		void disconnectAutopilot();
		void disconnectRabbit();

		void gotoLatLon(float lat, float lon);

	private:


		Object ^ theDelegate;
		AutopilotComport ^ autopilot;
		RabbitComport ^ rabbit;

		String ^ autopilotPortname;
		String ^ rabbitPortname;
 	};

}