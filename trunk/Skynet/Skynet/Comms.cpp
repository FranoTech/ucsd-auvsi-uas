#include "StdAfx.h"
#include "Comms.h"
#include "Comport.h"
#include "Form1.h"

using namespace Communications;
using namespace System::Threading;


Comms::Comms(Object ^ telSimulator, Object ^ newDelegate) {

	theDelegate = newDelegate;
	autopilotPortname = nullptr;
	rabbitPortname = nullptr;
	autopilot = gcnew AutopilotComport((TelemetrySimulator ^)telSimulator, this);
	rabbit = gcnew RabbitComport((TelemetrySimulator ^)telSimulator, this);

	theTelSimulator = (TelemetrySimulator ^)telSimulator;

	
	comDelegate = gcnew comportUpdateDelegate(((Skynet::Form1 ^ )theDelegate), &Skynet::Form1::updateComData );
	consoleDelegate = gcnew guiConsoleDelegate(((Skynet::Form1 ^ )theDelegate), &Skynet::Form1::printToConsole );

}

void Comms::connectAll() {

	Comport ^ thePort = gcnew Comport(nullptr);

	array<String ^> ^ portNames = thePort->getPortNames();
	array<System::Byte> ^ helloPacket = {0x33, 0x01 };


	for (int i = 0; i < portNames->Length; i++) {
		
		// set port name
		//System::Diagnostics::Trace::WriteLine("Connecting to port " + portNames[i] );
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

	//autopilotPortname = "COM1";

	// connect autopilot and rabbit
	int retval = BOTH_FAILED;
	if (this->connectAutopilot())
		retval += AUTOPILOT_CONNECTED;

	if (this->connectRabbit())
		retval += RABBIT_CONNECTED;

	
	tellGUIAboutConnection ^ tellGUIDelegate = gcnew tellGUIAboutConnection(((Skynet::Form1 ^)theDelegate), &Skynet::Form1::handleConnectionResult );
	array<Int32> ^ retArr = {(Int32)retval};
	((Skynet::Form1 ^ )theDelegate)->Invoke( tellGUIDelegate, retArr );

	//printToConsole("Hello", gcnew ColorRef(Color::Blue));
}



void Comms::disconnectAll() {

	disconnectAutopilot();
	disconnectRabbit();

}




bool Comms::connectAutopilot() {
	if (autopilotPortname == nullptr)
		return false;
	
	System::Diagnostics::Trace::WriteLine("Connecting to AUTOPILOT");

	autopilot->connect(autopilotPortname);
	autopilot->beginReading("Autopilot");
	
	return true;
}


bool Comms::connectRabbit() {
	if (rabbitPortname == nullptr)
		return false;

	
	System::Diagnostics::Trace::WriteLine("Connecting to RABBIT");
	rabbit->connect(rabbitPortname);
	rabbit->beginReading("Rabbit");

	return true;
}



		
void Comms::disconnectAutopilot() {
}



void Comms::disconnectRabbit() {
}


void Comms::gotoLatLon(float lat, float lon) 
{
	autopilot->gotoLatLon(lat, lon);
}


void Comms::printToConsole( String ^ message, ColorRef ^ col )
{

	// THIS DOESNT WORK
	System::Diagnostics::Trace::WriteLine("Comms::printToConsole(): " + message);
	return;
	guiConsoleDelegate ^ newconsoleDelegate = gcnew guiConsoleDelegate(((Skynet::Form1 ^)theDelegate), &Skynet::Form1::printToConsole );
	array< Object^ >^ local = gcnew array< Object^ >(1);
	local[0] = message;
	//local[1] = col;

	array<Object ^> ^ retArr = {message};
	
	System::Diagnostics::Trace::WriteLine("about to invoke");
	((Skynet::Form1 ^ )theDelegate)->Invoke( newconsoleDelegate, gcnew array<Object ^>{message, col});

	//array<Object ^> ^ retArr = {message, col};
	//((Skynet::Form1 ^ )theDelegate)->Invoke( consoleDelegate, retArr );
	
}

void Comms::receiveRabbitPacket(ComportDownstream * packet) 
{
	
	comDelegate(packet);
}


