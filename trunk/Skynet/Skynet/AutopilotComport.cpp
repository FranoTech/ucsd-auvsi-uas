#include "StdAfx.h"

#include "AutopilotComport.h"
#include "Comms.h"

using namespace Communications;
using namespace System;
using namespace System::Drawing;


void AutopilotComport::writeData( array<System::Byte> ^ inBuffer )
{
	// write data differently
	
	thePort->writeEncodedData( inBuffer );

}


void AutopilotComport::receiveData( array<System::Byte> ^ inBuffer )
{
	
	System::Diagnostics::Trace::WriteLine("AutopilotComport::receiveData()");

	// handle data
	PacketHeader *theHeader = new PacketHeader();

	// copy packet over
	char * dataPtr = (char *)theHeader;
	int packetIndex = 0;
	for (int j = 0; j < 8; j++) {
		dataPtr[j] = inBuffer[packetIndex];
		packetIndex++;
	}
	
	if(theHeader->type == 10){  //pass through autopilot packet which we need to decode, passthrough non-guaranteed packet
		array<System::Byte> ^ newPacket = thePort->decodeData(inBuffer);

		if(newPacket != nullptr){  //if the decoded packet's xor worked out
			__int16 sourceAddress = getInt16FromBytes(newPacket, 0);  //get the source address of the packet, it should be from agentAddress
			unsigned char kestrelPacketType = getUCharFromBytes(newPacket, 2);  //the packet type, in kestrel terms rather than VC terms (ie, chapter 4 of kestrel comms guide)
			
			/*if(kestrelPacketType == 1){ // acknowledgement packet
				unsigned char packetType = getUCharFromBytes(newPacket, 3);  //the packet type being acknowledged.  XXX we don't do anything with it, but we can if we need to
			}*/
			if(kestrelPacketType == 29)  //mixed telemetry response
			{
				planeState->roll = (float)(getInt16FromBytes(newPacket, 3)) / 1000.0;
				planeState->pitch = (float)(getInt16FromBytes(newPacket, 5)) / 1000.0;
				//planeState->heading = (float)(get  //XXX finish this
			}
		}

	} else if(theHeader->type == 61) {  // get agent list response
		unsigned char numAgents = getUCharFromBytes(inBuffer, 8);  //get the number of agents in the list.  We typically expect 1
		int counter = 0;
		while(counter < numAgents && 9 + 2*counter < inBuffer->Length){
			__int16 agentAddress = getInt16FromBytes(inBuffer, 9 + 2 * counter);  //get the agent address
			if(planeState->address == 0){  //it is currently unassigned
				planeState->address = agentAddress;  //set the autopilot address
				System::Diagnostics::Trace::WriteLine("Set Autopilot Address to " + planeState->address);
				((Comms ^)theDelegate)->printToConsole("Set Autopilot Address to " + planeState->address, gcnew ColorRef( Color::Green ));
			}
			counter ++;
		}



	}

		//thePort->decodeData(packetData);
}
		
/*
 * Right after comms are set up, ask for packet forwarding and for the agent id
 */
void AutopilotComport::afterBeginReading()
{
	System::Diagnostics::Trace::WriteLine("running AutopilotComport::afterBeginReading()");
	requestPacketForwarding();  //ask VC to pass on data
	requestAgents();  //ask for all agents
}

void AutopilotComport::gotoLatLon(float lat, float lon) 
{
	System::Diagnostics::Trace::WriteLine("running AutopilotComport::gotoLatLon()");

	array<System::Byte> ^ packet = getVCPacket32(0, lat, lon, 14.0f, 100.0f, 0, 0, 0, 5.0f, 5.0f, 15.0f, 3.0f, 0.0f, 0.0f);

	// copy all data into packet

	writeData( packet ); // set waypoint command on VC

	packet = getVCPacket30();  //get the upload commands packet

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
	theStruct->destAddr = planeState->address;	

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
	theStruct->destAddr			 = planeState->address;	
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

	//XXX fix all of these

	return arr[startIndex] + (arr[startIndex + 1] << 8);
}

/*
 * extracts an unsigned 16 bit integer from a byte array
 */
unsigned __int16 AutopilotComport::getUInt16FromBytes(array<System::Byte> ^ arr, int startIndex)
{
	if(arr->Length < startIndex + 2){
		return 0;
	}

	return arr[startIndex] + (arr[startIndex + 1] << 8);
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
	unsigned char * pointerToFloat = (unsigned char *)(&retVal);
	for(int i = startIndex + 3; i >= startIndex; i--){
		* pointerToFloat = arr[i];
		pointerToFloat ++;
	}
	
	return retVal;
}

//XXX
//TODO:
// change all non-unsigned get functions to use the float pointer method
// finish packet interpretation, packet 29 was where you left off
