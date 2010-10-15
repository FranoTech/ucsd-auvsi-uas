#include "StdAfx.h"
#include "Form1.h"
#include "Saliency.h"
#include "SaveImage.h"


// Saliency update frequency (in miliseconds)
#define UPDATE_FREQUENCY 5

#define IMAGE_SAVE_BASE_PATH "D:\\Skynet Files\\Candidates\\bb"

using namespace Vision;

//[DllImport("Saliency_New", CharSet=CharSet::Ansi)]
//extern "C" void Auvsi_Saliency();	

// TODO: check for memory leaks

Saliency::Saliency()
{
	saliencyThread = gcnew Thread(gcnew ThreadStart(this, &Saliency::saliencyThreadFunction));
	saliencyThread->Name = "Saliency Analysis Thread";
	saliencyThread->Start();

	saveImagesThread = gcnew Thread(gcnew ThreadStart(this, &Saliency::saveImagesThreadFunction));
	saveImagesThread->Name = "Image Saving Thread";
	saveImagesThread->Start();

	newFrameReady = false;
	width = 0;
	height = 0;
	threshold = 0.7f;
	frameCount = 0;
	boundingBoxes = NULL;
	postSaliency = NULL;
}

void 
Saliency::saliencyThreadFunction(void)
{
	FILE * fd;
	fopen_s(&fd, "C:\\Users\\UCSD\\Pictures\\Saliency Sample\\errorLog.txt", "w");
	fprintf(fd, "Error Log for saliency:\n");
	fclose( fd );

	Auvsi_Saliency * theSaliency = new Auvsi_Saliency();
	/*const char * imagePath = "C:\\Users\\UCSD\\Pictures\\Saliency Sample\\heli.bmp";
	theSaliency->loadImage( imagePath);
	theSaliency->prepareFilters();
	theSaliency->prepareForSaliency();
	theSaliency->computeSaliency(0.6);*/

	int localW = 0, localH = 0;
	FrameData ^ localData;

	while( true )
	{
		Thread::Sleep( UPDATE_FREQUENCY ); // dont run too fast, or we eat up cpu ... ;)
		continue;	// DONT RUN SALIENCY 
		// check for new width, height
		if (localW != width || localH != height) 
		{
			localW = width;
			localH = height;

			// set new data in saliency, prepare shit
			if (localW != 0 && localH != 0) 
			{
				//theSaliency->loadImage( "C:\\Users\\UCSD\\Pictures\\Saliency Sample\\heli.bmp" );
				theSaliency->setInformation( localW, localH );
				theSaliency->prepareFilters();
				theSaliency->prepareForSaliency();
				//theSaliency->computeSaliency(0.6);
			}
		}
		
		// check for new frame
		if (newFrameReady) 
		{
			// set flags
			currentlyAnalyzing = true;
			newFrameReady = false;
			localData = currentFrameData; // data of current frame

			// run saliency
			theSaliency->loadImage( localW, localH, inputBuffer );
			theSaliency->computeSaliency(threshold);
			theSaliency->labelConnectedComponents();


			while (savingData) { Thread::Sleep( UPDATE_FREQUENCY ); }

			if( boundingBoxes != NULL )
			{
				delete boundingBoxes;
				boundingBoxes = NULL;
			}

			if( postSaliency != NULL )
			{
				delete postSaliency;
				postSaliency = NULL;
			}

			float * imageReturn = new float[ localW * localH ];
			box * boxReturn;
			int numBoxReturn;

			theSaliency->getFinalData( imageReturn, boxReturn, numBoxReturn); // save data for image saving thread to grab
			
			numBoxes = numBoxReturn;
			boundingBoxes = boxReturn;
			postSaliency = imageReturn;

			memcpy(savingBuffer, inputBuffer, width * height * 4 * sizeof(float)); // copy full color buffer for saving images
			savingFrameData = localData;
			currentlyAnalyzing = false;
			newFrameForSaving = true;
			
			//TODO: fix memory leaks, free the buffer
		}


	}
}

