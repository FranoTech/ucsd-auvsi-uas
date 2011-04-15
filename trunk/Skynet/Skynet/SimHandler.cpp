


#include "StdAfx.h"
#include "SimHandler.h"
#include "TelemetrySimulator.h"
#include <msclr/lock.h>


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
using namespace msclr;


// video write frame frequency (in milliseconds) (for 30 fps)
#define WAIT_INTERVAL		0
#define SHORT_WAIT_INTERVAL	2

#define FRAME_FREQUENCY		1000 / VIDEO_FRAMERATE


SimHandler::SimHandler(VideoSimulator ^ vidSim, Decklink::Callback * cb, OpenGLForm::COpenGL ^ opunGL)
{
	recordTelemetry = false;
	recordVideo = false;
	theVideoSimulator = vidSim;
	callback = cb;
	openGLView = opunGL; //opUn is spelled as intended.
	//fileO = new ofstream();
	//fileI = NULL;
	
	splitTimer = nullptr;

	theTelSimulator = gcnew TelemetrySimulator(this, nullptr, nullptr);
}

SimHandler::~SimHandler()
{
	fileO = nullptr;
}

void SimHandler::startPlayback(String ^ filename)
{
	((TelemetrySimulator ^) theTelSimulator)->startTelSim(filename);
}

void SimHandler::pausePlayback()
{
	((TelemetrySimulator ^) theTelSimulator)->pausePlayback();
	theVideoSimulator->pauseVideo();
}

void SimHandler::resumePlayback()
{
	theVideoSimulator->playVideo();
	((TelemetrySimulator ^) theTelSimulator)->resumePlayback();
}

bool SimHandler::isPaused()
{
	return ((TelemetrySimulator ^) theTelSimulator)->isPaused();
}

void SimHandler::endPlayback()
{
	theVideoSimulator->stopVideo();
	((TelemetrySimulator ^) theTelSimulator)->endRunLoop();
}



void SimHandler::startPlayingVideo(String ^ filename)
{
	theVideoSimulator->stopVideo();
	theVideoSimulator->loadVideo( (const char*)(Marshal::StringToHGlobalAnsi(filename)).ToPointer());
	theVideoSimulator->startVideo();
	callback->dontShow();
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
int SimHandler::beginRecording()
{
	int retStatus = ALL_FAILED;

	DateTime time = DateTime::Now;
	//time = time.AddSeconds( -Convert::ToInt32( SPLIT_LENGTH ) );
	path = "D:\\Skynet Files\\video\\video_" + time.ToString("o")->Replace(":", "-");

	recordVideo = false;

	firstPacket = true;
	pleaseRecord = true;
	beginTelemetry(path);
	retStatus += RECORDING_TELEM;
	


	if (videoWriteThread != nullptr) {
		// stop, then wait
		System::Diagnostics::Trace::WriteLine("saveVideoFrame called from SimHandler: " + numframes + " in " + (System::DateTime::Now.Subtract(before)).Milliseconds);

		breakNow = true;
		Thread::Sleep( 30 ); // ms

		videoWriteThread->Abort();
	}

	
	breakNow = false;
	videoAttemptThread = gcnew Thread(gcnew ParameterizedThreadStart(this, &SimHandler::tryToStartVideo));
	videoAttemptThread->Name = "SimHandler tryToStartVideo Thread";
	videoAttemptThread->Start(path);

	// alert user that video started
	((Comms ^)theComms)->printToConsole("Start Video", Color::Green);
	System::Diagnostics::Trace::WriteLine("Video start written to Telemetry");

	return retStatus;
}

void SimHandler::tryToStartVideo(Object ^ arg)
{
	breakNow = false;
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

				// write to telemetry file that video started
				array<Byte> ^ fileArray = u8->GetBytes(filename);
				writeTelemetry(System::DateTime::Now.Subtract(((TelemetrySimulator^)theTelSimulator)->time), STARTVIDEO, fileArray->Length ,fileArray);

				// begin timer
				TimerCallback^ tcb = gcnew TimerCallback(this, &SimHandler::splitVideo);
				splitTimer = gcnew Threading::Timer(tcb, nullptr, SPLIT_LENGTH*1000, SPLIT_LENGTH*1000);

				recordVideo = true;
				return;
			}


			// sleep for one second
			Thread::Sleep( 1000 );

		}
	} catch (Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("SimHandler::tryToStartVideo(): Catch: " + e);
	}

}

void SimHandler::stopVideo()
{
	if (videoWriteThread != nullptr) {
			// stop, then wait

			breakNow = true;
			Thread::Sleep( 30 ); // ms

			videoWriteThread->Abort();
	}

	openGLView->disableVideoRecording();
}

void SimHandler::endRecording()
{
	if(recordVideo)
	{
		if (splitTimer != nullptr) {
			splitTimer->~Timer();
			splitTimer = nullptr;
		}

		stopVideo();	

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

void SimHandler::splitVideo(Object^ stateInfo)
{
	
	stopVideo();

	DateTime time = DateTime::Now;
	//time = time.AddSeconds( -Convert::ToInt32( SPLIT_LENGTH ) );
	String ^ filename = "D:\\Skynet Files\\video\\video_" + time.ToString("o")->Replace(":", "-");

	tryToStartVideo(filename);

	// alert user that video started
	((Comms ^)theComms)->printToConsole("Split Video", Color::Green);
	System::Diagnostics::Trace::WriteLine("Video split written to Telemetry");

}

void SimHandler::writeTelemetry( System::TimeSpan time,  int type, int length, array<System::Byte>^ byteArray )
{
	//System::Diagnostics::Trace::WriteLine("write telem");
	if(pleaseRecord)
	{
		//System::Diagnostics::Trace::WriteLine("actually writing telem");
		lock l(this);
		fileO->Write("" + (float) time.TotalSeconds);
		fileO->Write(", ");
		fileO->Write("" + (int)type);
		fileO->Write(", ");
		fileO->Write("" + (int)length); 
		fileO->Write(", ");
		int x;
		for(x=0; x<length;x++)
		{
			fileO->Write(byteArray[x]);
			firstPacket = false;
		}
		fileO->WriteLine();
		fileO->WriteLine();
		
		fileO->Flush();
	}
	//System::Diagnostics::Trace::WriteLine("done with telem");
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
	if (fileO != nullptr)
	{
			fileO->Close();
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
	if (fileO != nullptr)
	{
		fileO->Close();
	}
	FileStream^ fs = gcnew FileStream( filename+".telemetry.txt", FileMode::CreateNew);
	fileO = gcnew StreamWriter( fs, Encoding::UTF8 );
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