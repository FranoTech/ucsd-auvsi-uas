#pragma once

#include "ComportHandler.h"

using namespace System;
using namespace System::Runtime::InteropServices;

#define		RABBIT_DISCONNECTED	0
#define		RABBIT_CONNECTING	2
#define		RABBIT_CONNECTED	1

namespace Communications
{
	// packet type (unsigned char), data
	//	gimbal data: __int16 roll, __int16 pitch
	//	camera data: unsigned __int32 zoomLevel

	ref class RabbitComport : public ComportHandler {
	public:
		RabbitComport(TelemetrySimulator ^ telSimulator, Object ^ newDelegate, Object ^ newAutopilotComport) : ComportHandler(telSimulator, newDelegate, "Rabbit") 
		{	autopilotComport = newAutopilotComport; connectionThread = nullptr; rabbitState = RABBIT_DISCONNECTED; rabbitState = false; };

		//void connectToRabbit();
		//void stopConnecting( bool connected );

		virtual void analyzeData( array<System::Byte> ^ inBuffer ) override; 
		virtual void afterBeginReading() override;
		virtual void disconnect() override;
		void sendGimbalCommand( unsigned __int16 roll, unsigned __int16 pitch );
		void sendCameraZoom( unsigned __int32 zoom );
		void sendHello();
		bool rabbitState;

	protected:
		void sendBytesToRabbit( array<System::Byte> ^ packet );
		//void attemptToConnect();

		unsigned __int32 zoomLevel;
		unsigned __int16 lastRoll;
		unsigned __int16 lastPitch;

		Object ^ autopilotComport;
		Thread ^ connectionThread;
	};


}