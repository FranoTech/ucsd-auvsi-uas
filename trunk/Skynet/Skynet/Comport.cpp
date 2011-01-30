#include "StdAfx.h"
#include "Form1.h"
#include "Comport.h"
#include <time.h>
#include "SimHandler.h"
#include "ComportHandler.h

/*
 * Serial use bytes
 * We use the same method as defined on page 15 of the Kestrel
 * communications manual
 */
 
#define START_BYTE 		0xFF
#define END_BYTE		0xFE
#define SPECIAL_BYTE 	0xFD
#define FUTURE_BYTE		0xCC

#define MAP_TO_START	0x00
#define MAP_TO_END		0x01
#define MAP_TO_SPECIAL	0x02
#define MAP_TO_FUTURE	0x03

#define BAUD_RATE		115200

// Comport read timeout in miliseconds
#define TIMEOUT_MS		1000

// Comport update frequency (in miliseconds)
#define UPDATE_FREQ 250

// buffer size in bytes to read in to
#define BUFFER_SIZE		128

using namespace Communications;
using namespace System::Threading;

//static ComportUpstream * lastPacket;


ComportUpstream::ComportUpstream()
{
	gps_lat_gimbal_x.i = 0;
	gps_lon_gimbal_y.i = 0;
	gps_alt.i = 0;
	camera_zoom = 0xAAAAAAAA;
	update_type = 0;
}

Comport::Comport(Object ^ theParent)
{
	_serialPort = gcnew SerialPort();
	_serialPort->ReadTimeout = TIMEOUT_MS;
	_serialPort->BaudRate = BAUD_RATE;

	portNames = SerialPort::GetPortNames();

	parent = theParent;

	// DEBUG: removed the following
	comDelegate = gcnew comportUpdateDelegate(((ComportHandler ^)theParent), &ComportHandler::receiveData );
}

Comport::~Comport(void)
{
	disconnect();
}

array<String ^> ^ Comport::getPortNames()
{
	return portNames;
}

bool Comport::connect()
{
	bool success = false;
	if( !_serialPort->IsOpen ) 
	{
		// try ... catch, and check to see if comport exists first
		try 
		{
			_serialPort->Open();
			success = true;
		}
		catch( Exception ^ theException)
		{
			System::Diagnostics::Trace::WriteLine("Exception ... could not connect");
			success = false;
		}
	} 
	else 
	{
		System::Diagnostics::Trace::WriteLine("Serial port already open");
	}

	return success;

}

bool Comport::disconnect(void)
{
	bool success = false;
	if( _serialPort->IsOpen ) 
	{
		_serialPort->Close();
		success = true;
	}

	return success;
}

bool Comport::setPortName( String ^ portName )
{
	bool success = false;
	if( _serialPort->IsOpen )
	{

	}
	else
	{
		_serialPort->PortName = portName;
		success = true;
	}
	return success;
}

bool Comport::setBaudRate( int baudRate )
{
	bool success = false;
	if( _serialPort->IsOpen )
	{

	}
	else
	{
		_serialPort->BaudRate = baudRate;
		success = true;
	}
	return success;
}

int Comport::decodeByte(void)
{
	int dataIn = -1;

	try
	{
		dataIn = _serialPort->ReadByte();

		if( dataIn >= 0 )
		{
			// Correct the byte here			
			switch( Convert::ToByte( dataIn ) )
			{
			case MAP_TO_START:
				dataIn = START_BYTE;
				break;
			case MAP_TO_END:
				dataIn = END_BYTE;
				break;
			case MAP_TO_SPECIAL:
				dataIn = SPECIAL_BYTE;
				break;
			case MAP_TO_FUTURE:
				dataIn = FUTURE_BYTE;
				break;
			default:
				break;
			}
		}
	}
	catch( Exception ^ )
	{
		dataIn = -1;
		System::Diagnostics::Trace::WriteLine("catch in comport");
	}

	return dataIn;
}

unsigned char Comport::encodeByte( unsigned char data )
{
	// FF = 0
	// FE = 1
	// FD = 2
	// CC = 3
	switch( data )
	{
		case 0xFF: return 0x0;
		case 0xFE: return 0x1;
		case 0xFD: return 0x2;
		case 0xCC: return 0x3;
		default : return 0x4; // should never reach
	}
}

