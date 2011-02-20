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
		System::DateTime time;

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
			//System::Diagnostics::Trace::WriteLine("WRITE IN TELEMETRYSIM IS CALLED "+record);
			if(record)
			{
				//System::Diagnostics::Trace::WriteLine("WRITE IN TELEMETRYSIM IS CALLED FOR REALS");
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
		Object ^ theRabbitComport;
		Object ^ theAutopilotComport;
		SimHandler ^ theSimHandler;
	};
}