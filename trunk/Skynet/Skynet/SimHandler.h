#pragma once


#include "VideoSimulator.h"
#include "Comport.h"
#include <fstream>
#include <iostream>

using namespace std;

namespace Simulator
{
	ref class SimHandler
	{
	public:
		int numframes;
		System::DateTime before;

		SimHandler(Communications::Comport ^ comPort, Simulator::VideoSimulator ^ vidSim, OpenGLForm::COpenGL ^ opunGL)
		{
			recordTelemetry = true;
			recordVideo = true;
			theComport = comPort;
			theVideoSimulator = vidSim;
			openGLView = opunGL; //opUn is spelled as intended.
			fileO = new ofstream();
			fileI = NULL;
		}
		~SimHandler()
		{
			delete(fileO);
			fileO = NULL;
		}
		//True = Comport's calls will result in telemetry data being recorded.
		//False = calls will result in return.

		void setRecordTelemetryBool(bool shouldRecordT)
		{
			recordTelemetry = shouldRecordT;
		}
		void setRecordVideoBool(bool shouldRecordV)
		{
			recordVideo = shouldRecordV;
		}
		/*
		 * bool runVideofileO->
		 * bool runTelemetryfileO->
		 */

		/**
		 * Calls OpenGL's savevideo()
		 * and initializes fileStream for telemetry.
		 */
		void beginRecording(String ^ filename);
			//if recordVideo call OpenGL
			//if recordTelem call beginTelemetryfileO->
	
		/**
		 * Calls OpenGL's stopVideofileO->
		 * Call endTelemetryfileO->
		 */
		void endRecording();
		/**
		 * Begins feeding video AS IF its coming from the plane.
		 * Begins feeding telemetry data from file.
		 *	Needs to be done at correct intervals using a thread.
		 */
				//UNIMPLEMENTED
		//void feedVideofileO->

		void writeTelemetry(Communications::ComportDownstream * packet);
		/*
		 * IF recordTelemetry
			write to file;
		   else
			return;
		 */
		//get ofstream stuff from Comport

	private:
		bool firstPacket;
		ofstream * fileO;
		ifstream * fileI;
		bool recordTelemetry;
		bool recordVideo;
		bool pleaseDontRecord;
		bool breakNow;

		Communications::Comport ^ theComport;
		Simulator::VideoSimulator ^ theVideoSimulator;
		OpenGLForm::COpenGL ^ openGLView;
		
		Thread ^ videoWriteThread;
		/**
		 * Should flush & close the stream writing to telemetry file.
		 */
		void endTelemetry();
		/**
		 * Initializes Telemetry Stream and names it to video filename.
		 * append .telemetry to filename resulting in filename.telemetry
		 */
		void beginTelemetry(String ^ filename);
		void writeVideo(); // video writing run loop


	protected:
		

		std::string ManagedToSTL(String ^ s) 
		{
		   using namespace Runtime::InteropServices;
		   const char* chars = 
		      (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
		   std::string retVal = chars;
		   Marshal::FreeHGlobal(IntPtr((void*)chars));

		   return retVal;
		}
	};
	/**
	 * UNIMPLEMENTED/FUTURE USE
	 */
	/**
	 * public:
	 */
		/**
		 * Should PAUSE/UNPAUSE video recording in OpenGL.
		 */
		//Unimplemented.
		//void pauseVideofileO->
		//void resumeVideofileO->
}