void Comport::readThread(void)
{
	while( true )
	{

		// only read data if we are connected
		if (isConnected())
			readData();

		Thread::Sleep( UPDATE_FREQ );
	}

}

void Comport::readData(void)
{
	const int NUM_ELEMENTS = 11;
	int dataIn = -1;
	int bufLen = 0;
	byte tempByte;

	array<System::Byte> ^ buffer = gcnew array<System::Byte>(BUFFER_SIZE);
	//ComportDownstream * packet = new ComportDownstream();

	try
	{
		// Read until we get a start byte
		while( true )
		{
			dataIn = _serialPort->ReadByte();
			tempByte = Convert::ToByte( dataIn );

			if( dataIn < 0 )
			{
				_serialPort->DiscardInBuffer();
				return;
			}
			else if( tempByte == START_BYTE )
			{
				buffer[bufLen] = tempByte;
				bufLen++;
				break;
			}
		}

		// Read in the message string, decoding as necessary
		while( true )
		{
			dataIn = _serialPort->ReadByte();
			tempByte = Convert::ToByte( dataIn );

			// error / end of data check
			if( dataIn < 0 )
			{
				_serialPort->DiscardInBuffer();
				return;
			}
			else if( tempByte == END_BYTE )
			{
				buffer[bufLen] = tempByte;
				bufLen++;
				break;
			}

			// process actual byte
			// if this is a special byte, decode it
			if( tempByte == SPECIAL_BYTE )
			{
				dataIn = decodeByte();

				if( dataIn < 0 )
				{
					_serialPort->DiscardInBuffer();
					return;
				}
				else
					tempByte = Convert::ToByte( dataIn );
			}

			buffer[bufLen] = tempByte;
			bufLen++;


		}


		__int16 checksum = calculateChecksum( buffer, bufLen - 3);

		//System::Diagnostics::Trace::WriteLine("Checksum: " + String::Format("{0:X}", checksum));

		if (!( ((unsigned char *)&checksum)[1] == buffer[bufLen - 3] && ((unsigned char *)&checksum)[0] == buffer[bufLen - 2] ))
		{
			//System::Diagnostics::Trace::WriteLine("Checksum: FAILED");
			_serialPort->DiscardInBuffer();
			return;
 		}

		/*ComportDownstream * packet = new ComportDownstream();

		char * dataPtr = (char *)packet;

		for (int j = 0; j < NUM_ELEMENTS; j++) {
			for (int i = 3, k = 0; i > -1; i--, k++) {
				int letterPosition = j*4 + k;
				dataPtr[letterPosition] = buffer[j*4 + i + 1];

			}			
		}*/

		//save time that packet came in

		//write to files
		//packet->gps_lat;

		//read file

		/*packet->error_code = buffer[bufLen - 4];

		if (false && packet->error_code) {
			System::Diagnostics::Trace::WriteLine("autopilot timeout \t     " + (bool)(packet->error_code & 0x01));
			System::Diagnostics::Trace::WriteLine("autopilot invalid packet " + (bool)(packet->error_code & 0x02));
			System::Diagnostics::Trace::WriteLine("ground station timeout   " + (bool)(packet->error_code & 0x04));
			System::Diagnostics::Trace::WriteLine("uplink timeout           " + (bool)(packet->error_code & 0x08));
			System::Diagnostics::Trace::WriteLine("uplink invalid packet    " + (bool)(packet->error_code & 0x10));
			System::Diagnostics::Trace::WriteLine("gimbal timeout           " + (bool)(packet->error_code & 0x20));
			System::Diagnostics::Trace::WriteLine("gimbal invalid mode      " + (bool)(packet->error_code & 0x40));
			System::Diagnostics::Trace::WriteLine("reserved                 " + (bool)(packet->error_code & 0x80));
			System::Diagnostics::Trace::WriteLine("\n");
		}
		*/
		// Verify checksum
		

		
	//	((Simulator::SimHandler ^)theSimHandler)->writeTelemetry(packet);

		// Callback to Form1 (stays in this offshoot thread)

		// copy packet into new packet
		array<System::Byte> ^ outBuffer = gcnew array<System::Byte>(bufLen);
		for (int i = 0; i < bufLen; i++) {
			outBuffer[i] = buffer[i];
		}

		// send data to delegate
		comDelegate( outBuffer );

		//JK..WRITE HERE!!

		//gcroot<ComportDownstream *> bla(packet);
		//((Skynet::Form1 ^)parent)->Invoke( ((Skynet::Form1 ^)parent)->comportUpdateDelegate, bla );
		// In this case packet is deleted by the main thread

		// clear the rest of the buffer
		_serialPort->DiscardInBuffer();
	}
	catch( Exception ^ )
	{
		delete packet;
		// TODO object disposed exception
		((Skynet::Form1 ^)parent)->Invoke( ((Skynet::Form1 ^)parent)->comportErrorDelegate );
		//System::Diagnostics::Trace::WriteLine("catch in comport");
		//comNoDataDelegate();
	}
}

