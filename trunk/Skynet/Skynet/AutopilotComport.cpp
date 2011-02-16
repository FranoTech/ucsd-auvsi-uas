#include "StdAfx.h"

#include "AutopilotComport.h"
#include "Comms.h"

using namespace Communications;
using namespace System;
using namespace System::Drawing;

void AutopilotComport::disconnect() 
{
	System::Diagnostics::Trace::WriteLine("disconnecting AutopilotComport::request termination of packet forwarding()");

	array<System::Byte> ^ packet = getVCPacket20(0, 0);

	writeData(packet);
	
	//try {
	thePort->disconnect();
	Thread::Sleep(10);
	if (comReadThread != nullptr) {
		comReadThread->Abort();	
		comReadThread = nullptr;
	}
	//}
	//catch (Exception ^) {}
}

void AutopilotComport::writeData( array<System::Byte> ^ inBuffer )
{
	thePort->writeEncodedData( inBuffer );
}


void AutopilotComport::receiveData( array<System::Byte> ^ inBuffer )
{
	
	//System::Diagnostics::Trace::WriteLine("AutopilotComport::receiveData()");

	// handle data
	PacketHeader *theHeader = new PacketHeader();

	// copy packet over
	char * dataPtr = (char *)theHeader;
	int packetIndex = 0;
	for (int j = 0; j < 8; j++) {
		dataPtr[j] = inBuffer[packetIndex];
		packetIndex++;
	}

	/* 	char * dataPtr = (char *)theHeader;
	int packetIndex = 0;
	
	theHeader->type = getInt32FromBytes(inBuffer, 0);
	theHeader->size = getInt32FromBytes(inBuffer, 4); */

	System::Diagnostics::Trace::WriteLine("AutopilotComport::receiveData(): packetType: " + theHeader->type);
	
	if(theHeader->type == 10){  //pass through autopilot packet which we need to decode, passthrough non-guaranteed packet
		array<System::Byte> ^ newPacket = thePort->decodeData(inBuffer);

		if(newPacket != nullptr){  //if the decoded packet's xor worked out
			__int16 sourceAddress = getInt16FromBytes(newPacket, 0);  //get the source address of the packet, it should be from agentAddress
			unsigned char kestrelPacketType = getUCharFromBytes(newPacket, 2);  //the packet type, in kestrel terms rather than VC terms (ie, chapter 4 of kestrel comms guide)
			
			/*if(kestrelPacketType == 1){ // acknowledgement packet
				unsigned char packetType = getUCharFromBytes(newPacket, 3);  //the packet type being acknowledged.  XXX we don't do anything with it, but we can if we need to
			}*/
			System::Diagnostics::Trace::WriteLine("AutopilotComport::receiveData(): kestrelPacketType: " + kestrelPacketType);
			if(kestrelPacketType == 29)  //mixed telemetry response
			{
				planeState->roll = (float)(getInt16FromBytes(newPacket, 3)) / 1000.0;
				planeState->pitch = (float)(getInt16FromBytes(newPacket, 5)) / 1000.0;
				planeState->heading = (float)(getUInt16FromBytes(newPacket, 7)) / 1000.0;
				planeState->groundTrack = (float)(getUInt16FromBytes(newPacket, 9)) / 1000.0;
				planeState->altitudeHAL = (float)(getUInt16FromBytes(newPacket, 11)) / 6.0 - 1000.0;
				planeState->gpsLatitude = getFloatFromBytes(newPacket, 13);
				planeState->gpsLongitude = getFloatFromBytes(newPacket, 17);
				planeState->UTCyear = getUCharFromBytes(newPacket, 23);
				planeState->UTCmonth = getUCharFromBytes(newPacket, 24);
				planeState->UTCday = getUCharFromBytes(newPacket, 25);
				planeState->UTChour = getUCharFromBytes(newPacket, 26);
				planeState->UTCmin = getUCharFromBytes(newPacket, 27);
				planeState->UTCmillisecond = getUInt16FromBytes(newPacket, 28);
				planeState->airspeed = (float)(getUInt16FromBytes(newPacket, 30)) / 20.0 - 10.0;
				System::Diagnostics::Trace::WriteLine("TESTING: Latitude " + planeState->gpsLatitude + "  Longitude: " + planeState->gpsLongitude);
			} 
			else if(kestrelPacketType == 248)  //navigation packet
			{
				planeState->gpsVelocity = (float)(getUInt16FromBytes(newPacket, 3)) / 20.0 - 10.0;
				planeState->gpsAltitude = (float)(getUInt16FromBytes(newPacket, 5)) / 6.0 - 1000.0;
				planeState->gpsHeading = (float)(getUInt16FromBytes(newPacket, 7)) / 1000.0;
				planeState->gpsLatitude = getFloatFromBytes(newPacket, 11);
				planeState->gpsLongitude = getFloatFromBytes(newPacket, 17);
				planeState->gpsHomePositionLatitude = getFloatFromBytes(newPacket, 21);
				planeState->gpsHomePositionLongitude = getFloatFromBytes(newPacket, 25);
				planeState->currentCommand = getUCharFromBytes(newPacket, 29);
				planeState->desiredLatitude = getFloatFromBytes(newPacket, 31);
				planeState->desiredLongitude = getFloatFromBytes(newPacket, 35);
				planeState->timeToTarget = getFloatFromBytes(newPacket, 39);
				planeState->distanceToTarget = getFloatFromBytes(newPacket, 43);
				planeState->headingToTarget = (float)(getUInt16FromBytes(newPacket, 47)) / 1000.0;
				planeState->UTCyear = getUCharFromBytes(newPacket, 56);
				planeState->UTCmonth = getUCharFromBytes(newPacket, 57);
				planeState->UTCday = getUCharFromBytes(newPacket, 58);
				planeState->UTChour = getUCharFromBytes(newPacket, 59);
				planeState->UTCmin = getUCharFromBytes(newPacket, 60);
				planeState->UTCmillisecond = getUInt16FromBytes(newPacket, 61);
				planeState->gpsHomePositionAltitude = (float)(getUInt16FromBytes(newPacket, 63) / 6.0 - 1000.0);
				System::Diagnostics::Trace::WriteLine("TESTING: Latitude " + planeState->gpsLatitude + "  Longitude: " + planeState->gpsLongitude); 
			}
			else if(kestrelPacketType == 249) //telemetry packet
			{
				planeState->altitudeHAL = (float)(getUInt16FromBytes(newPacket, 3)) / 6.0 - 1000.0;
				planeState->velocity = (float)(getUInt16FromBytes(newPacket, 5)) / 20.0 - 10.0;
				planeState->roll = (float)(getInt16FromBytes(newPacket, 7)) / 1000.0;
				planeState->pitch = (float)(getInt16FromBytes(newPacket, 9)) / 1000.0;
				planeState->heading = (float)(getUInt16FromBytes(newPacket, 11)) / 1000.0;
				planeState->turnRate = (float)(getInt16FromBytes(newPacket, 13)) / 1000.0;
				planeState->batteryVoltage = (float)(getUCharFromBytes(newPacket, 18)) / 5.0;
				planeState->gpsNumSats = getUCharFromBytes(newPacket, 21);
				planeState->desiredAltitude = (float)(getUInt16FromBytes(newPacket, 23)) / 6.0 - 1000.0;
				planeState->desiredVelocity = (float)(getUCharFromBytes(newPacket, 25)) / 2.0 - 10.0;
				planeState->desiredRoll = (float)(getInt16FromBytes(newPacket, 26)) / 1000.0;
				planeState->desiredPitch = (float)(getInt16FromBytes(newPacket, 28)) / 1000.0;
				planeState->desiredHeading = (float)(getUInt16FromBytes(newPacket, 30)) / 1000.0;
				planeState->desiredTurnRate = (float)(getInt16FromBytes(newPacket, 32)) / 1000.0;
				planeState->airbornTimer = getFloatFromBytes(newPacket, 43);
				//Note: I am not extractin gps time update here
				planeState->rollRate = ((float)(getCharFromBytes(newPacket, 57)) - 128.0) / 80.0;
				planeState->pitchRate = ((float)(getCharFromBytes(newPacket, 58)) - 128.0) / 80.0;
				planeState->yawRate = ((float)(getCharFromBytes(newPacket, 59)) - 128.0) / 80.0;
				planeState->altitudeMSL = (float)(getUInt16FromBytes(newPacket, 60)) / 6.0 - 1000.0;
				planeState->desiredClimbRate = (float)(getInt16FromBytes(newPacket, 77)) / 300.0;
				planeState->climbRate = (float)(getInt16FromBytes(newPacket, 79)) / 300.0;
			}
		}

	} else if(theHeader->type == 61) {  // get agent list response
		System::Diagnostics::Trace::WriteLine("Got agent list response");

		String ^ bufferString = "0x" + Convert::ToString(inBuffer[0], 16);
		for (int i = 1; i < inBuffer->Length; i++) {
			bufferString = bufferString + " 0x" + Convert::ToString(inBuffer[i], 16);
		}
		
		//System::Diagnostics::Trace::WriteLine("AutopilotComport::receiveData bufLen:" + inBuffer->Length + " buffer:" + bufferString);

		unsigned char numAgents = getUCharFromBytes(inBuffer, 8);  //get the number of agents in the list.  We typically expect 1
		if(numAgents > 0){
			__int16 agentAddress = getInt16FromBytes(inBuffer, 9);  //get the agent address
				planeState->address = agentAddress;  //set the autopilot address
				System::Diagnostics::Trace::WriteLine("Set Autopilot Address to " + planeState->address);
				//((Comms ^)theDelegate)->printToConsole("Set Autopilot Address to " + planeState->address, gcnew ColorRef( Color::Green ));
		}



	}

		//thePort->decodeData(packetData);
}
		
