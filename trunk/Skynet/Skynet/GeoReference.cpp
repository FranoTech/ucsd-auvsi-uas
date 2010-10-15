#include "StdAfx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cv.h>
#include "GeoReference.h"

using namespace System;

/*Reference ellipsoids derived from Peter H. Dana's website- 
http://www.utexas.edu/depts/grg/gcraft/notes/datum/elist.html
Department of Geography, University of Texas at Austin
Internet: pdana@mail.utexas.edu
3/22/95

Source
Defense Mapping Agency. 1987b. DMA Technical Report: Supplement to Department of Defense World Geodetic System
1984 Technical Report. Part I and II. Washington, DC: Defense Mapping Agency
*/

void 
GeoReference::LLtoUTM(int ReferenceEllipsoid, const double Lat, const double Long, 
			 double &UTMNorthing, double &UTMEasting, char* UTMZone)
{
//converts lat/long to UTM coords.  Equations from USGS Bulletin 1532 
//East Longitudes are positive, West longitudes are negative. 
//North latitudes are positive, South latitudes are negative
//Lat and Long are in decimal degrees
	//Written by Chuck Gantz- chuck.gantz@globalstar.com

	double a = ellipsoid[ReferenceEllipsoid].EquatorialRadius;
	double eccSquared = ellipsoid[ReferenceEllipsoid].eccentricitySquared;
	double k0 = 0.9996;

	double LongOrigin;
	double eccPrimeSquared;
	double N, T, C, A, M;
	
//Make sure the longitude is between -180.00 .. 179.9
	double LongTemp = (Long+180)-int((Long+180)/360)*360-180; // -180.00 .. 179.9;

	double LatRad = Lat*deg2rad;
	double LongRad = LongTemp*deg2rad;
	double LongOriginRad;
	int    ZoneNumber;

	ZoneNumber = int((LongTemp + 180)/6) + 1;
  
	if( Lat >= 56.0 && Lat < 64.0 && LongTemp >= 3.0 && LongTemp < 12.0 )
		ZoneNumber = 32;

  // Special zones for Svalbard
	if( Lat >= 72.0 && Lat < 84.0 ) 
	{
	  if(      LongTemp >= 0.0  && LongTemp <  9.0 ) ZoneNumber = 31;
	  else if( LongTemp >= 9.0  && LongTemp < 21.0 ) ZoneNumber = 33;
	  else if( LongTemp >= 21.0 && LongTemp < 33.0 ) ZoneNumber = 35;
	  else if( LongTemp >= 33.0 && LongTemp < 42.0 ) ZoneNumber = 37;
	 }
	LongOrigin = (ZoneNumber - 1)*6 - 180 + 3;  //+3 puts origin in middle of zone
	LongOriginRad = LongOrigin * deg2rad;

	//compute the UTM Zone from the latitude and longitude
	sprintf(UTMZone, "%d%c", ZoneNumber, UTMLetterDesignator(Lat));

	eccPrimeSquared = (eccSquared)/(1-eccSquared);

	N = a/sqrt(1-eccSquared*sin(LatRad)*sin(LatRad));
	T = tan(LatRad)*tan(LatRad);
	C = eccPrimeSquared*cos(LatRad)*cos(LatRad);
	A = cos(LatRad)*(LongRad-LongOriginRad);

	M = a*((1	- eccSquared/4		- 3*eccSquared*eccSquared/64	- 5*eccSquared*eccSquared*eccSquared/256)*LatRad 
				- (3*eccSquared/8	+ 3*eccSquared*eccSquared/32	+ 45*eccSquared*eccSquared*eccSquared/1024)*sin(2*LatRad)
									+ (15*eccSquared*eccSquared/256 + 45*eccSquared*eccSquared*eccSquared/1024)*sin(4*LatRad) 
									- (35*eccSquared*eccSquared*eccSquared/3072)*sin(6*LatRad));
	
	UTMEasting = (double)(k0*N*(A+(1-T+C)*A*A*A/6
					+ (5-18*T+T*T+72*C-58*eccPrimeSquared)*A*A*A*A*A/120)
					+ 500000.0);

	UTMNorthing = (double)(k0*(M+N*tan(LatRad)*(A*A/2+(5-T+9*C+4*C*C)*A*A*A*A/24
				 + (61-58*T+T*T+600*C-330*eccPrimeSquared)*A*A*A*A*A*A/720)));
	if(Lat < 0)
		UTMNorthing += 10000000.0; //10000000 meter offset for southern hemisphere
}

