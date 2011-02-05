#include "StdAfx.h"

#include "RabbitComport.h"

using namespace Communications;



void RabbitComport::receiveData( array<System::Byte> ^ inBuffer )
{
	const int NUM_ELEMENTS = 11;
	const int BYTES_PER_FLOAT = 4;

	// check input
	if (inBuffer->Length < (NUM_ELEMENTS*BYTES_PER_FLOAT + 1)) // 1 IS FOR ERROR CODE
		return;

	// override receiveData in ComportHandler
	ComportDownstream * packet = new ComportDownstream();

	char * dataPtr = (char *)packet;

	for (int j = 0; j < NUM_ELEMENTS; j++) {
		for (int i = 3, k = 0; i > -1; i--, k++) {
			int letterPosition = j*4 + k;
			dataPtr[letterPosition] = inBuffer[j*4 + i + 1];

		}			
	}

	packet->error_code = inBuffer[inBuffer->Length - 4];



}