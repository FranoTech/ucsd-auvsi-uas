


#include "StdAfx.h"
#include "SimHandler.h"
#include "TelemetrySimulator.h"


#include "Comport.h"
#include <time.h>
#include <iomanip>    // Needed for stream modifiers fixed and setprecision
#include <string>

#include "MasterHeader.h"
#include "Comms.h"

using namespace std;
using namespace Simulator;
using namespace Communications;
using namespace System;
using namespace System::Text;
using namespace System::Windows::Forms;
using namespace System::IO;


// video write frame frequency (in milliseconds) (for 30 fps)
#define WAIT_INTERVAL		0
#define SHORT_WAIT_INTERVAL	2

#define FRAME_FREQUENCY		1000 / VIDEO_FRAMERATE


SimHandler::SimHandler(VideoSimulator ^ vidSim, OpenGLForm::COpenGL ^ opunGL)
{
	recordTelemetry = false;
	recordVideo = false;
	theVideoSimulator = vidSim;
	openGLView = opunGL; //opUn is spelled as intended.
	fileO = new ofstream();
	fileI = NULL;

	theTelSimulator = gcnew TelemetrySimulator(this, nullptr, nullptr);
}

SimHandler::~SimHandler()
{
	delete(fileO);
	fileO = NULL;



}

	//True = Comport's calls will result in telemetry data being recorded.
	//False = calls will result in return.

	/*void setRecordTelemetryBool(bool shouldRecordT)
	{
		recordTelemetry = shouldRecordT;
	}
	void setRecordVideoBool(bool shouldRecordV);
	{
		recordVideo = shouldRecordV;
	}*/
	/*
	 * bool runVideo();
	 * bool runTelemetry();
	 */

	/**
	 * Calls OpenGL's savevideo()
	 * and initializes fileStream for telemetry.
	 */
int SimHandler::beginRecording(String ^ filename)
{
	int retStatus = ALL_FAILED;

	recordVideo = false;

	firstPacket = true;
	pleaseRecord = true;
	beginTelemetry(filename);
	retStatus += RECORDING_TELEM;
	


	if (videoWriteThread != nullptr) {
		// stop, then wait
		System::Diagnostics::Trace::WriteLine("saveVideoFrame called from SimHandler: " + numframes + " in " + (System::DateTime::Now.Subtract(before)).Milliseconds);

		breakNow = true;
		Thread::Sleep( 30 ); // ms

		videoWriteThread->Abort();
	}



	if (!openGLView->enableVideoRecording(filename)) {
		breakNow = false;
		videoAttemptThread = gcnew Thread(gcnew ParameterizedThreadStart(this, &SimHandler::tryToStartVideo));
		videoAttemptThread->Name = "SimHandler tryToStartVideo Thread";
		videoAttemptThread->Start(filename);

	}

	else 
	{
			
		
		// make thread that reads frame 30 times per second
		breakNow = false;
		videoWriteThread = gcnew Thread(gcnew ThreadStart(this, &SimHandler::writeVideo));
		videoWriteThread->Name = "SimHandler Video Write Thread";
		videoWriteThread->Start();

		retStatus += RECORDING_VIDEO;
		recordVideo = true;

		System::Diagnostics::Trace::WriteLine("beginRecording in SimHandler");


	}

	return retStatus;
}



void SimHandler::tryToStartVideo(Object ^ arg)
{
	try {
		String^ filename = ((String^) arg)+".avi";
		while (!breakNow)
		{
			// try to start video
			if (openGLView->enableVideoRecording(filename)) {
				breakNow = false;
				videoWriteThread = gcnew Thread(gcnew ThreadStart(this, &SimHandler::writeVideo));
				videoWriteThread->Name = "SimHandler Video Write Thread";
				videoWriteThread->Start();

				Encoding^ u8 = Encoding::UTF8;

				// TODO: write to telemetry file that video started
				array<Byte> ^ fileArray = u8->GetBytes(filename);
				writeTelemetry(System::DateTime::Now.Subtract(((TelemetrySimulator^)theTelSimulator)->time), STARTVIDEO, fileArray->Length ,fileArray);

				// TODO: alert user that video started
				((Comms ^)theComms)->printToConsole("Started recording video", Color::Green);
				System::Diagnostics::Trace::WriteLine("Video started written to Telemetry");

				recordVideo = true;
				return;
			}


			// sleep for one second
			Thread::Sleep( 10000 );

		}
	} catch (Exception ^) {}

}