char 
GeoReference::UTMLetterDesignator(double Lat)
{
//This routine determines the correct UTM letter designator for the given latitude
//returns 'Z' if latitude is outside the UTM limits of 84N to 80S
	//Written by Chuck Gantz- chuck.gantz@globalstar.com
	char LetterDesignator;

	if((84 >= Lat) && (Lat >= 72)) LetterDesignator = 'X';
	else if((72 > Lat) && (Lat >= 64)) LetterDesignator = 'W';
	else if((64 > Lat) && (Lat >= 56)) LetterDesignator = 'V';
	else if((56 > Lat) && (Lat >= 48)) LetterDesignator = 'U';
	else if((48 > Lat) && (Lat >= 40)) LetterDesignator = 'T';
	else if((40 > Lat) && (Lat >= 32)) LetterDesignator = 'S';
	else if((32 > Lat) && (Lat >= 24)) LetterDesignator = 'R';
	else if((24 > Lat) && (Lat >= 16)) LetterDesignator = 'Q';
	else if((16 > Lat) && (Lat >= 8)) LetterDesignator = 'P';
	else if(( 8 > Lat) && (Lat >= 0)) LetterDesignator = 'N';
	else if(( 0 > Lat) && (Lat >= -8)) LetterDesignator = 'M';
	else if((-8> Lat) && (Lat >= -16)) LetterDesignator = 'L';
	else if((-16 > Lat) && (Lat >= -24)) LetterDesignator = 'K';
	else if((-24 > Lat) && (Lat >= -32)) LetterDesignator = 'J';
	else if((-32 > Lat) && (Lat >= -40)) LetterDesignator = 'H';
	else if((-40 > Lat) && (Lat >= -48)) LetterDesignator = 'G';
	else if((-48 > Lat) && (Lat >= -56)) LetterDesignator = 'F';
	else if((-56 > Lat) && (Lat >= -64)) LetterDesignator = 'E';
	else if((-64 > Lat) && (Lat >= -72)) LetterDesignator = 'D';
	else if((-72 > Lat) && (Lat >= -80)) LetterDesignator = 'C';
	else LetterDesignator = 'Z'; //This is here as an error flag to show that the Latitude is outside the UTM limits

	return LetterDesignator;
}


