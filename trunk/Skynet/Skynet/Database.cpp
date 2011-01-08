#include "StdAfx.h"
#include "Database.h"

using namespace System;
using namespace System::Data::Odbc;
using namespace Database;

RowData::RowData(void)
{
	id = 0;							// Unique ID for this entry
	path = "not a path";					// Path to a file for the image
	target_latitude = 0.0;			// Latitude of point selected as target
	target_longitude = 0.0;			// Longitude of point selected as target
	target_X = 0;					// pixel X of target
	target_Y = 0;					// pixel Y of target
	heading = 0.0;					// heading of target
	letter = "-";				
	shape = "----";					
	fg_color = "----";				// foreground color
	bg_color = "----";				// background color
	processed = FALSE;					// whether this has been processed by OpenCV
	verified = FALSE;					// human verified
	center_latitude = 0.0;			// Latitude of center pixel
	center_longitude = 0.0;			// Longitude of center pixel
	mapping_latitude = 0.0;			// pixel to meter translation for latitude
	mapping_longitude = 0.0;
	homography = gcnew array<float>(9);
	for( int i = 0; i < 9; i++)
	{
		homography[i] = 0.0;
	}
}

DatabaseConnection::DatabaseConnection(void)
{
	_database = gcnew OdbcConnection("Driver={PostGreSQL 64-Bit ODBC Drivers};Server=localhost;Database=AUVSI_metadata;UID=postgres;PWD=triton");
	_command = gcnew OdbcCommand();	
	_database->Open();
	//OdbcCommand ^ command = gcnew OdbcCommand("SELECT * FROM candidate", _database);
	//_command->Connection = _database;
	//_command->CommandText = "INSERT INTO candidate (id, target_lat, target_lon, letter) VALUES (5, 987.5, 987.5, 'B')";
	
	// Execute the SQL command and return a reader for navigating the results.
	//OdbcDataReader ^ reader = _command->ExecuteReader();


	//reader->Close();

	
}

DatabaseConnection::~DatabaseConnection(void)
{
	disconnect();
}

bool
DatabaseConnection::connect(void)
{
	bool retVal = false;

	try
	{
		if( _database->State == System::Data::ConnectionState::Closed )
			_database->Open();
		retVal = true;
	}
	catch( Exception ^ )
	{
		System::Diagnostics::Trace::WriteLine("ERROR in database.cpp: connect()");

	}

	return retVal;
}

bool
DatabaseConnection::disconnect(void)
{
	bool retVal = false;

	try
	{
		_database->Close();
		retVal = true;
	}
	catch( Exception ^ )
	{
	
		System::Diagnostics::Trace::WriteLine("catch in database");
	}

	return retVal;
}

bool
DatabaseConnection::reset(void)
{
	bool retVal = false;
	_command->Connection = _database;
	_command->CommandText = "TRUNCATE TABLE " + getTableName( tableCandidateRegions ) + ";" + 
						    "TRUNCATE TABLE " + getTableName( tableUnverifiedTargets ) + ";" + 
							 "TRUNCATE TABLE " + getTableName( tableVerifiedTargets );	
	try
	{
		OdbcDataReader ^ reader = _command->ExecuteReader();
		reader->Close();
		retVal = true;
	}
	catch( Exception ^ )
	{
	
		System::Diagnostics::Trace::WriteLine("catch in database");
	}
	
	return retVal;
}

String ^
DatabaseConnection::formatHomography( array<float> ^ input )
{
	String ^ retVal = "'{";
	for( int i = 0; i < 8; ++i )
	{
		retVal += input[i] + ", ";
	}

	retVal += input[8];

	retVal += "}'";

	return retVal;
}

array<float> ^
DatabaseConnection::getHomography( TableName table, String ^ rowID )
{
	array<float> ^ retVal = gcnew array<float>(9);
	array<wchar_t> ^ splitValues = {Convert::ToChar(",")};
	array<String ^> ^ split;

	// 13 for candidate
	int columnNumber = (table == tableCandidateRegions) ? 13 : 17;
	String ^ homographyAsString = getField( table, columnNumber, rowID );
	homographyAsString = homographyAsString->Replace("{", "");
	homographyAsString = homographyAsString->Replace("}", "");
	split = homographyAsString->Split( splitValues );
	for( int i = 0; i < 9; ++i )
	{
		retVal[i] = Convert::ToDouble( split[i] );
		//System::Diagnostics::Trace::WriteLine(split[i]);
	}

	//System::Diagnostics::Trace::WriteLine(homographyAsString);

	return retVal;
}


