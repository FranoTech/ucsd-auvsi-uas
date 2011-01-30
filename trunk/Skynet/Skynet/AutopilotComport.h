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
		AutopilotComport(Object ^ newDelegate) : ComportHandler(newDelegate, "Autopilot") {};

		void writeData( array<System::Byte> ^ inBuffer ); 
		void receiveData( array<System::Byte> ^ inBuffer );
		
		void gotoLatLon(float lat, float lon);
		array<System::Byte> ^ getVCPacket32(__int16 destAddr, unsigned char commandType, float lat, float lon, float speed, float altitude, 
											unsigned __int16 gotoIndex, unsigned __int16 time, float radius, float flareSpeed, float flareAltitude, 
											float breakAltitude, float descentRate, float approachAltitude, float approachLongitude);

	};


}