void 
GeoReference::UTMtoLL(int ReferenceEllipsoid, const double UTMNorthing, const double UTMEasting, const char* UTMZone,
			  double& Lat,  double& Long )
{
//converts UTM coords to lat/long.  Equations from USGS Bulletin 1532 
//East Longitudes are positive, West longitudes are negative. 
//North latitudes are positive, South latitudes are negative
//Lat and Long are in decimal degrees. 
	//Written by Chuck Gantz- chuck.gantz@globalstar.com

	double k0 = 0.9996;
	double a = ellipsoid[ReferenceEllipsoid].EquatorialRadius;
	double eccSquared = ellipsoid[ReferenceEllipsoid].eccentricitySquared;
	double eccPrimeSquared;
	double e1 = (1-sqrt(1-eccSquared))/(1+sqrt(1-eccSquared));
	double N1, T1, C1, R1, D, M;
	double LongOrigin;
	double mu, phi1, phi1Rad;
	double x, y;
	int ZoneNumber;
	char* ZoneLetter;
	int NorthernHemisphere; //1 for northern hemispher, 0 for southern

	x = UTMEasting - 500000.0; //remove 500,000 meter offset for longitude
	y = UTMNorthing;

	ZoneNumber = strtoul(UTMZone, &ZoneLetter, 10);
	if((*ZoneLetter - 'N') >= 0)
		NorthernHemisphere = 1;//point is in northern hemisphere
	else
	{
		NorthernHemisphere = 0;//point is in southern hemisphere
		y -= 10000000.0;//remove 10,000,000 meter offset used for southern hemisphere
	}

	LongOrigin = (ZoneNumber - 1)*6 - 180 + 3;  //+3 puts origin in middle of zone

	eccPrimeSquared = (eccSquared)/(1-eccSquared);

	M = y / k0;
	mu = M/(a*(1-eccSquared/4-3*eccSquared*eccSquared/64-5*eccSquared*eccSquared*eccSquared/256));

	phi1Rad = mu	+ (3*e1/2-27*e1*e1*e1/32)*sin(2*mu) 
				+ (21*e1*e1/16-55*e1*e1*e1*e1/32)*sin(4*mu)
				+(151*e1*e1*e1/96)*sin(6*mu);
	phi1 = phi1Rad*rad2deg;

	N1 = a/sqrt(1-eccSquared*sin(phi1Rad)*sin(phi1Rad));
	T1 = tan(phi1Rad)*tan(phi1Rad);
	C1 = eccPrimeSquared*cos(phi1Rad)*cos(phi1Rad);
	R1 = a*(1-eccSquared)/pow(1-eccSquared*sin(phi1Rad)*sin(phi1Rad), 1.5);
	D = x/(N1*k0);

	Lat = phi1Rad - (N1*tan(phi1Rad)/R1)*(D*D/2-(5+3*T1+10*C1-4*C1*C1-9*eccPrimeSquared)*D*D*D*D/24
					+(61+90*T1+298*C1+45*T1*T1-252*eccPrimeSquared-3*C1*C1)*D*D*D*D*D*D/720);
	Lat = Lat * rad2deg;

	Long = (D-(1+2*T1+C1)*D*D*D/6+(5-2*C1+28*T1-3*C1*C1+8*eccPrimeSquared+24*T1*T1)
					*D*D*D*D*D/120)/cos(phi1Rad);
	Long = LongOrigin + Long * rad2deg;

}

// phi = roll
// theta = pitch
// psi = yaw

//void
//GeoReference::EulerMatrix( double phi, double theta, double psi, float * matrix )
//{
//	matrix[0] = cos( psi ) * cos( theta );
//	matrix[1] = cos( psi ) * sin( theta ) * sin( phi ) - sin( psi ) * cos( phi );
//	matrix[2] = cos( psi ) * sin( theta ) * cos( phi ) + sin( psi ) * sin( phi );
//	matrix[3] = sin( psi ) * cos( theta );
//	matrix[4] = sin( psi ) * sin( theta ) * sin( phi ) + cos( psi ) * cos( phi );
//	matrix[5] = sin( psi ) * sin( theta ) * cos( phi ) - cos( psi ) * sin( phi );
//	matrix[6] = -sin( theta );
//	matrix[7] = cos( theta ) * sin( phi );
//	matrix[8] = cos( theta ) * cos( phi );
//}

// yaw pitch roll
cv::Mat
GeoReference::eulerMatrix( double psi, double theta, double phi )
{
	typedef cv::Vec<float, 1> VT;
	cv::Mat retVal(3, 3, CV_32FC1 );
	// row, col
	retVal.at<VT>( 0, 0 ) = VT( cos( psi ) * cos( theta ) );
	retVal.at<VT>( 0, 1 ) = VT( cos( psi ) * sin( theta ) * sin( phi ) - sin( psi ) * cos( phi ) );
	retVal.at<VT>( 0, 2 ) = VT( cos( psi ) * sin( theta ) * cos( phi ) + sin( psi ) * sin( phi ) );

	retVal.at<VT>( 1, 0 ) = VT( sin( psi ) * cos( theta ) );
	retVal.at<VT>( 1, 1 ) = VT( sin( psi ) * sin( theta ) * sin( phi ) + cos( psi ) * cos( phi ) );
	retVal.at<VT>( 1, 2 ) = VT( sin( psi ) * sin( theta ) * cos( phi ) - cos( psi ) * sin( phi ) );

	retVal.at<VT>( 2, 0 ) = VT( -sin( theta ) );
	retVal.at<VT>( 2, 1 ) = VT( cos( theta ) * sin( phi ) );
	retVal.at<VT>( 2, 2 ) = VT( cos( theta ) * cos( phi ) );

	return retVal;
}