String ^ DatabaseConnection::getField( TableName table, int field, String ^ rowID )
{
	String ^ retString;
	_command->Connection = _database;
	_command->CommandText  ="SELECT * FROM " + getTableName( table ) + " WHERE id='" + rowID + "'";	

	//System::Diagnostics::Trace::WriteLine(_command->CommandText);
	try
	{
		OdbcDataReader ^ reader = _command->ExecuteReader();
		reader->HasRows;
		//System::Diagnostics::Trace::WriteLine(reader->HasRows);
		retString = Convert::ToString(reader[field]);
		reader->Close();
	}
	catch( Exception ^ )
	{
	
		System::Diagnostics::Trace::WriteLine("catch in database");
	}
	
	return retString;
}



bool 
DatabaseConnection::insertData( TableName table, RowData ^ data )
{
	bool retVal = false;
	_command->Connection = _database;
		//inserting the array makes it mad.  FOR ARRAY USE '{0.0,1.1,2.2}'
	_command->CommandText = "INSERT INTO " + getTableName( table ) + " (id, thumb, target_lat," + 
			" target_lon, target_x, target_y, heading, letter, shape, fg_color, bg_color," + 
			" processed, verified, center_lat, center_lon, mapping_lat, mapping_lon, homography) VALUES (" +
			data->id + ", '" + data->path + "', " + data->target_latitude + ", " + data->target_longitude + ", " +
			data->target_X + ", " + data->target_Y + ", " + data->heading + ", '" + data->letter + "', '" +
			data->shape + "', '" + data->fg_color + "', '" + data->bg_color + "', " + data->processed + ", " +
			data->verified + ", " + data->center_latitude + ", " + data->center_longitude + ", " +
			data->mapping_latitude + ", " + data->mapping_longitude + ", " +
			formatHomography( data->homography ) + ")";
	
	//System::Diagnostics::Trace::WriteLine(_command->CommandText);
	
	try
	{
		OdbcDataReader ^ reader = _command->ExecuteReader();
		reader->Close();
		retVal = true;
	}
	catch( Exception ^ )
	{
		System::Diagnostics::Trace::WriteLine("catch in database");

	}
	
	return retVal;
}

bool DatabaseConnection::updateValue( TableName table, String ^ field, String ^ value, String ^ rowID )
{
	bool retVal = false;
	_command->Connection = _database;
	_command->CommandText = "UPDATE " + getTableName(table) + " SET " + field + "='" + value + "' WHERE id='" + rowID + "'";
	
	//System::Diagnostics::Trace::WriteLine(_command->CommandText);
	
	try
	{
		OdbcDataReader ^ reader = _command->ExecuteReader();
		reader->Close();
		retVal = true;
	}
	catch( Exception ^ )
	{
		System::Diagnostics::Trace::WriteLine("catch in database");

	}
	
	return retVal;
}

bool DatabaseConnection::move( TableName source, TableName dest, String ^ verifiedStatus )
{
	bool retVal = false;
	_command->Connection = _database;
	_command->CommandText = "INSERT INTO " + getTableName(dest) + " SELECT * FROM " + getTableName(source) + " WHERE verified='" + verifiedStatus + 
							"'; DELETE FROM " + getTableName(source) + " WHERE verified='" + verifiedStatus + "'";
	
	//System::Diagnostics::Trace::WriteLine(_command->CommandText);
	
	try
	{
		OdbcDataReader ^ reader = _command->ExecuteReader();
		reader->Close();
		retVal = true;
	}
	catch( Exception ^ )
	{
		System::Diagnostics::Trace::WriteLine("catch in database");

	}
	
	return retVal;

}

bool DatabaseConnection::moveProcessed( TableName source, TableName dest, String ^ processedStatus )
{
	bool retVal = false;
	_command->Connection = _database;
	_command->CommandText = "INSERT INTO " + getTableName(dest) + " SELECT * FROM " + getTableName(source) + " WHERE processed='" + processedStatus + 
							"'; DELETE FROM " + getTableName(source) + " WHERE processed='" + processedStatus + "'";
	
	//System::Diagnostics::Trace::WriteLine(_command->CommandText);
	
	try
	{
		OdbcDataReader ^ reader = _command->ExecuteReader();
		reader->Close();
		retVal = true;
	}
	catch( Exception ^ )
	{
		System::Diagnostics::Trace::WriteLine("catch in database");

	}
	
	return retVal;

}

String ^
DatabaseConnection::getTableName( TableName table )
{
	String ^ retVal;

	switch( table )
	{
	case tableCandidateRegions:
		retVal = "candidate";
		break;
	case tableUnverifiedTargets:
		retVal = "unverified";
		break;
	case tableVerifiedTargets:
		retVal = "verified";
		break;
	}

	return retVal;
}