#include "StdAfx.h"
#include "Form1.h"
#include "Saliency.h"
#include "SaveImage.h"
#include "PlaneWatcher.h"


// Saliency update frequency (in miliseconds)
#define UPDATE_FREQUENCY 5

#define IMAGE_SAVE_BASE_PATH "D:\\Skynet Files\\Candidates\\bb"

using namespace Vision;

//[DllImport("Saliency_New", CharSet=CharSet::Ansi)]
//extern "C" void Auvsi_Saliency();	

// TODO: check for memory leaks

Saliency::Saliency( Object ^ watcher )
{
	saliencyThread = gcnew Thread(gcnew ThreadStart(this, &Saliency::saliencyThreadFunction));
	saliencyThread->Name = "Saliency Analysis Thread";
	saliencyThread->Start();

	saveImagesThread = gcnew Thread(gcnew ThreadStart(this, &Saliency::saveImagesThreadFunction));
	saveImagesThread->Name = "Image Saving Thread";
	saveImagesThread->Start();

	planeWatcher = watcher;

	newFrameReady = false;
	width = 0;
	height = 0;
	threshold = 0.7f;
	frameCount = 0;
	boundingBoxes = NULL;
	postSaliency = NULL;
	tempPause = false;
	
	/*System::DateTime start = System::DateTime::Now;
	
	Communications::PlaneState * state = ((Communications::PlaneWatcher ^)planeWatcher)->predictLocationAtTime( 0 );
	
	double plane_lat = state->gpsData->gpsLatitude, plane_lon = state->gpsData->gpsLongitude, plane_alt = state->gpsData->gpsAltitude;
	double plane_roll = state->telemData->roll, plane_pitch = state->telemData->pitch, plane_heading = state->telemData->heading;
	float gimbal_roll = state->gimbalInfo->roll, gimbal_pitch = state->gimbalInfo->pitch, gimbal_yaw = 0;
	
	
	System::Diagnostics::Trace::WriteLine("Saliency: Running Georeference: lat:" + plane_lat + " lon:" + plane_lon + " alt:" + plane_alt + " roll:" + plane_roll + " pitch:" + plane_pitch + " heading:" + plane_heading + " groll:" + gimbal_roll + " gpitch:" + gimbal_pitch + " gyaw:" + gimbal_yaw);
	
	float target_x = 0, target_y = 0, zoom = 1, t_lat = -1, t_lon = -1, t_alt = -1;
	
	float t_lat_1, t_lat_2, t_lon_1, t_lon_2;

	getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, target_x, target_y, zoom, t_lat_1, t_lon_1, t_alt);
	getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, 10, target_y, zoom, t_lat_2, t_lon_2, t_alt);

	System::Diagnostics::Trace::WriteLine("Distance: " + distanceBetweenGPS(t_lat_1, t_lon_1, t_lat_2, t_lon_2));
			
	System::Diagnostics::Trace::WriteLine("Saliency: Done with Georeference lat: " + t_lat + " lon: " + t_lon + " alt: " + t_alt + " time (ms): " + System::DateTime::Now.Subtract(start).TotalMilliseconds);

	delete state;*/
}