__int16 Comport::calculateChecksum( array<System::Byte> ^data, int packetSize ) 
{
	__int16 CheckValue = 0; 

	for (int i = 1; i < packetSize - 1; i+= 2)
	{
		CheckValue ^= data[i]<<8;
		CheckValue ^= data[i+1];
	}

	if ((packetSize - 1)%2)
		CheckValue ^= data[packetSize - 1]<<8;

	return CheckValue;

}

array<System::Byte> ^ Comport::readRawData( int timeout ) {

	// set new timeout
	//int oldtimeout = _serialPort->ReadTimeout;
	//_serialPort->ReadTimeout = timeout;



	const int NUM_ELEMENTS = 11;
	int dataIn = -1;
	int bufLen = 0;
	byte tempByte;

	array<System::Byte> ^ buffer = gcnew array<System::Byte>(BUFFER_SIZE);

	try
	{
		// Read until we get a start byte
		while( true )
		{
			dataIn = _serialPort->ReadByte();
			tempByte = Convert::ToByte( dataIn );

			if( dataIn < 0 )
			{
				_serialPort->DiscardInBuffer();
				return nullptr;
			}
			else if( tempByte == START_BYTE )
			{
				buffer[bufLen] = tempByte;
				bufLen++;
				break;
			}
		}

		// Read in the message string, decoding as necessary
		while( true )
		{
			dataIn = _serialPort->ReadByte();
			tempByte = Convert::ToByte( dataIn );

			// error / end of data check
			if( dataIn < 0 )
			{
				_serialPort->DiscardInBuffer();
				return nullptr;
			}
			else if( tempByte == END_BYTE )
			{
				buffer[bufLen] = tempByte;
				bufLen++;
				break;
			}

			// process actual byte
			// if this is a special byte, decode it
			if( tempByte == SPECIAL_BYTE )
			{
				dataIn = decodeByte();

				if( dataIn < 0 )
				{
					_serialPort->DiscardInBuffer();
					return nullptr;
				}
				else
					tempByte = Convert::ToByte( dataIn );
			}

			buffer[bufLen] = tempByte;
			bufLen++;


		}

		
		// calculate checksum
		__int16 checksum = calculateChecksum( buffer, bufLen - 3);

		/*if (!( ((unsigned char *)&checksum)[1] == buffer[bufLen - 3] && ((unsigned char *)&checksum)[0] == buffer[bufLen - 2] ))
		{
			//System::Diagnostics::Trace::WriteLine("Checksum: FAILED");
			_serialPort->DiscardInBuffer();
			return;
 		}*/

		// Verify checksum
		


		// clear the rest of the buffer
		_serialPort->DiscardInBuffer();
	}
	catch( Exception ^ )
	{
	}




	// restore old timeout
	//_serialPort->ReadTimeout = oldtimeout;


	
	array<System::Byte> ^ retbuffer = gcnew array<System::Byte>(bufLen);

	for (int i = 0; i < bufLen; i++)
		retbuffer[i] = buffer[i];

	return retbuffer;
}

