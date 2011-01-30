#include "AutopilotComport.h"

using namespace Communications;



void AutopilotComport::writeData( array<System::Byte> ^ inBuffer )
{
	// write data differently
	
	thePort->writeEncodedData( inBuffer );

}


void AutopilotComport::receiveData( array<System::Byte> ^ inBuffer )
{

	// handle data

}

void AutopilotComport::gotoLatLon(float lat, float lon) 
{
	System::Diagnostics::Trace::WriteLine("running AutopilotComport::gotoLatLon()");
	__int16 destAddr = 0; // later, get this from autopilot

	array<System::Byte> ^ packet = getVCPacket32(destAddr, 0, lat, lon, 14.0f, 100.0f, 0, 0, 0, 5.0f, 5.0f, 15.0f, 3.0f, 0.0f, 0.0f);

	// copy all data into packet



	// send it
	writeData( packet );


}
array<System::Byte> ^  AutopilotComport::getVCPacket32(__int16 destAddr, unsigned char commandType, float lat, float lon, float speed, float altitude, 
											unsigned __int16 gotoIndex, unsigned __int16 time, float radius, float flareSpeed, float flareAltitude, 
											float breakAltitude, float descentRate, float approachAltitude, float approachLongitude) 
{
	// 66 bytes total, including 64 bits of unused at the end
	int packetLength = 66;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength + 8);


	PacketHeader *theHeader = new PacketHeader();
	theHeader->type = 32;
	theHeader->size = packetLength;

	// copy packet over
	char * dataPtr = (char *)theHeader;
	int packetIndex = 0;
	for (int j = 0; j < 8; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}



	AutopilotVCPacket32 * theStruct = new AutopilotVCPacket32();

	// copy data into struct
	theStruct->destAddr			 = destAddr				;	
	theStruct->commandType		 = commandType			;
	theStruct->lat				 = lat					;
	theStruct->lon				 = lon					;
	theStruct->speed			 = speed				;
	theStruct->altitude			 = altitude				;
	theStruct->gotoIndex		 = gotoIndex			;
	theStruct->time				 = time					;
	theStruct->radius			 = radius				;
	theStruct->flareSpeed		 = flareSpeed			;
	theStruct->flareAltitude	 = flareAltitude		;
	theStruct->breakAltitude	 = breakAltitude		;
	theStruct->descentRate		 = descentRate			;
	theStruct->approachAltitude	 = approachAltitude		;
	theStruct->approachLongitude = approachLongitude	;




	// copy packet over
	dataPtr = (char *)theStruct;
	for (int j = 0; j < packetLength; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}

	//thePort->writeEncodedData( packet );
	
	return packet;
}