std::string ManagedToSTL(String ^ s) 
		{
		   using namespace Runtime::InteropServices;
		   const char* chars = 
		      (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
		   std::string retVal = chars;
		   Marshal::FreeHGlobal(IntPtr((void*)chars));

		   return retVal;
		}

void 
Saliency::saveImagesThreadFunction()
{
	
	while( true )
	{
		Thread::Sleep( UPDATE_FREQUENCY ); // dont run too fast, or we eat up cpu ... ;)
		if ( newFrameForSaving ) {
			continue;
			savingData = true;
			newFrameForSaving = false;
			frameCount++;
			// loop through each bounding box
			for (int i = 0; i < numBoxes; i++) {
				box currentBox = boundingBoxes[i]; // get bounding box

				ImageUtil::SaveImage theImSaver( currentBox.height, currentBox.width, 4 );

				// save image to file
				float * tempBuffer = new float[currentBox.height * currentBox.width * 4];
				int counter = 0;
				for (int k = currentBox.x; k < currentBox.x + currentBox.width; k++)
					for (int j = currentBox.y; j < currentBox.y + currentBox.height; j++)
						tempBuffer[counter] = savingBuffer[k * width + j];

				theImSaver.saveFrame(tempBuffer, ManagedToSTL(IMAGE_SAVE_BASE_PATH + frameCount + "_" + i + ".bmp"));

				// make RowData, pass to callback in form1.h
				Database::RowData ^ data = gcnew Database::RowData();

				data->path = (IMAGE_SAVE_BASE_PATH + frameCount + "_" + i + ".bmp")->Replace("\\", "\\\\");
				data->target_latitude = 0;			// Latitude of point selected as target
				data->target_longitude = 0;			// Longitude of point selected as target
				data->target_X = currentBox.x + currentBox.width / 2;					// pixel X of target
				data->target_Y = currentBox.y + currentBox.height / 2;					// pixel Y of target
				data->heading = savingFrameData->heading;	// heading of target
				data->letter = "A";				
				data->shape = "Circle";					
				data->fg_color = "White";				// foreground color
				data->bg_color = "Yellow";				// background color
				data->processed = FALSE;				// whether this has been processed by OpenCV
				//data->verified = FALSE;				// human verified
				data->center_latitude = savingFrameData->latitude;			// Latitude of center pixel
				data->center_longitude = savingFrameData->longitude;			// Longitude of center pixel
				data->mapping_latitude = 0;			// pixel to meter translation for latitude
				data->mapping_longitude = 0;		// pixel to meter translation for longitude
				data->homography = savingFrameData->homography;

				((Skynet::Form1 ^)parent)->Invoke( ((Skynet::Form1 ^)parent)->saliencyAddTarget, data );
				delete tempBuffer;
			}


			savingData = false;
		}
	}


}

// set new width, height, delegate
void 
Saliency::setValues(int w, int h, Object ^ newDelegate) 
{
	width = w;
	height = h;

	// (re) allocate all buffers
	if (postSaliency)
	{
		delete postSaliency;
		postSaliency = NULL;
	}

	//postSaliency = new float[w*h];

	if (savingBuffer)
	{
		delete savingBuffer;
		savingBuffer = NULL;
	}
	
	savingBuffer = new float[w*h*4];

	if (inputBuffer)
	{
		delete inputBuffer;
		inputBuffer = NULL;
	}

	inputBuffer = new float[w*h*4];

	if (newDelegate != nullptr)
		parent = newDelegate;
}

// copies new frame into buffer, sets flag
void 
Saliency::analyzeFrame(float * buffer, FrameData ^ theData)	
{
	if (canAcceptFrame()) {
		memcpy(inputBuffer, buffer, width*height*4*sizeof(float));
		currentFrameData = theData;
		newFrameReady = true;
	}
}

Saliency::~Saliency()
{
	// kill threads
	if( saliencyThread )
		saliencyThread->Abort();

	if( saveImagesThread )
		saveImagesThread->Abort();

}