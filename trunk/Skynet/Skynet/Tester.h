#pragma once

namespace Skynet {

	public ref class Tester {
	public:
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

			testGeoreferencing();


			testPrediction();

		}
	};



}