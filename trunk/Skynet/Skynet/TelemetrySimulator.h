#pragma once

//#include "ComportHandler.h"
#include "SimHandler.h"

using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;

namespace Simulator
{
	ref class TelemetrySimulator{
	public:
		TelemetrySimulator(SimHandler ^ simHandler, Object ^ rabbitComport, Object ^ autopilotComport)
		{
			theRabbitComport = rabbitComport;
			theAutopilotComport = autopilotComport;
			theSimHandler = simHandler;
			record = false;
		}

		void setRabbit(Object ^ rabbitComport) { theRabbitComport = rabbitComport; }

		void setAutopilot(Object ^ autopilotComport) { theAutopilotComport = autopilotComport; }

		void beginRecording()
		{
			time = System::DateTime::Now;
			record = true;
		}
		void write(int type, array<System::Byte> ^ byteArray)
		{
			if(record)
			{
				theSimHandler->writeTelemetry(System::DateTime::Now.Subtract(time), type, byteArray->Length, byteArray);
			}
			else{return;}
		}
		void stopRecording()
		{
			record=false;
		}
	protected:
	private:
		bool record;
		System::DateTime time;
		Object ^ theRabbitComport;
		Object ^ theAutopilotComport;
		SimHandler ^ theSimHandler;
	};
}