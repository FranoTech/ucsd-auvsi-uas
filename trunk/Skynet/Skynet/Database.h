#pragma once

using namespace System;
using namespace System::Data::Odbc;

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

	ref struct RowData
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

	ref class DatabaseConnection
	{
	public:
		DatabaseConnection(void);
		~DatabaseConnection(void);

		String ^ getField( TableName table, int field, String ^ rowID );
		bool connect(void);
		bool disconnect(void);
		void fillDatabase();
		bool reset(void);
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
