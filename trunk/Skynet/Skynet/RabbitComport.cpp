#include "StdAfx.h"

#include "RabbitComport.h"
#include "Comms.h"

using namespace Communications;

/*
 * Right after comms are set up, ask for packet forwarding and for the agent id
 */
void RabbitComport::afterBeginReading()
{
	// better update frequency
	thePort->updateFrequency = 5;
}

void RabbitComport::receiveData( array<System::Byte> ^ inBuffer )
{
	
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
		
		//System::Diagnostics::Trace::WriteLine("packet type 1. roll: " + packet->roll + " pitch: " + packet->pitch);
		((Comms ^)theDelegate)->receiveRabbitPacket(packet);
		
	}

}