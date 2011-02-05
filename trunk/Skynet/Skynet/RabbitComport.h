#pragma once

#include "ComportHandler.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Communications
{


	ref class RabbitComport : public ComportHandler {
	public:
		RabbitComport(TelemetrySimulator ^ telSimulator, Object ^ newDelegate) : ComportHandler(telSimulator, newDelegate, "Rabbit") {};

		void receiveData( array<System::Byte> ^ inBuffer ); 

	private:

	};


}