void SimHandler::endRecording()
{
	if(recordVideo)
	{
			
		if (videoWriteThread != nullptr) {
			// stop, then wait

			breakNow = true;
			Thread::Sleep( 30 ); // ms

			videoWriteThread->Abort();
		}


		openGLView->disableVideoRecording();

		System::Diagnostics::Trace::WriteLine("endRecording in SimHandler");
	}

	else 
	{
		// kills the "attempt to start video" thread
		breakNow = true;
		videoAttemptThread->Abort();

	}


	if(recordTelemetry)
	{
		endTelemetry();
		pleaseRecord=false;
	}
	//if recordTelem call endTelem()
}

	/**
	 * Calls OpenGL's stopVideo();
	 * Call endTelemetry();
	 */
	/*void endVideo();
	{
		//if recordvideo call end in OpenGL
		recordVideo = false;
		//if recordTelem call endTelem()
		recordTelemetry = false;
	}*/
	/**
	 * Begins feeding video AS IF its coming from the plane.
	 * Begins feeding telemetry data from file.
	 *	Needs to be done at correct intervals using a thread.
	 */
			//UNIMPLEMENTED
	//void feedVideo();

void
SimHandler::writeVideo() {
	// saveVideoFrame()

	numframes = 0;
	before = System::DateTime::Now;

	try {
		System::DateTime last = System::DateTime::Now;
		System::DateTime first = System::DateTime::Now;
		int counter = 0;

		
		System::Diagnostics::Trace::WriteLine("writeVideo() in SimHandler");

		// main video reading loop
		while (true) {


			try {
		
				// check to see if we should stop
				if (breakNow)
					break;

				// wait until 33 ms comes up
				while ((System::DateTime::Now.Subtract(last)).Milliseconds < FRAME_FREQUENCY) {
					Thread::Sleep( SHORT_WAIT_INTERVAL );
				}

				// save time that we are reading this frame
				last = last.AddMilliseconds( FRAME_FREQUENCY );
		
				// check to see if we should stop
				if (breakNow)
					break;

				// save video frame
				openGLView->saveVideoFrame();
				numframes++;
				//

			} catch (ThreadInterruptedException ^) {}
		}
	} catch (Exception ^ ) {
		//System::Diagnostics::Trace::WriteLine("Exception in writeVideo() in SimHandler");
		

	}
}

void SimHandler::writeTelemetry( System::TimeSpan time,  int type, int length, array<System::Byte>^ byteArray )
{
	//System::Diagnostics::Trace::WriteLine("WRITE TELEMETRY "+pleaseRecord);
	if(pleaseRecord)
	{
		*fileO << time.TotalSeconds << ", ";
		*fileO << type << ", ";
		*fileO << length << ", ";
		int x;
		for(x=0; x<length;x++)
		{
			*fileO << byteArray[x];
			firstPacket = false;
		}
		*fileO << endl << endl;
		
		fileO->flush();
	}
}
	//get ofstream stuff from Comport

/*private:
	ofstream fileO;
	ifstream fileI;
	bool recordTelemetry;
	bool recordVideo;*/
	/**
	 * Should flush & close the stream writing to telemetry file.
	 */
void SimHandler::endTelemetry()
{
	if (fileO && fileO->is_open())
	{
			fileO->close();
	}
	pleaseRecord=false;
	recordTelemetry = false;
	((TelemetrySimulator ^) theTelSimulator)->stopRecording();
}
/**
	* Initializes Telemetry Stream and names it to video filename.
	* append .telemetry to filename resulting in filename.telemetry
	*/
void SimHandler::beginTelemetry(String ^ filename)
{
	if (fileO && fileO->is_open())
	{
		fileO->close();
	}
	
	fileO->open(ManagedToSTL(filename+".telemetry.txt"));
	((TelemetrySimulator ^) theTelSimulator)->beginRecording();
	pleaseRecord=true;
	recordTelemetry = true;
}
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
	//void pauseVideo();
	//void resumeVideo();

//void SimHandler::write(System::TimeSpan theOffset, int type, int length, array<System::Byte> ^ byteArray) {} // TODO: finish implementing