#pragma once

#include "TelemetryStructures.h"

using namespace System;
using namespace System::Data::Odbc;
//using namespace Communications;

namespace Database
{
	enum TableName
	{
		tableCandidateRegions,
		tableUnverifiedTargets,
		tableVerifiedTargets
	};

	enum ColumnName
	{
		columnID
	};

	public ref struct CandidateRowData
	{
		CandidateRowData() {
			shape = "Unkown";
			shapeColor = "Unkown";
			letter = "Unkown";
			letterColor = "Unkown";
			
			data = 0;

			dataWidth = 0;
			dataHeight = 0;
			dataNumChannels = 0;

			originX = 0;

			targetX = 0;
			targetY = 0;
		
			gpsAltitude = 0;
			gpsLatitude = 0;
			gpsLongitude = 0;
			altitudeAboveLaunch = 0;
			velocity = 0;
			planeRollDegrees = 0;
			planePitchDegrees = 0;
			planeHeadingDegres = 0;

			gimbalRollDegrees = 0;
			gimbalPitchDegrees = 0;

			zoom = 0;

			gimbalRollRateDegrees = 0;
			gimbalPitchRateDegrees = 0;
		
			planeRollRateDegrees = 0;
			planePitchRateDegrees = 0;
			planeHeadingRateDegres = 0;

			timestamp = 0;
		}

		String ^ shape;
		String ^ shapeColor;
		String ^ letter;
		String ^ letterColor;

		float * data;

		int dataWidth;
		int dataHeight;
		int dataNumChannels;

		int originX;

		int targetX;
		int targetY;
		
		float gpsAltitude;
		float gpsLatitude;
		float gpsLongitude;
		float altitudeAboveLaunch;
		float velocity;
		float planeRollDegrees;
		float planePitchDegrees;
		float planeHeadingDegres;

		float gimbalRollDegrees;
		float gimbalPitchDegrees;

		float zoom;

		float gimbalRollRateDegrees;
		float gimbalPitchRateDegrees;
		
		float planeRollRateDegrees;
		float planePitchRateDegrees;
		float planeHeadingRateDegres;

		
		double timestamp;
	};

	public ref struct RowData
	{
		RowData(void);

		int id;							// Unique ID for this entry
		String ^ path;					// Path to a file for the image
		float target_latitude;			// Latitude of point selected as target
		float target_longitude;			// Longitude of point selected as target
		int target_X;					// pixel X of target
		int target_Y;					// pixel Y of target
		float heading;					// heading of target
		String ^ letter;				
		String ^ shape;					
		String ^ fg_color;				// foreground color
		String ^ bg_color;				// background color
		bool processed;					// whether this has been processed by OpenCV
		bool verified;					// human verified
		float center_latitude;			// Latitude of center pixel
		float center_longitude;			// Longitude of center pixel
		float mapping_latitude;			// pixel to meter translation for latitude
		float mapping_longitude;		// pixel to meter translation for longitude
		array<float> ^ homography;		// homography used to transform this image
	};

	public ref class DatabaseConnection
	{
	public:
		DatabaseConnection(void);
		~DatabaseConnection(void);

		String ^ getField( TableName table, int field, String ^ rowID );
		bool connect(void);
		bool disconnect(void);
		void fillDatabase();
		bool reset(void);
		void saveNewCandidate(float * data, int width, int height, int numChannels, int originX, int originY, Communications::PlaneState ^ stateOfPlane);
		 
		bool insertData( TableName table, RowData ^ data );
		bool updateValue( TableName table, String ^ field, String ^ value, String ^ rowID );
		String ^ formatHomography( array<float> ^ input );
		bool move( TableName source, TableName dest, String ^ verifiedStatus );		
		bool moveProcessed( TableName source, TableName dest, String ^ processedStatus );

		array<float> ^ getHomography( TableName table, String ^ rowID );

	protected:
		String ^ getTableName( TableName table );
		

	private:
		OdbcConnection ^ _database;
		OdbcCommand ^ _command;
		OdbcDataReader ^ _reader;
	};
}