void 
Saliency::saliencyThreadFunction(void)
{
	
	try {
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
			//continue;	// DONT RUN SALIENCY 
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
				//System::Diagnostics::Trace::WriteLine( "New frame Ready");
			
				// set flags
				currentlyAnalyzing = true;
				newFrameReady = false;
				localData = currentFrameData; // data of current frame

				// run saliency
				theSaliency->loadImage( localW, localH, inputBuffer );
				theSaliency->computeSaliency(threshold);
				theSaliency->labelConnectedComponents();

			
				//System::Diagnostics::Trace::WriteLine( "Computing Saliency frame. frameW: " + width + " frameH: " + height);
			

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
			
			}

			//TODO: fix memory leaks, free the buffer
		}


	}

	catch ( Exception ^ theException) {
		System::Diagnostics::Trace::WriteLine( "Exception in saliencyThread: " + theException);
				
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
			
			savingData = true;
			newFrameForSaving = false;
			frameCount++;

			// DEBUG: save original image
			
			//ImageUtil::SaveImage theImSaver( height, width, 4 );
			//theImSaver.saveFrame(savingBuffer, ManagedToSTL(IMAGE_SAVE_BASE_PATH + frameCount + "_original.bmp"));
			
			//System::DateTime start = System::DateTime::Now;
	
	
	
			//System::Diagnostics::Trace::WriteLine("Saliency: Running Georeference: lat:" + plane_lat + " lon:" + plane_lon + " alt:" + plane_alt + " roll:" + plane_roll + " pitch:" + plane_pitch + " heading:" + plane_heading + " groll:" + gimbal_roll + " gpitch:" + gimbal_pitch + " gyaw:" + gimbal_yaw);
	
	
	
			//System::Diagnostics::Trace::WriteLine("Distance: " + distanceBetweenGPS(t_lat_1, t_lon_1, t_lat_2, t_lon_2));
			
			//System::Diagnostics::Trace::WriteLine("Saliency: Done with Georeference lat: " + t_lat + " lon: " + t_lon + " alt: " + t_alt + " time (ms): " + System::DateTime::Now.Subtract(start).TotalMilliseconds);

	
			Communications::PlaneState ^ state = ((Communications::PlaneWatcher ^)planeWatcher)->predictLocationAtTime( 0 );
	
			double plane_lat = state->gpsData->gpsLatitude, plane_lon = state->gpsData->gpsLongitude, plane_alt = state->gpsData->gpsAltitude;
			double plane_roll = state->telemData->roll, plane_pitch = state->telemData->pitch, plane_heading = state->telemData->heading;
			double gimbal_roll = state->gimbalInfo->roll, gimbal_pitch = state->gimbalInfo->pitch, gimbal_yaw = 0;
	
			double target_x = 0, target_y = 0, zoom = 1, t_lat = -1, t_lon = -1, t_alt = -1;
	
			double t_lat_1, t_lat_2, t_lon_1, t_lon_2;

			double metersPerXPixel, metersPerYPixel;

			target_x = 0; target_y = 100;
			getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, target_x, target_y, zoom, t_lat_1, t_lon_1, t_alt);
			
			target_x = 200; target_y = 100;
			getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, target_x, target_y, zoom, t_lat_2, t_lon_2, t_alt);
			metersPerXPixel = distanceBetweenGPS(t_lat_1, t_lon_1, t_lat_2, t_lon_2) / 200.0f;
			
			target_x = 100; target_y = 0;
			getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, target_x, target_y, zoom, t_lat_1, t_lon_1, t_alt);
			//System::Diagnostics::Trace::WriteLine("Saliency: lat1:" + t_lat_1 + " t_lon_1:" + t_lon_1 + " t_lat_2:" + t_lat_2 + " t_lon_2:" + t_lon_2);

			target_x = 100; target_y = 200;
			getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, target_x, target_y, zoom, t_lat_2, t_lon_2, t_alt);
			metersPerYPixel = distanceBetweenGPS(t_lat_1, t_lon_1, t_lat_2, t_lon_2) / 200.0f;
			//System::Diagnostics::Trace::WriteLine("Saliency: lat1:" + (double)t_lat_1 + " t_lon_1:" + (double)t_lon_1 + " t_lat_2:" + (double)t_lat_2 + " t_lon_2:" + (double)t_lon_2);

			//System::Diagnostics::Trace::WriteLine("Saliency: metersPerX:" + metersPerXPixel + " metersPerY:" + metersPerYPixel);
			
			// loop through each bounding box
			for (int i = 0; i < numBoxes; i++) {
				box currentBox = boundingBoxes[i]; // get bounding box

				// check for correct size ... if wrong size, skip this result
				if (!(validSize(((double)currentBox.width)*metersPerXPixel) && validSize(((double)currentBox.height)*metersPerYPixel))) {
					//System::Diagnostics::Trace::WriteLine("Saliency: size of target disregarded w:" + currentBox.width*metersPerXPixel + " h:" + currentBox.height*metersPerYPixel );
					continue;
				}
				else
					System::Diagnostics::Trace::WriteLine("Saliency: size of target included w:" + currentBox.width*metersPerXPixel + " h:" + currentBox.height*metersPerYPixel  );


				// save image to file
				
				ImageUtil::SaveImage theImSaver( currentBox.height, currentBox.width, 4 );


				float * tempBuffer = new float[currentBox.height * currentBox.width * 4];
				int counter = 0;

				for (int y =  currentBox.y; y < currentBox.y + currentBox.height && y < height; y++) {
					for (int x =  currentBox.x; x < currentBox.x + currentBox.width && x < width; x++) {

						for (int c = 0; c < 4; c++) {
							tempBuffer[counter] = savingBuffer[(y * width + x)*4 + c];
							counter++;

						}

					}
				}

				


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

				// what we really need for first database (candidate regions):
				//	plane info, gimbal info, camera info
				//	image size
				//	image data (the image itself, unrectified)

				// second database (unconfirmed targets):
				//	OCR info (letter, shape, letter color, letter shape)
				//	target location, heading, width and height
				//	image size
				//	rectified image data (the image itself)
				//	revision number (increment everytime we change the row)

				// third database (confirmed targets):
				//	all from second database except revision number

				
				theImSaver.saveFrame(tempBuffer, ManagedToSTL(IMAGE_SAVE_BASE_PATH + frameCount + "_" + i + ".bmp"));

				array<Object ^>^myArr = {data};
				try {
					
					((Skynet::Form1 ^ )parent)->Invoke( ((Skynet::Form1 ^ )parent)->saliencyAddTarget, myArr );
				}

				catch (Exception ^ theEx) {
					
					System::Diagnostics::Trace::WriteLine( "WARNING: Saliency exception: " + theEx);
				}

				


				delete tempBuffer;

			}

			delete state;

			savingData = false;
		}
	}


}

