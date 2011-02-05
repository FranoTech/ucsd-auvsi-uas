


#include "StdAfx.h"
#include "SimHandler.h"
#include "TelemetrySimulator.h"


#include "Comport.h"
#include <time.h>
#include <iomanip>    // Needed for stream modifiers fixed and setprecision
#include <string>

#include "MasterChiefHeaderFile.h"

using namespace std;
using namespace Simulator;
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
	recordTelemetry = true;
	recordVideo = true;
	theVideoSimulator = vidSim;
	openGLView = opunGL; //opUn is spelled as intended.
	fileO = new ofstream();
	fileI = NULL;

	theTelSimulator = gcnew TelemetrySimulator(this, nullptr, nullptr);
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
bool SimHandler::beginRecording(String ^ filename)
{
	if(recordVideo)
	{
		if (videoWriteThread != nullptr) {
			// stop, then wait
			System::Diagnostics::Trace::WriteLine("saveVideoFrame called from SimHandler: " + numframes + " in " + (System::DateTime::Now.Subtract(before)).Milliseconds);

			breakNow = true;
			Thread::Sleep( 30 ); // ms

			videoWriteThread->Abort();
		}



		if (!openGLView->enableVideoRecording(filename)) {
			// SOMETHING WENT WRONG

			return false;
		}
		
		// make thread that reads frame 30 times per second
		breakNow = false;
		videoWriteThread = gcnew Thread(gcnew ThreadStart(this, &SimHandler::writeVideo));
		videoWriteThread->Name = "SimHandler Video Write Thread";
		videoWriteThread->Start();

		System::Diagnostics::Trace::WriteLine("beginRecording in SimHandler");
	}
	if(recordTelemetry)
	{


		/*theTelSimulator->setRabbit(theComms->rabbit);
		theTelSimulator->setAutopilot(theComms->autopilot);*/ // used only when playing back simulation info


		firstPacket = true;
		beginTelemetry(filename);
	}

	//if recordVideo call OpenGL
	//if recordTelem call beginTelemetry();

	return true;
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
	if(pleaseRecord)
	{
		//*file0 << time;
		*fileO << type;
		*fileO << length;
		int x;
		for(x=0; x<length;x++)
		{
			*fileO << byteArray[x];
			firstPacket = false;
		}
		*fileO << endl;
		
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
	pleaseRecord=true;
	fileO->open(ManagedToSTL(filename+".telemetry.txt"));
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