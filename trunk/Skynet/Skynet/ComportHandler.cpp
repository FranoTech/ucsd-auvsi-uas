#include "StdAfx.h"

#include "ComportHandler.h"
#include "Comport.h"


using namespace Communications;



ComportHandler::ComportHandler(TelemetrySimulator ^ telSimulator, Object ^ newDelegate, String ^ comType) 
{
	theDelegate = newDelegate;
	type = comType;
	theTelSimulator = telSimulator;
	thePort = gcnew Comport(this);
}

void ComportHandler::connect(String ^ portName) 
{

	thePort->setPortName(portName);
	
	thePort->connect();
}


void ComportHandler::disconnect() 
{
	comReadThread->Abort();	
	thePort->disconnect();
}

void ComportHandler::beginReading(String ^ comportType)
{
	type = comportType;
	comReadThread = gcnew Thread(gcnew ThreadStart(thePort, &Comport::readThread));
	comReadThread->Name =  comportType + " Comport Read Thread";
	comReadThread->Start();

	afterBeginReading();
}


void ComportHandler::afterBeginReading()
{
	// overriden (by AutopilotComport) for custom events run right after we begin receiving packets

}

void ComportHandler::writeData( array<System::Byte> ^ inBuffer ) 
{
	thePort->writeEncodedData( inBuffer );
}

void ComportHandler::receiveData( array<System::Byte> ^ inBuffer ) 
{
	// do something with this data (probably up to theDelegate?)
}


/**
Flip 2 Bytes. Works for 16 bit numbers. UNTESTED. Looks buff.
inline void switchbytes(int &input) { input = (input >> 8) | (input << 8);}
*/