#pragma once
#include <string>
#include "Auvsi_Saliency.h"
//#include "Auvsi_FFT.h"
#include "Database.h"

using namespace System::Threading;
using namespace System;
using namespace System::Windows::Forms;


namespace Vision
{
	ref struct FrameData
	{
		double latitude;
		double longitude;
		double altitude;
		double heading;
		double azimuth;
		double elevation;
		double zoomLevel;
		array<float> ^ homography;
	};

	
	delegate void saliencyUpdateDelegate( Database::RowData ^ data );

	ref class Saliency
	{
	public:
		Saliency(); 	// constructor
		~Saliency();	// destructor

		void setValues(int w, int h, Object ^ newDelegate);		// set initial values
		bool canAcceptFrame() { return !currentlyAnalyzing;}	// call to see if ready to accept new frame
		void analyzeFrame(float * buffer, FrameData ^ theData);						// pass in new frame to analyze
		float * postSaliency;		// results of saliency (black and white)
	protected:
		bool currentlyAnalyzing;	// saliency is running
		bool newFrameReady;			// saliency is ready for new frame
		Thread ^ saliencyThread;	// thread for running saliency
		Thread ^ saveImagesThread;	// thread for saving images
		float * inputBuffer;		// frame being analyzed by saliency
		int width;					// frame width
		int height;					// frame height
		float threshold;			// threshold for saliency
		Object ^ parent;			// delegate to send info to
		FrameData ^ currentFrameData;	// current saliency frame
		
		
		saliencyUpdateDelegate ^ saliencyDelegate;
		int frameCount;

		// for saving images thread
		FrameData ^ savingFrameData;
		box * boundingBoxes;
		int numBoxes;
		float * savingBuffer;		// color image for saving to file
		bool newFrameForSaving;		// new frame ready for saveThread to save
		bool savingData;			// images are being saved

		bool tempPause;
		
		
		void saliencyThreadFunction(); // main run loop for saliency
		void saveImagesThreadFunction();	// main run loop for saving images
		bool isValidTarget(Database::RowData ^ data, box theBox); // determine whether a target is valid

	};
}