double divideZero( double a, double b )
{
	if( b == 0 )
		return 0;
	else
		return a / b;
}

cv::Mat
GeoReference::imagePoints( void )
{
	typedef cv::Vec<float, 2> VT;
	cv::Mat retVal( 5, 1, CV_32FC2 );
	
	// Upper left corner 
	// pixel coords = 0, 0

	// Upper right corner
	// pixel coords = 719, 0

	// Lower right corner
	// pixel coords = 719, 485

	// Lower left corner
	// pixel coords = 0, 485

	// x, y
	retVal.at<VT>( 0, 0 ) = VT( 0, 0 );
	retVal.at<VT>( 1, 0 ) = VT( 719, 0 );
	retVal.at<VT>( 2, 0 ) = VT( 719, 485 );
	retVal.at<VT>( 3, 0 ) = VT( 0, 485 );
	retVal.at<VT>( 4, 0 ) = VT( 359, 242 );

	return retVal;
}

cv::Mat
defaultVector( void )
{
	typedef cv::Vec<float, 1> VT;
	cv::Mat retVal(3, 1, CV_32FC1 );

	retVal.at<VT>(0, 0)[0] = 1.0;
	retVal.at<VT>(1, 0)[0] = 0.0;
	retVal.at<VT>(2, 0)[0] = 0.0;

	return retVal;
}

void printMatrix( cv::Mat matrix )
{
	typedef cv::Vec<float, 1> VT;
	System::Diagnostics::Trace::WriteLine(matrix.at<VT>(0, 0)[0] + " " + matrix.at<VT>(0, 1)[0] +  " " + matrix.at<VT>(0, 2)[0]);
	System::Diagnostics::Trace::WriteLine(matrix.at<VT>(1, 0)[0] + " " + matrix.at<VT>(1, 1)[0] +  " " + matrix.at<VT>(1, 2)[0]);
	System::Diagnostics::Trace::WriteLine(matrix.at<VT>(2, 0)[0] + " " + matrix.at<VT>(2, 1)[0] +  " " + matrix.at<VT>(2, 2)[0]);	
}

void
GeoReference::applyHomography( array<float> ^ homography, int pixelX, int pixelY, double airplane_lat, double airplane_lon, double & lat, double & lon  )
{
	typedef cv::Vec<float, 1> VT;
	cv::Mat imagePoint(3, 1, CV_32FC1 );
	cv::Mat earthPoint;
	imagePoint.at<VT>(0, 0) = VT( pixelX );
	imagePoint.at<VT>(1, 0) = VT( pixelY );
	imagePoint.at<VT>(2, 0) = VT( 1.0 );

	cv::Mat homo(3, 3, CV_32FC1 );
	homo.at<VT>( 0, 0 )[0] = homography[0];
	homo.at<VT>( 0, 1 )[0] = homography[1];
	homo.at<VT>( 0, 2 )[0] = homography[2];
	homo.at<VT>( 1, 0 )[0] = homography[3];
	homo.at<VT>( 1, 1 )[0] = homography[4];
	homo.at<VT>( 1, 2 )[0] = homography[5];
	homo.at<VT>( 2, 0 )[0] = homography[6];
	homo.at<VT>( 2, 1 )[0] = homography[7];
	homo.at<VT>( 2, 2 )[0] = homography[8];

	earthPoint = homo * imagePoint;
	earthPoint.at<VT>( 0, 0 )[0] = divideZero( earthPoint.at<VT>( 0, 0 )[0], earthPoint.at<VT>( 2, 0 )[0] );
	earthPoint.at<VT>( 1, 0 )[0] = divideZero( earthPoint.at<VT>( 1, 0 )[0], earthPoint.at<VT>( 2, 0 )[0] );

	double northing, easting;
	char UTMZone[4];
	
	LLtoUTM(23, airplane_lat, airplane_lon, northing, easting, UTMZone);
		
	northing = northing + earthPoint.at<VT>(1, 0)[0];
	easting = easting + earthPoint.at<VT>(1, 0)[0];

	UTMtoLL( 23, northing, easting, UTMZone,
			  lat, lon );			   	
}

