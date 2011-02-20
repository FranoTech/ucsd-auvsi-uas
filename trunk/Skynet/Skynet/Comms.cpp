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

	autopilotConnected = false;
	rabbitConnected = false;
	
	rabbitDelegate = gcnew rabbitUpdateDelegate(((Skynet::Form1 ^ )theDelegate), &Skynet::Form1::updateGimbalInfo );
	planeGPS = gcnew planeGPSDelegate(((Skynet::Form1 ^ )theDelegate), &Skynet::Form1::updatePlaneGPSInfo );
	planeTelem = gcnew planeTelemDelegate(((Skynet::Form1 ^ )theDelegate), &Skynet::Form1::updatePlaneTelemInfo );
	consoleDelegate = gcnew guiConsoleDelegate(((Skynet::Form1 ^ )theDelegate), &Skynet::Form1::printToConsole );

}

void Comms::connectAll() {

	Comport ^ thePort = gcnew Comport(nullptr);

	array<String ^> ^ portNames = thePort->getPortNames();
	array<System::Byte> ^ helloPacket = {0x33, 0x01 };


	for (int i = 0; i < portNames->Length; i++) {
		
		String ^thePortName = portNames[i];

		Thread ^ connectThread = gcnew Thread(gcnew ParameterizedThreadStart(this, &Comms::attemptConnectionOnPort));
		connectThread->Name = "Comms Connection Thread for port " + thePortName;
		connectThread->Start(thePortName);
			

		/*// set port name
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
			System::Diagnostics::Trace::WriteLine("4 or more bytes in response");
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
		}*/

	}

	Thread::Sleep( 700 );

	//autopilotPortname = "COM1";
	//rabbitPortname = "COM1";

	// connect autopilot and rabbit
	int retval = BOTH_FAILED;
	if (this->connectAutopilot())
		retval += AUTOPILOT_CONNECTED;

	if (this->connectRabbit())
		retval += RABBIT_CONNECTED;

	
	tellGUIAboutConnection ^ tellGUIDelegate = gcnew tellGUIAboutConnection(((Skynet::Form1 ^)theDelegate), &Skynet::Form1::handleConnectionResult );
	array<Int32> ^ retArr = {(Int32)retval};
	((Skynet::Form1 ^ )theDelegate)->Invoke( tellGUIDelegate, retArr );

	printToConsole("Hello", Color::Orange);
}

void Comms::attemptConnectionOnPort( Object ^ port )
{
	String ^ portName = (String ^) port;
	System::Diagnostics::Trace::WriteLine("Comms: attempting to connect on port " + portName);
	
	Comport ^ thePort = gcnew Comport(nullptr);
	array<System::Byte> ^ helloPacket = {0x33, 0x01 };

	thePort->setPortName(portName);

	// connect to port
	thePort->connect();

	// send "hello" packet
	thePort->writeEncodedData( helloPacket );

	// wait for response
	array<System::Byte> ^ response = thePort->readRawData( 500 );

	// test response
	int responseType = -1; // 3 for autopilot, 2 for rabbit
	if (response->Length >= 4) {
		System::Diagnostics::Trace::WriteLine("4 or more bytes in response");
		if (response[0] == 0xFF && response[1] == 0x33 && response[5] == 0xFE) {
				responseType = response[2];
		}

	}


	// close comport
	thePort->disconnect();

	// save port name if valid
	if (responseType == 2) {
		rabbitPortname = portName;

	}
	else if (responseType == 3) {
		autopilotPortname = portName;
	}

	
	System::Diagnostics::Trace::WriteLine("Comms: Finished attempting to connect on port " + portName);

}

void Comms::disconnectAll() {

	disconnectAutopilot();
	disconnectRabbit();

}




bool Comms::connectAutopilot() {
	if (autopilotPortname == nullptr)
		return false;
	
	bool success = false;

	System::Diagnostics::Trace::WriteLine("Connecting to AUTOPILOT");

	success = autopilot->connect(autopilotPortname);
	if (success) {
		autopilotConnected = true;
		autopilot->beginReading("Autopilot");
	}
	
	return success;
}


bool Comms::connectRabbit() {
	bool success = false;
	if (rabbitPortname == nullptr)
		return false;

	
	System::Diagnostics::Trace::WriteLine("Connecting to RABBIT");
	success = rabbit->connect(rabbitPortname);
	if (success) {
		rabbitConnected = true;
		rabbit->beginReading("Rabbit");
	}

	return success;
}



		
void Comms::disconnectAutopilot() {
	if (autopilotConnected) {
		autopilot->disconnect();
		autopilotConnected = false;
	}
}



void Comms::disconnectRabbit() {
	if (rabbitConnected) {
		rabbit->disconnect();
		rabbitConnected = false;
	}
}


void Comms::gotoLatLon(float lat, float lon) 
{
	autopilot->gotoLatLon(lat, lon);
}


void Comms::printToConsole( String ^ message, Color col )
{

	guiConsoleDelegate ^ newconsoleDelegate = gcnew guiConsoleDelegate(((Skynet::Form1 ^)theDelegate), &Skynet::Form1::printToConsole );

	array<Object ^> ^ retArr = gcnew array< Object^ >{message, col};
	
	((Skynet::Form1 ^ )theDelegate)->Invoke( newconsoleDelegate, gcnew array<Object ^>{retArr});

	
}

void Comms::updateUIAboutCommsStatus(bool status, String ^ type)
{
	guiConsoleDelegate ^ newconsoleDelegate = gcnew guiConsoleDelegate(((Skynet::Form1 ^)theDelegate), &Skynet::Form1::updateCommsStatus );

	Boolean value = status;
	array<Object ^> ^ retArr = gcnew array< Object^ >{type, value};
	
	((Skynet::Form1 ^ )theDelegate)->Invoke( newconsoleDelegate, gcnew array<Object ^>{retArr});
}

void Comms::receiveRabbitPacket(GimbalInfo * packet) 
{	
	rabbitDelegate(packet);
}

void Comms::receiveKestrelPacket248(TelemPacket248 * packet)
{

}

void Comms::receiveKestrelPacket249(TelemPacket249 * packet)
{

}