void Comport::writeEncodedData( array<System::Byte> ^ inBuffer ) 
{
	if (!isConnected())
		return;


	array<System::Byte> ^ buffer = gcnew array<System::Byte>(BUFFER_SIZE);
	array<System::Byte> ^ bufferNoSpecial = gcnew array<System::Byte>(BUFFER_SIZE);
	int bufLen = 0;
	int bufNoSpecialLen = 0;
	char outByte;

	try
	{
		// start byte
		buffer[bufLen] = START_BYTE;
		bufferNoSpecial[bufNoSpecialLen] = START_BYTE;
		bufNoSpecialLen++;
		bufLen++;

		
		// encode packet
		for (int j = 0; j < inBuffer->Length; j ++) {
			outByte = inBuffer[j];

			bufferNoSpecial[bufNoSpecialLen] = outByte;
			bufNoSpecialLen++;

			if (isSpecialByte(outByte)) {
				buffer[bufLen] = SPECIAL_BYTE;
				bufLen++;
				buffer[bufLen] = encodeByte(outByte);
				bufLen++;
			} else {
				buffer[bufLen] = outByte;
				bufLen++;
			}
		}		


		// calculate checksum
		__int16 checksum = calculateChecksum( bufferNoSpecial, bufNoSpecialLen );
		
		outByte = ((char *)&checksum)[1];
		if (isSpecialByte(outByte)) {
			buffer[bufLen] = SPECIAL_BYTE;
			bufLen++;
			buffer[bufLen] = encodeByte(outByte);
			bufLen++;
		} else {
			buffer[bufLen] = outByte;
			bufLen++;
		}

		outByte = ((char *)&checksum)[0];
		if (isSpecialByte(outByte)) {
			buffer[bufLen] = SPECIAL_BYTE;
			bufLen++;
			buffer[bufLen] = encodeByte(outByte);
			bufLen++;
		} else {
			buffer[bufLen] = outByte;
			bufLen++;
		}

		// end byte
		buffer[bufLen] = END_BYTE;
		bufLen++;
		

		// send it
		_serialPort->Write( buffer, 0, bufLen );		
	}
	catch( Exception ^ )
	{
		// no valid data, no big deal
		System::Diagnostics::Trace::WriteLine("catch in Comport::writeEncodedData( array<System::Byte> ^ buffer ) ");

	}

}


void Comport::writeRawData( array<System::Byte> ^ buffer )
{
	if (!isConnected())
		return;

	
	System::Diagnostics::Trace::WriteLine( "Comport: writeRawData" );
	_serialPort->Write( buffer, 0, buffer->Length );	
}


