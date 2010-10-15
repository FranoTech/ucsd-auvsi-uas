#pragma once

using namespace System::IO::Ports;
using namespace System;
using namespace System::Runtime::InteropServices;

namespace Communications
{
	union intfloat
	{
		__int32 i; 
		float f;
	};
	
	struct ComportUpstream
	{
		ComportUpstream();

		intfloat gps_lat_gimbal_x;
		intfloat gps_lon_gimbal_y;
		intfloat gps_alt;
		__int32 camera_zoom;
		char update_type;
	};

	struct ComportDownstream
	{
		__int32 time_offset;
		float gps_lat;
		float gps_lon;
		float gps_alt;
		float airplane_roll;
		float airplane_pitch;
		float airplane_heading;
		__int32 gimbal_azimuth;
		__int32 gimbal_elevation;
		float gimbal_heading;
		__int32 camera_zoom;
		char error_code;
	};

	float decodeZoomFloat( __int32 zoomRaw );
	int decodeZoomInt( __int32 zoomRaw );

	delegate void comportUpdateDelegate( ComportDownstream * data );
	delegate void comportNoDataDelegate( void );

	ref class Comport
	{
	public:
		Comport(Object ^ parent);
		~Comport(void);

		bool connect();
		bool disconnect();

		array<String ^> ^ getPortNames();

		bool setPortName( String ^ portName );
		bool setBaudRate( int baudRate );

		void readThread(void);

		void readData(void);
		void writeData( ComportUpstream * data );
		bool isConnected(void) { return _serialPort->IsOpen; }

	private:
		int decodeByte(void);
		unsigned char encodeByte( unsigned char data );
		inline bool isSpecialByte( unsigned char data );
		__int16 calculateChecksum( array<System::Byte> ^data, int packetSize );
		
	protected:
		SerialPort ^ _serialPort;
		array<String ^> ^ data;
		array<String ^> ^ portNames;
		Object ^ parent;
			
		comportUpdateDelegate ^ comDelegate;
		comportNoDataDelegate ^ comNoDataDelegate;
	};
}
