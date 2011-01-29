#pragma once
#include <iostream>
#include <fstream>

using namespace std;
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

	private:
		void connectAutopilotOnPort(String ^thePort);
		void connectRabbitOnPort(String ^thePort);

		Object ^ theDelegate;
		Object ^ autopilot;
		Object ^ rabbit;

		String ^ autopilotPortname;
		String ^ rabbitPortname;
 	};

}