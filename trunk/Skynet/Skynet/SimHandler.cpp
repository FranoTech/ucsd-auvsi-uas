


#include "StdAfx.h"
#include "SimHandler.h"


#include "Comport.h"
#include <time.h>
#include <iomanip>    // Needed for stream modifiers fixed and setprecision
#include <string>

using namespace std;
using namespace Simulator;
using namespace System;
using namespace System::Text;
using namespace System::Windows::Forms;

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
void SimHandler::beginRecording(String ^ filename)
{
	if(recordVideo)
	{
		openGLView->enableVideoRecording(filename);
		System::Diagnostics::Trace::WriteLine("beginRecording in SimHandler");
	}
	if(recordTelemetry)
	{
		firstPacket = true;
		beginTelemetry(filename);
	}

	//if recordVideo call OpenGL
	//if recordTelem call beginTelemetry();
}
void SimHandler::endRecording()
	{
		if(recordVideo)
		{
			openGLView->disableVideoRecording();
		}
		//if recordvideo call end in OpenGL

		if(recordTelemetry)
		{
			endTelemetry();
			pleaseDontRecord=false;
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

void SimHandler::writeTelemetry(Communications::ComportDownstream * packet)
{
	if(pleaseDontRecord)
	{
		if(firstPacket)
		{
			/*
			fileO << fixed << setprecision(2) << packet.time_offset << endl;
			*/
			*fileO << packet->time_offset << endl << endl;
			firstPacket = false;
		}
		/*
		fileO << fixed << setprecision(2) << packet.time_offset << endl;
		fileO << fixed << setprecision(2) << packet.gps_lat << endl;
		fileO << fixed << setprecision(2) << packet.gps_lon << endl;
		fileO << fixed << setprecision(2) << packet.gps_alt << endl;
		fileO << fixed << setprecision(2) << packet.airplane_roll << endl;
		fileO << fixed << setprecision(2) << packet.airplane_pitch << endl;
		fileO << fixed << setprecision(2) << packet.airplane_heading << endl;
		fileO << fixed << setprecision(2) << packet.gimbal.azimuth << endl;
		fileO << fixed << setprecision(2) << packet.gimbal_elevation << endl;
		fileO << fixed << setprecision(2) << packet.gimbal_heading << endl;
		fileO << fixed << setprecision(2) << packet.camera_zoom << endl;
		*/
		*fileO << packet->time_offset << endl;
		*fileO << packet->gps_lat << endl;
		*fileO << packet->gps_lon << endl;
		*fileO << packet->gps_alt << endl;
		*fileO << packet->airplane_roll << endl;
		*fileO << packet->airplane_pitch << endl;
		*fileO << packet->airplane_heading << endl;
		*fileO << packet->gimbal_azimuth << endl;
		*fileO << packet->gimbal_elevation << endl;
		*fileO << packet->gimbal_heading << endl;
		*fileO << packet->camera_zoom << endl;
		*fileO << packet->error_code << endl << endl;
		fileO->flush();
	}
	/*
	* IF recordTelemetry
	write to file;
	else
	return;
	*/
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
	pleaseDontRecord=false;
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
	pleaseDontRecord=true;
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