#pragma once

#include "Comport.h"
#include "ComportHandler.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Communications
{

	struct PacketHeader
	{
		__int32 type;
		__int32 size;

	};

	struct AutopilotVCPacket20
	{
		unsigned char packetID;
		unsigned char onOff;
	};

	struct AutopilotVCPacket30
	{
		__int16 destAddr;
	};

	struct AutopilotVCPacket32
	{
		__int16				destAddr			;
		unsigned char		commandType			;
		float				lat					;
		float				lon					;
		float				speed				;
		float				altitude			;
		unsigned __int16	gotoIndex			;
		unsigned __int16	time				;
		float 				radius				;
		float 				flareSpeed			;
		float 				flareAltitude		;
		float 				breakAltitude		;
		float				descentRate			;
		float				approachAltitude	;
		float				approachLongitude	;
		float				unusedone;
		float				unusedtwo;
	};

	ref class AutopilotComport : public ComportHandler {
	public:
		AutopilotComport(TelemetrySimulator ^ telSimulator, Object ^ newDelegate) : ComportHandler(telSimulator, newDelegate, "Autopilot") {};

		void writeData( array<System::Byte> ^ inBuffer ); 
		void receiveData( array<System::Byte> ^ inBuffer );
		
		void gotoLatLon(float lat, float lon);
		void requestPacketForwarding();  //turn on packet forwarding from VC
		void afterBeginReading();
		void requestAgents();

	private:
		__int16 autopilotAddress;  //the address of the autopilot
		array<System::Byte> ^ getVCPacket20(unsigned char packetID, unsigned char onOff);
		array<System::Byte> ^ getVCPacket32(unsigned char commandType, float lat, float lon, float speed, float altitude, 
											unsigned __int16 gotoIndex, unsigned __int16 time, float radius, float flareSpeed, float flareAltitude, 
											float breakAltitude, float descentRate, float approachAltitude, float approachLongitude);
		array<System::Byte> ^ getVCPacket30();
		array<System::Byte> ^ getVCPacket61();

		__int16 getInt16FromBytes(array<System::Byte> ^ arr, int startIndex);
		__int32 getInt32FromBytes(array<System::Byte> ^ arr, int startIndex);
		unsigned char getUCharFromBytes(array<System::Byte> ^ arr, int startIndex);
		char getCharFromBytes(array<System::Byte> ^ arr, int startIndex);
		float getFloatFromBytes(array<System::Byte> ^ arr, int startIndex);

	};

}