bool Saliency::validSize(double size)
{
	return (size < MAX_SIZE) && (size > MIN_SIZE);

}

// set new width, height, delegate
void 
Saliency::setValues(int w, int h, Object ^ newDelegate) 
{
	
	tempPause = true;
	Thread::Sleep( 40 );
	

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

	tempPause = false;
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
	//System::Diagnostics::Trace::WriteLine( "analyzeFrame()");
}

Saliency::~Saliency()
{
	// kill threads
	if( saliencyThread )
		saliencyThread->Abort();

	if( saveImagesThread )
		saveImagesThread->Abort();

}

#define PI 3.14159265

double cosd(double input)
{
	return cos(input*PI/180.0);
}

double sind(double input)
{
	return sin(input*PI/180.0);
}

double atand(double input)
{
	return atan(input)*180.0/PI;
}

double Saliency::distanceBetweenGPS(double lat1, double lon1, double lat2, double lon2)
{
	double radius = 6378000; // radius of earth!
	double deltaLat = lat2 - lat1;
	double deltaLon = lon2 - lon1;
	double a = sind(deltaLat/2)*sind(deltaLat*2) + cosd(lat1)*cosd(lat2)*sind(deltaLon/2)*sind(deltaLon*2);
	double c = 2*atan2(sqrt(a), sqrt((1-a)));
	double d = radius*c;

	return d;
}

cv::Mat Saliency::EulerAngles(bool transpose, cv::Mat Orig_Vector, double Roll, double Pitch, double Yaw)
{
	double R = Roll;
	double P = Pitch;
	double Y = Yaw;

	 
	double transarr[3][3] = {{cosd(P)*cosd(Y), cosd(P)*sind(Y), -sind(P)},
							{sind(R)*sind(P)*cosd(Y)-cosd(R)*sind(Y), sind(R)*sind(P)*sind(Y)+cosd(R)*cosd(Y), sind(R)*cosd(P)},  
	{cosd(R)*sind(P)*cosd(Y)+sind(R)*sind(Y), cosd(R)*sind(P)*sind(Y)-sind(R)*cosd(Y), cosd(R)*cosd(P)}};
	cv::Mat Transfer = cv::Mat(3, 3, CV_64FC1, transarr).inv();

	if (transpose)
		Transfer = Transfer.t();

	return Transfer*Orig_Vector;
}

