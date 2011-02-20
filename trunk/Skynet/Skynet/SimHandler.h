#pragma once

#include "VideoSimulator.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;



#define ALL_FAILED			0
#define RECORDING_VIDEO		1
#define RECORDING_TELEM		2
#define RECORDING_ALL		3


#define RABBIT 0
#define AUTOPILOT 1
#define UNKNOWN 2
#define STARTVIDEO 3
#define ENDVIDEO 4

namespace Simulator
{
	ref class SimHandler
	{
	public:
		int numframes;
		System::DateTime before;
		Object ^ theTelSimulator; // telemetry
		Object ^ theComms;

		SimHandler(VideoSimulator ^ vidSim, OpenGLForm::COpenGL ^ opunGL);
		~SimHandler();
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
		int beginRecording(String ^ filename);
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
		void tryToStartVideo(Object ^ arg);
		void writeTelemetry( System::TimeSpan time,  int type, int length, array<System::Byte>^ byteArray);
		/*
		 * IF recordTelemetry
			write to file;
		   else
			return;
		 */
		//get ofstream stuff from Comport

		//void write(System::TimeSpan theOffset, int type, int length, array<System::Byte> ^ byteArray); // TODO: implement
	private:
		bool firstPacket;
		ofstream * fileO;
		ifstream * fileI;
		bool recordTelemetry;
		bool recordVideo;
		bool pleaseRecord;
		bool breakNow;

		Simulator::VideoSimulator ^ theVideoSimulator;
		OpenGLForm::COpenGL ^ openGLView;
		Thread ^ videoAttemptThread;
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