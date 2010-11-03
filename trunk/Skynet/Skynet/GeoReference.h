#pragma once

#include <cv.h>

namespace GeoReference
{
	// Source code taken from http://www.gpsy.com/gpsinfo/geotoutm/
	class Ellipsoid
	{
	public:
		Ellipsoid(){};
		Ellipsoid(int Id, char* name, double radius, double ecc)
		{
			id = Id; ellipsoidName = name; 
			EquatorialRadius = radius; eccentricitySquared = ecc;
		}

		int id;
		char* ellipsoidName;
		double EquatorialRadius; 
		double eccentricitySquared;  
	};

	void LLtoUTM(int ReferenceEllipsoid, const double Lat, const double Long, 
			 double &UTMNorthing, double &UTMEasting, char* UTMZone);

	void UTMtoLL(int ReferenceEllipsoid, const double UTMNorthing, const double UTMEasting, const char* UTMZone,
				  double& Lat,  double& Long );

	array<float> ^ computeHomography( const double airplane_lat, const double airplane_lon, const double altitude, 
		const double airplane_roll, const double airplane_pitch, const double airplane_heading,
		const double gimbal_azimuth, const double gimbal_elevation, const double zoom_level );

	cv::Mat eulerMatrix( double phi, double theta, double psi );
	cv::Mat imagePoints( void );
	void applyHomography( array<float> ^ homography, int pixelX, int pixelY, double airplane_lat, double airplane_lon, double & lat, double & lon  );

	char UTMLetterDesignator(double Lat);

	const double PI = 3.14159265;
	const double FOURTHPI = PI / 4;
	const double deg2rad = PI / 180;
	const double rad2deg = 180.0 / PI;
	const int WGS_84_ELLIPSOID = 23;

	static Ellipsoid ellipsoid[] = 
	{//  id, Ellipsoid name, Equatorial Radius, square of eccentricity	
		Ellipsoid( -1, "Placeholder", 0, 0),//placeholder only, To allow array indices to match id numbers
		Ellipsoid( 1, "Airy", 6377563, 0.00667054),
		Ellipsoid( 2, "Australian National", 6378160, 0.006694542),
		Ellipsoid( 3, "Bessel 1841", 6377397, 0.006674372),
		Ellipsoid( 4, "Bessel 1841 (Nambia) ", 6377484, 0.006674372),
		Ellipsoid( 5, "Clarke 1866", 6378206, 0.006768658),
		Ellipsoid( 6, "Clarke 1880", 6378249, 0.006803511),
		Ellipsoid( 7, "Everest", 6377276, 0.006637847),
		Ellipsoid( 8, "Fischer 1960 (Mercury) ", 6378166, 0.006693422),
		Ellipsoid( 9, "Fischer 1968", 6378150, 0.006693422),
		Ellipsoid( 10, "GRS 1967", 6378160, 0.006694605),
		Ellipsoid( 11, "GRS 1980", 6378137, 0.00669438),
		Ellipsoid( 12, "Helmert 1906", 6378200, 0.006693422),
		Ellipsoid( 13, "Hough", 6378270, 0.00672267),
		Ellipsoid( 14, "International", 6378388, 0.00672267),
		Ellipsoid( 15, "Krassovsky", 6378245, 0.006693422),
		Ellipsoid( 16, "Modified Airy", 6377340, 0.00667054),
		Ellipsoid( 17, "Modified Everest", 6377304, 0.006637847),
		Ellipsoid( 18, "Modified Fischer 1960", 6378155, 0.006693422),
		Ellipsoid( 19, "South American 1969", 6378160, 0.006694542),
		Ellipsoid( 20, "WGS 60", 6378165, 0.006693422),
		Ellipsoid( 21, "WGS 66", 6378145, 0.006694542),
		Ellipsoid( 22, "WGS-72", 6378135, 0.006694318),
		Ellipsoid( 23, "WGS-84", 6378137, 0.00669438)
	};

	// Using NTSC camera - pixel aspect ratio is 11:10
	// Using Sony FCB EX 11D, focal length linear from 4.2mm (zoom level 1)
	// to 42.0mm (zoom level 10)
};