// SEND IN BIG-ENDIAN (x86 stores in little-Endian)
void Comport::writeData( ComportUpstream * data )
{
	//System::Diagnostics::Trace::WriteLine("DATA TO WRITE TO COMPORT : ");
	//for( int i=0; i<21; i++){
	//	System::Diagnostics::Trace::WriteLine(data[i] + " ");
	//}
	//System::Diagnostics::Trace::WriteLine("\n");
	
	//lastPacket

	if (!isConnected())
		return;

	array<System::Byte> ^ buffer = gcnew array<System::Byte>(BUFFER_SIZE);
	array<System::Byte> ^ bufferNoSpecial = gcnew array<System::Byte>(BUFFER_SIZE);
	int bufLen = 0;
	int bufNoSpecialLen = 0;
	char outByte;

	try
	{
		// start byte
		buffer[bufLen] = START_BYTE;
		bufferNoSpecial[bufNoSpecialLen] = START_BYTE;
		bufNoSpecialLen++;
		bufLen++;

		// encode packet 
		//outByte = &(char *)data[0];
		
		char * thePointer = (char *)&data->gps_lat_gimbal_x;
		for (int j = 0; j < 4; j ++) {
			for (int i = 3; i >= 0; i --) {
				outByte = thePointer[i + j*4];

				bufferNoSpecial[bufNoSpecialLen] = outByte;
				bufNoSpecialLen++;

				if (isSpecialByte(outByte)) {
					buffer[bufLen] = SPECIAL_BYTE;
					bufLen++;
					buffer[bufLen] = encodeByte(outByte);
					bufLen++;
				} else {
					buffer[bufLen] = outByte;
					bufLen++;
				}
			}
		}		
		
		// Handle non word sized data to send
		outByte = data->update_type;

		bufferNoSpecial[bufNoSpecialLen] = outByte;
		bufNoSpecialLen++;

		if (isSpecialByte(outByte)) {
			buffer[bufLen] = SPECIAL_BYTE;
			bufLen++;
			buffer[bufLen] = encodeByte(outByte);
			bufLen++;
		} else {
			buffer[bufLen] = outByte;
			bufLen++;
		}		

		// calculate checksum
		__int16 checksum = calculateChecksum( bufferNoSpecial, bufNoSpecialLen );
		
		outByte = ((char *)&checksum)[1];
		if (isSpecialByte(outByte)) {
			buffer[bufLen] = SPECIAL_BYTE;
			bufLen++;
			buffer[bufLen] = encodeByte(outByte);
			bufLen++;
		} else {
			buffer[bufLen] = outByte;
			bufLen++;
		}

		outByte = ((char *)&checksum)[0];
		if (isSpecialByte(outByte)) {
			buffer[bufLen] = SPECIAL_BYTE;
			bufLen++;
			buffer[bufLen] = encodeByte(outByte);
			bufLen++;
		} else {
			buffer[bufLen] = outByte;
			bufLen++;
		}
		
		/*
		buffer[bufLen] = data->update_type;
		bufLen++;

		buffer[bufLen] = 0x12;
		bufLen++;

		buffer[bufLen] = 0x13;
		bufLen++;
		*/

		// end byte
		buffer[bufLen] = END_BYTE;
		bufLen++;
		
		//for(int i=0;i<bufNoSpecialLen;i++){
		//	System::Diagnostics::Trace::WriteLine("predec " + i + " : " + String::Format("{0:X}", bufferNoSpecial[i]));
		//}

		//System::Diagnostics::Trace::WriteLine("\nchecksum : " + String::Format("{0:X}", checksum) + "\n");

		//for(int i=0;i<bufLen;i++){
		//	System::Diagnostics::Trace::WriteLine("Buff " + i + " : " + String::Format("{0:X}", buffer[i]) + "\n");
		//}
		

		// send it
		_serialPort->Write( buffer, 0, bufLen );		
	}
	catch( Exception ^ )
	{
		// no valid data, no big deal
		//System::Diagnostics::Trace::WriteLine("catch in comport");

	}

	// Delete input data
	//delete data;
}

bool Comport::isSpecialByte( unsigned char data )
{
	/*System::Diagnostics::Trace::Write(" ");
	if ((data ^ 0xff) == 0)
		System::Diagnostics::Trace::Write(" ");
	else
		System::Diagnostics::Trace::Write(" ");

	if ((data ^ 0xfe) == 0)
		System::Diagnostics::Trace::Write(" ");
	else
		System::Diagnostics::Trace::Write(" ");*/
	
	return (data == START_BYTE || data == END_BYTE || data == SPECIAL_BYTE || data == FUTURE_BYTE );
}

float
Communications::decodeZoomFloat( __int32 zoomRaw )
{
	float zoom = 1;

	switch( zoomRaw )
	{
		case 0x00000000:
			zoom = 1.0f;
			break;
		case 0x00080000:
			zoom = 1.2f;
			break;
		case 0x01000000:
			zoom = 1.5f;
			break;
		case 0x01080000:
			zoom = 1.9f;
			break;
		case 0x02000000:
			zoom = 2.5f;
			break;
		case 0x02080000:
			zoom = 3.4f;
			break;
		case 0x03000000:
			zoom = 4.8f;
			break;
		case 0x03080000:
			zoom = 6.8f;
			break;
		case 0x04000000:
			zoom = 10.1f;
			break;
		default:
			zoom = -1.0f;
			break;
	}

	return zoom;
}

int
Communications::decodeZoomInt( __int32 zoomRaw )
{
	int zoom = 1;

	switch( zoomRaw )
	{
		case 0x00000000:
			zoom = 1;
			break;
		case 0x00080000:
			zoom = 2;
			break;
		case 0x01000000:
			zoom = 3;
			break;
		case 0x01080000:
			zoom = 4;
			break;
		case 0x02000000:
			zoom = 5;
			break;
		case 0x02080000:
			zoom = 6;
			break;
		case 0x03000000:
			zoom = 7;
			break;
		case 0x03080000:
			zoom = 8;
			break;
		case 0x04000000:
			zoom = 9;
			break;
		default:
			zoom = -1;
			break;
	}

	return zoom;
}