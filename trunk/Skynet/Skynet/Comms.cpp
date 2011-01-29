#include "StdAfx.h"
#include "Comms.h"
#include "Comport.h"

using namespace Communications;
using namespace System::Threading;


Comms::Comms(Object ^ newDelegate) {

	theDelegate = newDelegate;
	autopilotPortname = nullptr;
	rabbitPortname = nullptr;

}

void Comms::connectAll() {

	Comport ^ thePort = gcnew Comport(this);

	array<String ^> ^ portNames = thePort->getPortNames();
	array<System::Byte> ^ helloPacket = {0x33, 0x01 };


	for (int i = 0; i < portNames->Length; i++) {
		
		// set port name
		System::Diagnostics::Trace::WriteLine("Connecting to port " + portNames[i] );
		thePort->setPortName(portNames[i]);

		// connect to port
		thePort->connect();

		// send "hello" packet
		thePort->writeEncodedData( helloPacket );

		// wait for response
		array<System::Byte> ^ response = thePort->readRawData( 500 );

		// test response
		int responseType = -1; // 3 for autopilot, 2 for rabbit
		if (response->Length >= 4) {
			if (response[0] == 0xFF && response[1] == 0x33 && response[5] == 0xFE) {
					responseType = response[2];
			}

		}


		// close comport
		thePort->disconnect();

		// save port name if valid
		if (responseType == 2) {
			rabbitPortname = portNames[i];
			System::Diagnostics::Trace::WriteLine("FOUND RABBIT");

		}
		else if (responseType == 3) {
			autopilotPortname = portNames[i];
			System::Diagnostics::Trace::WriteLine("FOUND AUTOPILOT");
		}

	}

	// connect autopilot and rabbit
	this->connectAutopilot();
	this->connectRabbit();
}



void Comms::disconnectAll() {

	disconnectAutopilot();
	disconnectRabbit();

}




void Comms::connectAutopilot() {
	if (autopilotPortname == nullptr)
		return;

	connectAutopilotOnPort(autopilotPortname);
}


void Comms::connectRabbit() {{
	if (rabbitPortname == nullptr)
		return;

	
	connectRabbitOnPort(rabbitPortname);
}



		
void Comms::disconnectAutopilot() {
}



void Comms::disconnectRabbit() {
}




void Comms::connectAutopilotOnPort(String ^thePort) {
}



void Comms::connectRabbitOnPort(String ^thePort) {
}