cv::Mat homography( cv::Mat x, cv::Mat y )
{
	typedef cv::Vec<float, 2> VT_POINT;
	typedef cv::Vec<float, 1> VT;
	cv::Mat retVal;

	int length = x.rows;
	
	cv::Mat A( 3*length, 9, CV_32FC1 );
	A = 0;
	
	float X1, X2, X3;
	float xx, yy, zz;
	for( int i = 1; i < length + 1; ++i )
	{
		X1 = x.at<VT_POINT>(i-1, 0)[0];
		X2 = x.at<VT_POINT>(i-1, 0)[1];
		X3 = 1.0f;

		xx = y.at<VT_POINT>(i-1, 0)[0];
		yy = y.at<VT_POINT>(i-1, 0)[1];
		zz = 1.0f;

		A.at<VT>(3*i-3, 3)[0] = -zz*X1;
		A.at<VT>(3*i-3, 4)[0] = (float)(-zz*X2); // HORRIBLE ROUNDING ERROR?
		A.at<VT>(3*i-3, 5)[0] = -zz*X3;

		A.at<VT>(3*i-3, 6)[0] = yy*X1;
		A.at<VT>(3*i-3, 7)[0] = yy*X2;
		A.at<VT>(3*i-3, 8)[0] = yy*X3;

		A.at<VT>(3*i-2, 0)[0] = zz*X1;
		A.at<VT>(3*i-2, 1)[0] = zz*X2;
		A.at<VT>(3*i-2, 2)[0] = zz*X3;

		A.at<VT>(3*i-2, 6)[0] = -xx*X1;
		A.at<VT>(3*i-2, 7)[0] = -xx*X2;
		A.at<VT>(3*i-2, 8)[0] = -xx*X3;

		A.at<VT>(3*i-1, 0)[0] = -yy*X1;
		A.at<VT>(3*i-1, 1)[0] = -yy*X2;
		A.at<VT>(3*i-1, 2)[0] = -yy*X3;

		A.at<VT>(3*i-1, 3)[0] = xx*X1;
		A.at<VT>(3*i-1, 4)[0] = xx*X2;
		A.at<VT>(3*i-1, 5)[0] = xx*X3;
	}

	cv::SVD svdThing(A);
	
	cv::Mat temp = svdThing.vt.t().col(8);

	retVal = cv::Mat( 3, 3, CV_32FC1 );

	retVal.at<VT>(0, 0)[0] = temp.at<VT>(0, 0)[0];
	retVal.at<VT>(1, 0)[0] = temp.at<VT>(1, 0)[0];
	retVal.at<VT>(2, 0)[0] = temp.at<VT>(2, 0)[0];

	retVal.at<VT>(0, 1)[0] = temp.at<VT>(3, 0)[0];
	retVal.at<VT>(1, 1)[0] = temp.at<VT>(4, 0)[0];
	retVal.at<VT>(2, 1)[0] = temp.at<VT>(5, 0)[0];

	retVal.at<VT>(0, 2)[0] = temp.at<VT>(6, 0)[0];
	retVal.at<VT>(1, 2)[0] = temp.at<VT>(7, 0)[0];
	retVal.at<VT>(2, 2)[0] = temp.at<VT>(8, 0)[0];
	
	return retVal.t();
}

