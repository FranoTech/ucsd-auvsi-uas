#pragma once

#include "Saliency.h"

namespace Skynet {

	public ref class Tester {
	public:
		static void testSaliency()
		{
			System::Diagnostics::Trace::WriteLine("Tester::testSaliency() STARTING test ");

			Vision::Saliency ^ sal = gcnew Vision::Saliency();
			sal->runTestOnImageNamed("C:\\Users\\UCSD\\Pictures\\Saliency Sample\\heli.bmp");//"C:\\Users\\UCSD\\Desktop\\test.png");

			System::Diagnostics::Trace::WriteLine("Tester::testSaliency() FINISHED with test");
		}

		static void testGeoreferencing() 
		{
			System::Diagnostics::Trace::WriteLine("Tester::testGeoreferencing() STARTING test");
			Vision::GeoReference::runTests();
			System::Diagnostics::Trace::WriteLine("Tester::testGeoreferencing() FINISHED with test");
		}

		static void testPrediction()
		{
			System::Diagnostics::Trace::WriteLine("Tester::testPrediction() STARTING test ... not implemented yet");


			System::Diagnostics::Trace::WriteLine("Tester::testPrediction() FINISHED with test");
		}

		static void runTests() 
		{
			//return;
			//testGeoreferencing();


			//testPrediction();

			//testSaliency();

		}
	};



}