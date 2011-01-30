#pragma once

#include "ComportHandler.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Communications
{


	ref class RabbitComport : public ComportHandler {
	public:
		RabbitComport(Object ^ newDelegate) : ComportHandler(newDelegate, "Rabbit") {};

		void receiveData( array<System::Byte> ^ inBuffer ); 

	private:

	};


}