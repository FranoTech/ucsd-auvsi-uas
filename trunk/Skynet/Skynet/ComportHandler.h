#pragma once

#include "Comport.h"

using namespace System::IO::Ports;
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;

namespace Communications
{


	ref class ComportHandler {
	public:
		ComportHandler(Object ^ newDelegate, String ^ comType);

		void connect(String ^ portName);
		void disconnect();

		//void beginReading();
		void writeData( array<System::Byte> ^ inBuffer ); 
		void receiveData( array<System::Byte> ^ inBuffer );
		void beginReading(String ^ comportType);

	protected:
		Object ^ theDelegate;
		Comport ^ thePort;
		Thread ^ comReadThread;
		String ^ type;
	};


}