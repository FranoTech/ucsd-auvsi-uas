#include "StdAfx.h"

#include "RabbitComport.h"
#include "Comms.h"
#include "AutopilotComport.h"

#define RABBIT_DELAY	125 // (ms)

#define GIMBAL_PACKET	0x01
#define CAMERA_PACKET	0x00
#define HELLO_PACKET    0x33

using namespace Communications;

/*
 * Right after comms are set up, ask for packet forwarding and for the agent id
 */
void RabbitComport::afterBeginReading()
{
	// better update frequency
	thePort->updateFrequency = 5;
	zoomLevel = 1;
}

/*
void RabbitComport::connectToRabbit()
{
	
	rabbitState = RABBIT_CONNECTING;
	connectionThread = gcnew Thread(gcnew ThreadStart(this, &Comms::attemptToConnect));
	connectionThread->Name = "Rabbit Connection Thread";
	connectionThread->Start(thePortName);

}

void RabbitComport::attemptToConnect()
{
	try {
		while (rabbitState == RABBIT_CONNECTING)
		{
			// send hello packet through autopilot
			autopilot->sendPassthroughPacket( {0x33, 0x01 } );

			// sleep 1 second
			Thread::Sleep( 1000 );
		}

		
	} catch (Exception ^)
	{
		// kill when we get a response or disconnect from autopilot
	}
	
	connectionThread = nullptr;

}


void RabbitComport::stopConnecting( bool connected )
{
	if (connected)
		rabbitState = RABBIT_CONNECTED;
	else 
		rabbitState = RABBIT_DISCONNECTED;

	Thread::Sleep( 100 );
	if (connectionThread != nullptr) {
		connectionThread->Abort();
		connectionThread = nullptr;
	}

}*/

void RabbitComport::sendBytesToRabbit( array<System::Byte> ^ packet )
{
	// TODO: implement
	((AutopilotComport ^)autopilotComport)->sendPassthroughPacket( packet );
}

void RabbitComport::sendGimbalCommand( __int16 roll, __int16 pitch )
{
	const int packetLength = 5;
	unsigned char packetType = GIMBAL_PACKET;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength);
	packet[0] = packetType;
	packet[1] = ( roll >> 8 ) && 0xFF;
	packet[2] =  roll && 0xFF;
	packet[3] = ( pitch >> 8 ) && 0xFF;
	packet[4] =  pitch && 0xFF;


	// send 
	sendBytesToRabbit( packet );
}

void RabbitComport::sendHello()
{
	const int packetLength = 2;
	unsigned char packetType = HELLO_PACKET;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength);
	packet[0] = packetType;
	packet[1] = 0x01;  //because its comming from the ground station computer

	// send 
	sendBytesToRabbit( packet );
}

void RabbitComport::sendCameraZoom( unsigned __int32 zoom )
{
	const int packetLength = 5;
	unsigned char packetType = GIMBAL_PACKET;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength);
	packet[0] = packetType;
	packet[1] = ( zoom >> 24 ) && 0xFF;
	packet[2] = ( zoom >> 16 ) && 0xFF;
	packet[3] = ( zoom >> 8 ) && 0xFF;
	packet[4] =  zoom && 0xFF;;


	// send 
	sendBytesToRabbit( packet );


}

void RabbitComport::analyzeData( array<System::Byte> ^ inBuffer )
{
	// TODO: check for hello response packet:
	/*
		if (valid)
			rabbitState = RABBIT_CONNECTED;
			stopConnecting( true );

	*/
	System::Diagnostics::Trace::WriteLine("RabbitComport::receiveData()");
	
	// check input
	if (inBuffer->Length < 1)
		return;

	const int NUM_ELEMENTS = 11;
	const int BYTES_PER_FLOAT = 4;


	unsigned char packetType = inBuffer[0];

	// gimbal info
	if (packetType == 1) {
		GimbalInfo * packet = new GimbalInfo();
		const int bytesPerElement = 2;
		const int packetStart = 1;
		const int numElements = 2;

		char * dataPtr = (char *)packet;

		for (int j = 0; j < numElements; j++) {
			for (int i = 0; i < 2; i++) {
				*dataPtr =  inBuffer[j*bytesPerElement + i + packetStart]; // 1 to ignore packetType byte
				dataPtr++;

			}			
		}

		// add last zoom level
		packet->zoom = zoomLevel;

		// calculate packet time (ideally, set this on rabbit and send down with packet, but we will assume a 125ms delay)
		System::DateTime packetTime = System::DateTime::Now.AddMilliseconds(RABBIT_DELAY);
		packet->UTCyear = packetTime.Year;
		packet->UTCmonth = packetTime.Month;
		packet->UTCday = packetTime.Day;
		packet->UTChour = packetTime.Hour;
		packet->UTCmin = packetTime.Minute;
		packet->UTCmillisecond = packetTime.Second*1000 + packetTime.Millisecond;
		
		//System::Diagnostics::Trace::WriteLine("packet type 1. roll: " + packet->roll + " pitch: " + packet->pitch);
		((Comms ^)theDelegate)->receiveRabbitPacket(packet);
		
	}

}