/**
 * This function will take the four image corner vectors and intersect them with the ground to find their GPS locations
 * in UTM, meaning everything gets an (X, Y) coordinate.
 *
 * Knowing these four points and the four corresponding points on the image (which are simply the corner pixels), we can
 * compute a Homography H that will transform our image to look how it should as if viewed from a fronto-parallel viewpoint.
 *
 * This transformed image is perfectly aligned to a grid and any GPS coordinate can then simply be calculated by knowing 
 * the center GPS point and the translation from pixels to meters.
 *
 * These homographies should be calculated per full image and can be applied to any sub image to get its gps coordinates and
 * to rectify it.
 *
 */
array<float> ^ 
GeoReference::computeHomography( const double airplane_lat, const double airplane_lon, const double altitude, 
	const double airplane_roll, const double airplane_pitch, const double airplane_heading,
	const double gimbal_azimuth, const double gimbal_elevation, const double zoom_level )
{
	typedef cv::Vec<float, 1> VT;
	typedef cv::Vec<float, 2> VT_POINTS;
	array<float> ^ retVal = gcnew array<float>(9);
	const float f_base_horiz = 46.0 / 2.0 * Math::PI / 180.0;
	const float f_base_vert  = 34.1 / 2.0 * Math::PI / 180.0;

	// current location
	double northing, easting;
	char UTMZone[4];

	//LLtoUTM( 23, airplane_lat, airplane_lon, northing, easting, UTMZone);
	northing = easting = 0;

	// Camera pointing vector
	cv::Mat cameraVector;

	// points vector
	cv::Mat imgPoints = imagePoints();
	cv::Mat earthPoints( 5, 1, CV_32FC2 );

	// yaw pitch roll
	cv::Mat r_gimbal_to_airplane = eulerMatrix( gimbal_azimuth, gimbal_elevation, 0 );
	cv::Mat r_airplane_to_northEastDown = eulerMatrix( airplane_heading, airplane_pitch, airplane_roll );
	cv::Mat r_camera_to_gimbal;

	double yaw = f_base_horiz / zoom_level;
	double pitch = f_base_vert / zoom_level;
	double deltaNorthDown, deltaEastDown;

	// Middle
	r_camera_to_gimbal = eulerMatrix( 0, 0, 0 );
	cameraVector = r_airplane_to_northEastDown * r_gimbal_to_airplane * r_camera_to_gimbal * defaultVector();
	deltaNorthDown = divideZero( cameraVector.at<VT>(0, 0)[0], cameraVector.at<VT>(2, 0)[0] ) * altitude;
	deltaEastDown = divideZero( cameraVector.at<VT>(1, 0)[0],  cameraVector.at<VT>(2, 0)[0] ) * altitude;
	earthPoints.at<VT_POINTS>(4, 0) = VT_POINTS( deltaNorthDown + northing, deltaEastDown + easting );

	// Upper left corner 
	// pixel coords = 0, 0
	r_camera_to_gimbal = eulerMatrix( -yaw, pitch, 0 );
	cameraVector = r_camera_to_gimbal * defaultVector();

	cameraVector =  r_airplane_to_northEastDown * r_gimbal_to_airplane * r_camera_to_gimbal * defaultVector();
	deltaNorthDown = divideZero( cameraVector.at<VT>(0, 0)[0], cameraVector.at<VT>(2, 0)[0] ) * altitude;
	deltaEastDown = divideZero( cameraVector.at<VT>(1, 0)[0],  cameraVector.at<VT>(2, 0)[0] ) * altitude;

	earthPoints.at<VT_POINTS>(0, 0) = VT_POINTS( deltaNorthDown + northing, deltaEastDown + easting );

	// Upper right corner
	// pixel coords = 719, 0
	r_camera_to_gimbal = eulerMatrix( yaw, pitch, 0 );
	cameraVector = r_airplane_to_northEastDown * r_gimbal_to_airplane * r_camera_to_gimbal * defaultVector();
	deltaNorthDown = divideZero( cameraVector.at<VT>(0, 0)[0], cameraVector.at<VT>(2, 0)[0] ) * altitude;
	deltaEastDown = divideZero( cameraVector.at<VT>(1, 0)[0],  cameraVector.at<VT>(2, 0)[0] ) * altitude;

	earthPoints.at<VT_POINTS>(1, 0) = VT_POINTS( deltaNorthDown + northing, deltaEastDown + easting );

	// Lower right corner
	// pixel coords = 719, 485
	r_camera_to_gimbal = eulerMatrix( yaw, -pitch, 0 );
	cameraVector = r_airplane_to_northEastDown * r_gimbal_to_airplane * r_camera_to_gimbal * defaultVector();
	deltaNorthDown = divideZero( cameraVector.at<VT>(0, 0)[0], cameraVector.at<VT>(2, 0)[0] ) * altitude;
	deltaEastDown = divideZero( cameraVector.at<VT>(1, 0)[0],  cameraVector.at<VT>(2, 0)[0] ) * altitude;

	earthPoints.at<VT_POINTS>(2, 0) = VT_POINTS( deltaNorthDown + northing, deltaEastDown + easting );

	// Lower left corner
	// pixel coords = 0, 485
	r_camera_to_gimbal = eulerMatrix( -yaw, -pitch, 0 );
	cameraVector = r_airplane_to_northEastDown * r_gimbal_to_airplane * r_camera_to_gimbal * defaultVector();
	deltaNorthDown = divideZero( cameraVector.at<VT>(0, 0)[0], cameraVector.at<VT>(2, 0)[0] ) * altitude;
	deltaEastDown = divideZero( cameraVector.at<VT>(1, 0)[0],  cameraVector.at<VT>(2, 0)[0] ) * altitude;

	earthPoints.at<VT_POINTS>(3, 0) = VT_POINTS( deltaNorthDown + northing, deltaEastDown + easting );

	cv::Mat homo = homography( imgPoints, earthPoints );

	printMatrix( homo );

	retVal[0] = homo.at<VT>( 0, 0 )[0];
	retVal[1] = homo.at<VT>( 0, 1 )[0];
	retVal[2] = homo.at<VT>( 0, 2 )[0];
	retVal[3] = homo.at<VT>( 1, 0 )[0];
	retVal[4] = homo.at<VT>( 1, 1 )[0];
	retVal[5] = homo.at<VT>( 1, 2 )[0];
	retVal[6] = homo.at<VT>( 2, 0 )[0];
	retVal[7] = homo.at<VT>( 2, 1 )[0];
	retVal[8] = homo.at<VT>( 2, 2 )[0];

	return retVal;
}

