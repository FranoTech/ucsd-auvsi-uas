#include "StdAfx.h"

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
	PacketHeader *theHeader = new PacketHeader();

	// copy packet over
	char * dataPtr = (char *)theHeader;
	int packetIndex = 0;
	for (int j = 0; j < 8; j++) {
		dataPtr[j] = packet[packetIndex];
		packetIndex++;
	}
	
	if(theHeader->type == 10){  //pass through autopilot packet which we need to decode

	} else if(theHeader->type == 61) {  // get agent list response
		unsigned char numAgents = get;
	}

		//thePort->decodeData(packetData);
}
		
/*
 * Right after comms are set up, ask for packet forwarding and for the agent id
 */
void AutopilotComport::afterBeginReading()
{
	requestPacketForwarding();  //ask VC to pass on data
	requestAgents();  //ask for all agents
}

void AutopilotComport::gotoLatLon(float lat, float lon) 
{
	System::Diagnostics::Trace::WriteLine("running AutopilotComport::gotoLatLon()");

	array<System::Byte> ^ packet = getVCPacket32(0, lat, lon, 14.0f, 100.0f, 0, 0, 0, 5.0f, 5.0f, 15.0f, 3.0f, 0.0f, 0.0f);

	// copy all data into packet

	writeData( packet ); // set waypoint command on VC

	packet = getVCPacket30(destAddr);  //get the upload commands packet

	writeData( packet );  //upload waypoint from VC to Autopilot
}

/*
 * asks VC to forward all packets
 */
void AutopilotComport::requestPacketForwarding()
{
	System::Diagnostics::Trace::WriteLine("running AutopilotComport::requestPacketForwarding()");

	array<System::Byte> ^ packet = getVCPacket20(0, 1);

	writeData( packet );
}

/*
 * asks VC to give a list of all agents
 */
void AutopilotComport::requestAgents()
{
	System::Diagnostics::Trace::WriteLine("running AutopilotComport::requestAgents()");

	array<System::Byte> ^ packet = getVCPacket61();

	writeData( packet );
}

/*
 * Packet Forwarding Setup
 * packetID = the kestrel packet id to forward, 0 means all packets will be forwarded
 * onOff = set to 1 to turn on forwarding, 0 turns off forwarding
 */
array<System::Byte> ^ AutopilotComport::getVCPacket20(unsigned char packetID, unsigned char onOff)
{
	int packetLength = 2;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength + 8);

	PacketHeader *theHeader = new PacketHeader();
	theHeader->type = 20;
	theHeader->size = packetLength;

	// copy packet over
	char * dataPtr = (char *)theHeader;
	int packetIndex = 0;
	for (int j = 0; j < 8; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}

	AutopilotVCPacket20 * theStruct = new AutopilotVCPacket20();

	// copy data into struct
	theStruct->packetID = packetID;	
	theStruct->onOff = onOff;	

	// copy packet over
	dataPtr = (char *)theStruct;
	for (int j = 0; j < packetLength; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}
	
	return packet;
}

/*
 * Upload command list from VC to autopilot
 */
array<System::Byte> ^ AutopilotComport::getVCPacket30()
{
	int packetLength = 2;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength + 8);

	PacketHeader *theHeader = new PacketHeader();
	theHeader->type = 30;
	theHeader->size = packetLength;

	// copy packet over
	char * dataPtr = (char *)theHeader;
	int packetIndex = 0;
	for (int j = 0; j < 8; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}

	AutopilotVCPacket30 * theStruct = new AutopilotVCPacket30();

	// copy data into struct
	theStruct->destAddr = autopilotAddress;	

	// copy packet over
	dataPtr = (char *)theStruct;
	for (int j = 0; j < packetLength; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}
	
	return packet;
}

/*
 * Set Command List on VC
 */
array<System::Byte> ^  AutopilotComport::getVCPacket32(unsigned char commandType, float lat, float lon, float speed, float altitude, 
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
	theStruct->destAddr			 = autopilotAddress		;	
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

/*
 * Request List of Agents
 */
array<System::Byte> ^ AutopilotComport::getVCPacket61()
{
	int packetLength = 0;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength + 8);

	PacketHeader *theHeader = new PacketHeader();
	theHeader->type = 61;
	theHeader->size = packetLength;

	// copy packet over
	char * dataPtr = (char *)theHeader;
	int packetIndex = 0;
	for (int j = 0; j < 8; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}
	
	return packet;
}

/*
 * extracts a 16 bit integer from a byte array
 */
__int16 AutopilotComport::getInt16FromBytes(array<System::Byte> ^ arr, int startIndex)
{
	if(arr->Length < startIndex + 2){
		return 0;
	}

	return arr[startIndex] + (a[startIndex + 1] << 8);
}

/*
 * extracts a 32 bit integer from a byte array
 */
__int32 AutopilotComport::getInt32FromBytes(array<System::Byte> ^ arr, int startIndex)
{
	if(arr->Length < startIndex + 4){
		return 0;
	}
	return arr[startIndex] + (arr[startIndex + 1] << 8) + (arr[startIndex + 2] << 16) + (arr[startIndex + 3] << 24); 
}

/*
 * extracts an 8 bit unsigned char from a byte array
 */
unsigned char AutopilotComport::getUCharFromBytes(array<System::Byte> ^ arr, int startIndex)
{
	if(arr->Length < startIndex + 1){
		return 0;
	}
	return arr[startIndex];
}

/*
 * extracts an 8 bit char from a byte array
 */
char AutopilotComport::getCharFromBytes(array<System::Byte> ^ arr, int startIndex)
{
	if(arr->Length < startIndex + 1){
		return 0;
	}
	return arr[startIndex];
}

/*
 * extracts a 32 bit float from a byte array
 */
float AutopilotComport::getFloatFromBytes(array<System::Byte> ^ arr, int startIndex)
{
	if(arr->Length < startIndex + 4){
		return 0;
	}

	float retVal;
	unsigned char * pointerToFloat = &retVal;
	for(int i = startIndex + 3; i >= startIndex; i--){
		* pointerToFloat = arr[i];
		pointerToFloat ++;
	}
	
	return retVal;
}