String ^ matToString(cv::Mat in)
{
	String ^ ret = "{";
	typedef cv::Vec<double, 1> VT;

	for (int r = 0; r < in.rows; r++)
	{
		ret += "{";
		for (int c = 0; c < in.cols; c++)
		{
			ret += in.at<VT>(r, c)[0];
		}
		ret += "}, ";
	}

	ret += "}";
	return ret;

}

void Saliency::getGPS(double plane_latitude, double plane_longitude, double plane_altitude, double plane_roll, double plane_pitch, double plane_heading, double gimbal_roll, double gimbal_pitch, double gimbal_yaw, 
				double target_x, double target_y, double zoom, double & Target_Latitude, double & Target_Longitude, double & Target_Height)
{
	double x_fov = 46.0f;
	double y_fov = 34.0f;
	double x_pixels = 720;
	double y_pixels = 486;
	double zoom_factor = zoom;
	double target_pixel_x = target_x;
	double target_pixel_y = target_y;


	typedef cv::Vec<double, 1> VT;
	double pix[3][1] = {{0}, {0}, {1}};
	cv::Mat Pixel_CF_Vector(3, 1, CV_64FC1, pix );

	double ground_altitude = 0;

	double a = 6378137;
	double b = 6356752.3142;

	// PART A
	double fovarr[3][1] = {{x_fov}, {y_fov}, {1}};
	cv::Mat FOV(3, 1, CV_64FC1, fovarr );
	

	cv::Mat Scale(3, 3, CV_64FC1 );
	Scale.at<VT>(0, 0)[0] = 1/zoom_factor;
	Scale.at<VT>(0, 1)[0] = 0;
	Scale.at<VT>(0, 2)[0] = 0;
	Scale.at<VT>(1, 0)[0] = 1;
	Scale.at<VT>(1, 1)[0] = 1/zoom_factor;
	Scale.at<VT>(1, 2)[0] = 0;
	Scale.at<VT>(2, 0)[0] = 0;
	Scale.at<VT>(2, 1)[0] = 0;
	Scale.at<VT>(2, 2)[0] = 1;
	
	cv::Mat FOV_zoom_accounted = Scale*FOV;

	// PART B
	double Pixel_Roll = (FOV_zoom_accounted.at<VT>(1, 0)[0])/2 * target_pixel_x / (x_pixels);
	double Pixel_Pitch = (FOV_zoom_accounted.at<VT>(2, 0)[0])/2 * target_pixel_y / (y_pixels);
	double Pixel_Yaw = 0;

	//TODO: define EulerAngles function
	cv::Mat CC_CF_Vector = EulerAngles(1, Pixel_CF_Vector, Pixel_Roll, Pixel_Pitch, Pixel_Yaw);

	// PART C
	cv::Mat GZ_CF_Vector = EulerAngles(1, CC_CF_Vector, gimbal_roll, gimbal_pitch, gimbal_yaw);

	// PART D
	cv::Mat Plane_CF_Vector = EulerAngles(1, GZ_CF_Vector, plane_roll, plane_pitch, plane_heading);

	// PART E
	double f = a/(a-b);
	//double temp = ;
	double e = sqrt( (1.0 / f ) * (2 - 1*(1 / f ))  );
	double N = a/(sqrt((double)(1.0f - (e*e)*sind(plane_latitude)*sind(plane_latitude))));

	double X = (N+plane_altitude)*cosd(plane_latitude)*cosd(plane_longitude);
	double Y = (N+plane_altitude)*cosd(plane_latitude)*sind(plane_longitude);
	double Z = (N*(1-e*e)+plane_altitude)*sind(plane_latitude);

	double m[3][1] = {{X}, {Y}, {Z}};
	cv::Mat InitialXYZ = cv::Mat(3, 1, CV_64FC1, m);//.inv();
	
	/*System::Diagnostics::Trace::WriteLine("Part E initialxyz: " + matToString(InitialXYZ) + " f:" + f + " e:" + e + " N:" + N + " X:" + X + " Y:" + Y + " Z:" + Z);
	System::Diagnostics::Trace::WriteLine("Part E a:" + a + " b:" + b + " plane_latitude:" + plane_latitude + " plane_longitude:" + plane_longitude);
	System::Diagnostics::Trace::WriteLine("Part E a:" + sind(plane_latitude));
	System::Diagnostics::Trace::WriteLine("Part E a:" + (e*e)*sind(plane_latitude)*sind(plane_latitude));
	System::Diagnostics::Trace::WriteLine("Part E a:" + (double)(1.0f - (e*e)*sind(plane_latitude)*sind(plane_latitude)));
	System::Diagnostics::Trace::WriteLine("Part E a:" + (sqrt((double)(1.0f - (e*e)*sind(plane_latitude)*sind(plane_latitude)))));*/

	// PART F
	double n = 1;
	Target_Height = 0;
	
	const double HEIGHT_OF_FIELD = 3.3333333333;
	const double MAX_DISTANCE = 500; // all of this in meters
	double MIN_DISTANCE = plane_altitude - HEIGHT_OF_FIELD;
	const double MARGIN_OF_ERROR = 0.5;
	double range = MAX_DISTANCE - MIN_DISTANCE;
	n = range/2 + MIN_DISTANCE;
	range /= 2;

	double h, p, mlong, latchange, newlat, lat;
	int loopcounter = 0;
	while (loopcounter < 100 && fabs(Target_Height - HEIGHT_OF_FIELD) > MARGIN_OF_ERROR)
	{
		cv::Mat NED = Plane_CF_Vector * n;


		double transarr[3][3] = {{-sind(plane_latitude)*cosd(plane_longitude), -sind(plane_longitude), -cosd(plane_latitude)*cosd(plane_longitude)}, 
								{-sind(plane_latitude)*sind(plane_longitude), -cosd(plane_longitude), -cosd(plane_latitude)*sind(plane_longitude)}, 
								{cosd(plane_latitude), 0, -sind(plane_latitude)}};
		cv::Mat Trans = cv::Mat(3, 3, CV_64FC1, transarr).inv();
		cv::Mat XYZ = InitialXYZ + Trans*NED;

		X = XYZ.at<VT>(0, 0)[0];
		Y = XYZ.at<VT>(1, 0)[0];
		Z = XYZ.at<VT>(2, 0)[0];
		h = 0;
		N = a;
		p = sqrt(X*X + Y*Y);
		mlong = atand(Y/X);
		latchange = 10;
		newlat = 10;
		int count = 0;
		double sinfind;
		while (latchange > 0.0001)
		{
			sinfind = Z/(N*(1-e*e) + h);
			lat = atand((Z+e*e*N*sinfind)/p);
			N = a/(sqrt(1-e*e*sind(lat)*sind(lat)));
			h = p/cosd(lat)-N;
			latchange = abs(newlat - lat);
			count ++;
			newlat = lat;
		}

		mlong = (180-mlong)*-1;
		Target_Latitude = lat;
		Target_Longitude = mlong;
		Target_Height = h;


		if (norm(NED) > MAX_DISTANCE) {
			System::Diagnostics::Trace::WriteLine("FAILURE NO TARGET IN RANGE");
			return;
		}

		if (h < HEIGHT_OF_FIELD)
			n = n - range/2;
		else 
			n = n + range/2;

		range /= 2;
		loopcounter++;
		
	}

	// return values!!!
	Target_Latitude = lat;
	Target_Longitude = mlong;
	Target_Height = h;
}