/*
 * Reference usage of this code:
void main()
{
	double Lat = 47.37816667;
	double Long = 8.23250000;
	double UTMNorthing;
	double UTMEasting;
	double SwissNorthing;
	double SwissEasting;
	char UTMZone[4];
	int RefEllipsoid = 23;//WGS-84. See list with file "LatLong- UTM conversion.cpp" for id numbers

	cout << "Starting position(Lat, Long):  " << Lat << "   " << Long <<endl;

	LLtoUTM(RefEllipsoid, Lat, Long, UTMNorthing, UTMEasting, UTMZone);
	cout << setiosflags(ios::showpoint | ios::fixed) << setprecision(5);
	cout << "Calculated UTM position(Northing, Easting, Zone):  ";
	cout << UTMNorthing << "   " << UTMEasting;
	cout << "   " << UTMZone <<endl;
	
	UTMtoLL(RefEllipsoid, UTMNorthing, UTMEasting, UTMZone, Lat, Long);
	cout << "Calculated Lat, Long position(Lat, Long):  " << Lat << "   " << Long << endl <<endl;

	LLtoSwissGrid(Lat, Long, SwissNorthing, SwissEasting);
	cout << setiosflags(ios::showpoint | ios::fixed) << setprecision(5);
	cout << "Calculated Swiss Grid position(Northing, Easting):  ";
	cout << SwissNorthing << "   " << SwissEasting << endl;
}
*/