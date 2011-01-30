#include "ComportHandler.h"
#include "Comport.h"


using namespace Communications;



ComportHandler::ComportHandler(Object ^ newDelegate, String ^ comType) 
{
	theDelegate = newDelegate;
	type = comType;

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

}

void ComportHandler::writeData( array<System::Byte> ^ inBuffer ) 
{
	thePort->writeEncodedData( inBuffer );
}

void ComportHandler::receiveData( array<System::Byte> ^ inBuffer ) 
{
	// do something with this data (probably up to theDelegate?)
}