/*
 * Right after comms are set up, ask for packet forwarding and for the agent id
 */
void AutopilotComport::afterBeginReading()
{
	// better update frequency
	thePort->updateFrequency = 5;


	System::Diagnostics::Trace::WriteLine("running AutopilotComport::afterBeginReading()");
	Thread::Sleep( 250 );
	requestAgents();  //ask for all agents
	Thread::Sleep( 250 );
	requestPacketForwarding();  //ask VC to pass on data
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

	__int16 retVal;
	unsigned char * pointerToVal = (unsigned char *)(&retVal);
	for(int i = startIndex; i < startIndex + 2; i++){
		* pointerToVal = arr[i];
		pointerToVal ++;
		//System::Diagnostics::Trace::WriteLine("AutopilotComport::getInt16FromBytes(): byte being converted: " + Convert::ToString(arr[i], 16));
	}
	
	return retVal;
}

/*
 * extracts an unsigned 16 bit integer from a byte array
 */
unsigned __int16 AutopilotComport::getUInt16FromBytes(array<System::Byte> ^ arr, int startIndex)
{
	if(arr->Length < startIndex + 2){
		return 0;
	}

	unsigned __int16 retVal;
	unsigned char * pointerToVal = (unsigned char *)(&retVal);
	for(int i = startIndex + 1; i >= startIndex; i--){
		* pointerToVal = arr[i];
		pointerToVal ++;
	}
	
	return retVal;
}

/*
 * extracts a 32 bit integer from a byte array
 */
__int32 AutopilotComport::getInt32FromBytes(array<System::Byte> ^ arr, int startIndex)
{
	if(arr->Length < startIndex + 4){
		return 0;
	}

	__int32 retVal;
	unsigned char * pointerToVal = (unsigned char *)(&retVal);
	for(int i = startIndex + 3; i >= startIndex; i--){
		* pointerToVal = arr[i];
		pointerToVal ++;
	}
	
	return retVal;
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
