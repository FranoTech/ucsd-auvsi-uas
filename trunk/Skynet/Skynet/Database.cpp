#include "StdAfx.h"
#include "Database.h"

#include "AutopilotComport.h"
#include <msclr/lock.h>

using namespace System;
using namespace System::Data::Odbc;
using namespace Database;
using namespace Communications;
using namespace msclr;

#define DATABASE_NAME	"AUVSI_flightdata" //"AUVSI_metadata"

DatabaseConnection::DatabaseConnection(void)
{
	databaseOpen = false;
	try {
		_database = gcnew OdbcConnection("Driver={PostGreSQL 64-Bit ODBC Drivers};Server=localhost;Database=" + DATABASE_NAME + ";UID=postgres;PWD=triton");
		_command = gcnew OdbcCommand();	
		_database->Open();
		_command->Connection = _database;
		databaseOpen = true;
	}

	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::DatabaseConnection(): Could not connect to database - " + e);
	}
	//OdbcCommand ^ command = gcnew OdbcCommand("SELECT * FROM candidate", _database);
	//_command->Connection = _database;
	//_command->CommandText = "INSERT INTO candidate (id, target_lat, target_lon, letter) VALUES (5, 987.5, 987.5, 'B')";
	
	// Execute the SQL command and return a reader for navigating the results.
	//OdbcDataReader ^ reader = _command->ExecuteReader();


	//reader->Close();
	//fillDatabase();
	
}

void DatabaseConnection::saveNewCandidate( CandidateRowData ^ data)
{
	//String ^ retString;

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::saveNewCandidateWithData() running");

	if (data == nullptr) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::saveNewCandidateWithData(): data == nullptr");
		return;
	}
	if (_database->State == System::Data::ConnectionState::Closed) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::saveNewCandidateWithData(): database not open.");
		return;
	}

	
	lock l(_database);
	_command->Connection = _database;

	// form command
	String ^ commandText = "INSERT INTO computeroutput (id, shape, shapecolor, letter, lettercolor, \"timestamp\", \"gpsAltitude\", \"gpsLatitude\", \"gpsLongitude\", \"altitudeAboveLaunch\""
		+", velocity, \"planeRoll\", \"planePitch\", \"planeHeading\", \"gimbalRoll\", \"gimbalPitch\", zoom, \"gimbalRollRate\", \"gimbalPitchRate\", \"planeRollRate\", \"planePitchRate\""
		+", \"planeHeadingRate\", imagename, width, height, \"numChannels\", \"originX\", \"originY\", \"targetX\", \"targetY\") VALUES (" + data->id + ", "
		+ "'" + data->shape + "'" +", "
		+ "'" + data->shapeColor + "'" + ", "
		+ "'" + data->letter + "'"  + ", "
		+ "'" + data->letterColor + "'" + ", "
		+  data->timestamp + ", "
		+  data->gpsAltitude + ", "
		+  data->gpsLatitude + ", "
		+  data->gpsLongitude + ", "
		+  data->altitudeAboveLaunch + ", "
		+  data->velocity + ", "
		+  data->planeRollDegrees + ", "
		+  data->planePitchDegrees + ", "
		+  data->planeHeadingDegrees + ", "
		+  data->gimbalRollDegrees + ", "
		+  data->gimbalPitchDegrees + ", "
		+  data->zoom + ", "
		+  data->gimbalRollRateDegrees + ", "
		+ data->gimbalPitchRateDegrees + ", "
		+ data->planeRollRateDegrees + ", "
		+ data->planePitchRateDegrees + ", "
		+ data->planeHeadingRateDegrees + ", "
		+ "'" + data->imageName + "'" + ", "
		+ data->dataWidth + ", "
		+ data->dataHeight + ", "
		+ data->dataNumChannels + ", "
		+ data->originX + ", "
		+ data->originY + ", "
		+ data->targetX + ", "
		+ data->targetY + ")";

	_command->CommandText = commandText;
	//System::Diagnostics::Trace::WriteLine(_command->CommandText);
	// execute command
	try
	{
		_reader = _command->ExecuteReader();
		_reader->HasRows;
		_reader->Close();
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("DatabaseConnection::saveNewCandidate(): Executing command failed :( " + e);
		_reader->Close();
	}

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::saveNewCandidate() finished");
}


void DatabaseConnection::saveNewTarget( TargetRowData ^ data)
{
	//String ^ retString;

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::saveNewTarget() running");

	if (data == nullptr) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::saveNewTarget(): data == nullptr");
		return;
	}
	if (_database->State == System::Data::ConnectionState::Closed) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::saveNewTarget(): database not open.");
		return;
	}

	
	lock l(_database);
	_command->Connection = _database;

	// form command
	String ^ commandText = "INSERT INTO identifiedtargets (id, shape, shapecolor, letter, lettercolor, \"timestamp\", \"gpsAltitude\", \"gpsLatitude\", \"gpsLongitude\", \"altitudeAboveLaunch\""
		+", velocity, \"planeRoll\", \"planePitch\", \"planeHeading\", \"gimbalRoll\", \"gimbalPitch\", zoom, \"gimbalRollRate\", \"gimbalPitchRate\", \"planeRollRate\", \"planePitchRate\""
		+", \"planeHeadingRate\", imagename, width, height, \"numChannels\", \"originX\", \"originY\", \"targetX\", \"targetY\", \"topOfTargetX\", \"topOfTargetY\") VALUES (" + data->id + ", "
		+ "'" + data->shape + "'" +", "
		+ "'" + data->shapeColor + "'" + ", "
		+ "'" + data->letter + "'"  + ", "
		+ "'" + data->letterColor + "'" + ", "
		+  data->timestamp + ", "
		+  data->gpsAltitude + ", "
		+  data->gpsLatitude + ", "
		+  data->gpsLongitude + ", "
		+  data->altitudeAboveLaunch + ", "
		+  data->velocity + ", "
		+  data->planeRollDegrees + ", "
		+  data->planePitchDegrees + ", "
		+  data->planeHeadingDegrees + ", "
		+  data->gimbalRollDegrees + ", "
		+  data->gimbalPitchDegrees + ", "
		+  data->zoom + ", "
		+  data->gimbalRollRateDegrees + ", "
		+  data->gimbalPitchRateDegrees + ", "
		+  data->planeRollRateDegrees + ", "
		+  data->planePitchRateDegrees + ", "
		+  data->planeHeadingRateDegrees + ", "
		+  "'" + data->imageName + "'" + ", "
		+  data->dataWidth + ", "
		+  data->dataHeight + ", "
		+  data->dataNumChannels + ", "
		+  data->originX + ", "
		+  data->originY + ", "
		+  data->targetX + ", "
		+  data->targetY + ", "
		+  data->topOfTargetX + ", "
		+  data->topOfTargetX + ")";

	_command->CommandText = commandText;
	//System::Diagnostics::Trace::WriteLine(_command->CommandText);
	// execute command
	try
	{
		_reader = _command->ExecuteReader();
		_reader->HasRows;
		_reader->Close();
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("DatabaseConnection::saveNewTarget(): Executing command failed :( " + e);
		_reader->Close();
	}

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::saveNewTarget() finished");
}


void DatabaseConnection::modifyCandidate( CandidateRowData ^ data)
{
	//String ^ retString;

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::modifyCandidate() running");

	if (data == nullptr) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::modifyCandidate(): data == nullptr");
		return;
	}
	if (_database->State == System::Data::ConnectionState::Closed) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::modifyCandidate(): database not open.");
		return;
	}

	
	lock l(_database);
	_command->Connection = _database;

	// form command
	String ^ commandText = "UPDATE computeroutput\n"
		+ "SET id=" + data->id 
		+ ", shape='" + data->shape + "'" 
		+ ", shapecolor='" + data->shapeColor + "'"
		+ ", letter='" + data->letter + "'"
		+ ", lettercolor='" + data->letterColor + "'" 
		+ ", \"timestamp\"=" + data->timestamp 
		+ ", \"gpsAltitude\"=" + data->gpsAltitude 
		+ ", \"gpsLatitude\"=" + data->gpsLatitude 
		+ ", \"gpsLongitude\"=" + data->gpsLongitude 
		+ ", \"altitudeAboveLaunch\"=" + data->altitudeAboveLaunch 
		+ ", velocity="+ data->velocity 
		+ ", \"planeRoll\"="+ data->planeRollDegrees 
		+ ", \"planePitch\"="+ data->planePitchDegrees 
		+ ", \"planeHeading\"="+ data->planeHeadingDegrees 
		+ ", \"gimbalRoll\"="+ data->gimbalRollDegrees
		+ ", \"gimbalPitch\"="+ data->gimbalPitchDegrees
		+ ", zoom="+ data->zoom 
		+ ", \"gimbalRollRate\"="+ data->gimbalRollRateDegrees
		+ ", \"gimbalPitchRate\"="+ data->gimbalPitchRateDegrees
		+ ", \"planeRollRate\"="+ data->planeRollRateDegrees
		+ ", \"planePitchRate\"="+ data->planePitchRateDegrees
		+ ", \"planeHeadingRate\"="+ data->planeHeadingRateDegrees
		+ ", imagename='"+ data->imageName + "'"
		+ ", width="+ data->dataWidth 
		+ ", height="+ data->dataHeight
		+ ", \"numChannels\"="+ data->dataNumChannels
		+ ", \"originX\"="+ data->originX
		+ ", \"originY\""+ data->originY
		+ ", \"targetX\"="+ data->targetX
		+ ", \"targetY\"="+ data->targetY
		+ " WHERE id=" + data->id;

	_command->CommandText = commandText;
	System::Diagnostics::Trace::WriteLine(_command->CommandText);
	// execute command
	try
	{
		_reader = _command->ExecuteReader();
		_reader->HasRows;
		_reader->Close();
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("DatabaseConnection::modifyCandidate(): Executing command failed :( " + e);
		_reader->Close();
	}

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::modifyCandidate() finished");
}

void DatabaseConnection::modifyTarget( TargetRowData ^ data)
{
	//String ^ retString;

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::modifyTarget() running");

	if (data == nullptr) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::modifyTarget(): data == nullptr");
		return;
	}
	if (_database->State == System::Data::ConnectionState::Closed) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::modifyTarget(): database not open.");
		return;
	}

	
	lock l(_database);
	_command->Connection = _database;

	// form command
	String ^ commandText = "UPDATE identifiedtargets\n"
		+ "SET id=" + data->id 
		+ ", shape='" + data->shape + "'" 
		+ ", shapecolor='" + data->shapeColor + "'"
		+ ", letter='" + data->letter + "'"
		+ ", lettercolor='" + data->letterColor + "'" 
		+ ", \"timestamp\"=" + data->timestamp 
		+ ", \"gpsAltitude\"=" + data->gpsAltitude 
		+ ", \"gpsLatitude\"=" + data->gpsLatitude 
		+ ", \"gpsLongitude\"=" + data->gpsLongitude 
		+ ", \"altitudeAboveLaunch\"=" + data->altitudeAboveLaunch 
		+ ", velocity="+ data->velocity 
		+ ", \"planeRoll\"="+ data->planeRollDegrees 
		+ ", \"planePitch\"="+ data->planePitchDegrees 
		+ ", \"planeHeading\"="+ data->planeHeadingDegrees 
		+ ", \"gimbalRoll\"="+ data->gimbalRollDegrees
		+ ", \"gimbalPitch\"="+ data->gimbalPitchDegrees
		+ ", zoom="+ data->zoom 
		+ ", \"gimbalRollRate\"="+ data->gimbalRollRateDegrees
		+ ", \"gimbalPitchRate\"="+ data->gimbalPitchRateDegrees
		+ ", \"planeRollRate\"="+ data->planeRollRateDegrees
		+ ", \"planePitchRate\"="+ data->planePitchRateDegrees
		+ ", \"planeHeadingRate\"="+ data->planeHeadingRateDegrees
		+ ", imagename='"+ data->imageName + "'"
		+ ", width="+ data->dataWidth 
		+ ", height="+ data->dataHeight
		+ ", \"numChannels\"="+ data->dataNumChannels
		+ ", \"originX\"="+ data->originX
		+ ", \"originY\""+ data->originY
		+ ", \"targetX\"="+ data->targetX
		+ ", \"targetY\"="+ data->targetY
		+ ", \"topOfTargetX\"="+ data->topOfTargetX
		+ ", \"topOfTargetY\"="+ data->topOfTargetY
		+ " WHERE id=" + data->id;

	_command->CommandText = commandText;
	System::Diagnostics::Trace::WriteLine(_command->CommandText);
	// execute command
	try
	{
		_reader = _command->ExecuteReader();
		_reader->HasRows;
		_reader->Close();
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("DatabaseConnection::modifyTarget(): Executing command failed :( " + e);
		_reader->Close();
	}

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::modifyTarget() finished");
}

void DatabaseConnection::removeCandidate( String ^ id)
{
	//String ^ retString;

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::removeCandidate() running");

	if (id == nullptr) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::removeCandidate(): id == nullptr");
		return;
	}
	if (_database->State == System::Data::ConnectionState::Closed) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::removeCandidate(): database not open.");
		return;
	}

	
	lock l(_database);
	_command->Connection = _database;

	// form command
	String ^ commandText = "DELETE FROM computeroutput WHERE id=" + id;

	_command->CommandText = commandText;
	System::Diagnostics::Trace::WriteLine(_command->CommandText);
	// execute command
	try
	{
		_reader = _command->ExecuteReader();
		_reader->HasRows;
		_reader->Close();
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("DatabaseConnection::removeCandidate(): Executing command failed :( " + e);
		_reader->Close();
	}

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::removeCandidate() finished");
}

void DatabaseConnection::removeTarget( String ^id )
{
	//String ^ retString;

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::removeTarget() running");

	if (id == nullptr) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::removeTarget(): id == nullptr");
		return;
	}
	if (_database->State == System::Data::ConnectionState::Closed) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::removeTarget(): database not open.");
		return;
	}

	lock l(_database);
	_command->Connection = _database;

	// form command
	String ^ commandText = "DELETE FROM identifiedtargets WHERE id=" + id;

	_command->CommandText = commandText;
	System::Diagnostics::Trace::WriteLine(_command->CommandText);
	// execute command
	try
	{
		_reader = _command->ExecuteReader();
		_reader->HasRows;
		_reader->Close();
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("DatabaseConnection::removeTarget(): Executing command failed :( " + e);
		_reader->Close();
	}

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::removeTarget() finished");
}

array<CandidateRowData ^>^ DatabaseConnection::getAllCandidates()
{
	int size = 20;
	array<CandidateRowData ^>^ retArr = gcnew array<CandidateRowData ^>(size);
	System::Diagnostics::Trace::WriteLine("DatabaseConnection::getAllCandidates() running");

	if (_database->State == System::Data::ConnectionState::Closed) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::getAllCandidates(): database not open.");
		return nullptr;
	}

	
	lock l(_database);
	_command->Connection = _database;

	// form command
	String ^ commandText = "SELECT * FROM computeroutput";

	_command->CommandText = commandText;
	System::Diagnostics::Trace::WriteLine(_command->CommandText);
	// execute command
	try
	{
		_reader = _command->ExecuteReader();
		if (_reader->HasRows)
		{
			int i = 0;
			while (_reader->Read())
			{
				retArr[i++] = candidateFromReader(_reader);

				if (i >= size) {
					size += 10;
					Array::Resize(retArr, size);
				}

			}

			Array::Resize(retArr, i);

		}
		else {
			retArr = nullptr;
		}
		_reader->Close();
	
	
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("DatabaseConnection::getAllCandidates(): Executing command failed :( " + e);
		
		_reader->Close();
		return nullptr;
	}

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::getAllCandidates() finished");
	return retArr;
}

// TODO: implement next three functions
CandidateRowData ^ DatabaseConnection::candidateFromReader(OdbcDataReader ^ theReader)
{
	CandidateRowData ^ newRow = gcnew CandidateRowData();

	// fill row
	int col = 0;
	newRow->id = theReader->GetInt32(col++);

	newRow->shape = theReader->GetString(col++);
	newRow->shapeColor = theReader->GetString(col++);
	newRow->letter = theReader->GetString(col++);
	newRow->letterColor = theReader->GetString(col++);
				
	newRow->timestamp = theReader->GetDouble(col++);
		
	newRow->gpsAltitude = theReader->GetFloat(col++);
	newRow->gpsLatitude = theReader->GetFloat(col++);
	newRow->gpsLongitude = theReader->GetFloat(col++);

	newRow->altitudeAboveLaunch = theReader->GetFloat(col++);
	newRow->velocity = theReader->GetFloat(col++);
	newRow->planeRollDegrees =theReader->GetFloat(col++);
	newRow->planePitchDegrees = theReader->GetFloat(col++);
	newRow->planeHeadingDegrees = theReader->GetFloat(col++);

	newRow->gimbalRollDegrees = theReader->GetFloat(col++);
	newRow->gimbalPitchDegrees = theReader->GetFloat(col++);

	newRow->zoom = theReader->GetFloat(col++);

	newRow->gimbalRollRateDegrees = theReader->GetFloat(col++);
	newRow->gimbalPitchRateDegrees =theReader->GetFloat(col++);
		
	newRow->planeRollRateDegrees = theReader->GetFloat(col++);
	newRow->planePitchRateDegrees =theReader->GetFloat(col++);
	newRow->planeHeadingRateDegrees = theReader->GetFloat(col++);

	newRow->imageName = theReader->GetString(col++);

	newRow->dataWidth = theReader->GetInt32(col++);
	newRow->dataHeight = theReader->GetInt32(col++);
	newRow->dataNumChannels = theReader->GetInt32(col++);

	newRow->originX = theReader->GetInt32(col++);
	newRow->originY = theReader->GetInt32(col++);

	newRow->targetX = theReader->GetInt32(col++);
	newRow->targetY = theReader->GetInt32(col++);
	
	return newRow;
}


TargetRowData ^ DatabaseConnection::targetFromReader(OdbcDataReader ^ theReader)
{
	TargetRowData ^ newRow = gcnew TargetRowData();

	// fill row
	int col = 0;
	newRow->id = theReader->GetInt32(col++);

	newRow->shape = theReader->GetString(col++);
	newRow->shapeColor = theReader->GetString(col++);
	newRow->letter = theReader->GetString(col++);
	newRow->letterColor = theReader->GetString(col++);
				
	newRow->timestamp = theReader->GetDouble(col++);

				
	newRow->gpsAltitude = theReader->GetFloat(col++);
	newRow->gpsLatitude = theReader->GetFloat(col++);
	newRow->gpsLongitude = theReader->GetFloat(col++);

	newRow->altitudeAboveLaunch = theReader->GetFloat(col++);
	newRow->velocity = theReader->GetFloat(col++);
	newRow->planeRollDegrees =theReader->GetFloat(col++);
	newRow->planePitchDegrees = theReader->GetFloat(col++);
	newRow->planeHeadingDegrees = theReader->GetFloat(col++);

	newRow->gimbalRollDegrees = theReader->GetFloat(col++);
	newRow->gimbalPitchDegrees = theReader->GetFloat(col++);

	newRow->zoom = theReader->GetFloat(col++);

	newRow->gimbalRollRateDegrees = theReader->GetFloat(col++);
	newRow->gimbalPitchRateDegrees =theReader->GetFloat(col++);
		
	newRow->planeRollRateDegrees = theReader->GetFloat(col++);
	newRow->planePitchRateDegrees =theReader->GetFloat(col++);
	newRow->planeHeadingRateDegrees = theReader->GetFloat(col++);

	newRow->imageName = theReader->GetString(col++);

	newRow->dataWidth = theReader->GetInt32(col++);
	newRow->dataHeight = theReader->GetInt32(col++);
	newRow->dataNumChannels = theReader->GetInt32(col++);

	newRow->originX = theReader->GetInt32(col++);
	newRow->originY = theReader->GetInt32(col++);

	newRow->targetX = theReader->GetInt32(col++);
	newRow->targetY = theReader->GetInt32(col++);

	newRow->topOfTargetX = theReader->GetInt32(col++);
	newRow->topOfTargetY = theReader->GetInt32(col++);
		

	return newRow;
}

CandidateRowData ^ DatabaseConnection::candidateWithID(String ^ id)
{
	CandidateRowData ^ retData = nullptr;
	System::Diagnostics::Trace::WriteLine("DatabaseConnection::candidateWithID() running");

	if (_database->State == System::Data::ConnectionState::Closed) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::candidateWithID(): database not open.");
		return nullptr;
	}

	
	lock l(_database);
	_command->Connection = _database;

	// form command
	String ^ commandText = "SELECT * FROM computeroutput WHERE id=" + id;;

	_command->CommandText = commandText;
	System::Diagnostics::Trace::WriteLine(_command->CommandText);

	// execute command
	try
	{
		_reader = _command->ExecuteReader();
		if (_reader->HasRows)
		{

			retData = candidateFromReader(_reader);
		}
		_reader->Close();
	
	
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("DatabaseConnection::candidateWithID(): Executing command failed :( " + e);
		_reader->Close();
		return nullptr;
	}

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::candidateWithID() finished");
	return retData;
}

TargetRowData ^ DatabaseConnection::targetWithID(String ^ id)
{
	TargetRowData ^ retData = nullptr;
	System::Diagnostics::Trace::WriteLine("DatabaseConnection::candidateWithID() running");

	if (_database->State == System::Data::ConnectionState::Closed) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::candidateWithID(): database not open.");
		return nullptr;
	}

	
	lock l(_database);
	_command->Connection = _database;

	// form command
	String ^ commandText = "SELECT * FROM identifiedtargets WHERE id=" + id;;

	_command->CommandText = commandText;
	System::Diagnostics::Trace::WriteLine(_command->CommandText);

	// execute command
	try
	{
		_reader = _command->ExecuteReader();
		if (_reader->HasRows)
		{

			retData = targetFromReader(_reader);
		}
		_reader->Close();
	
	
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("DatabaseConnection::candidateWithID(): Executing command failed :( " + e);
		_reader->Close();
		return nullptr;
	}

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::candidateWithID() finished");
	return retData;
}

array<TargetRowData ^>^ DatabaseConnection::getAllTargets()
{
	int size = 20;

	array<TargetRowData ^>^ retArr = gcnew array<TargetRowData ^>(size);
	System::Diagnostics::Trace::WriteLine("DatabaseConnection::getAllTargets() running");

	if (_database->State == System::Data::ConnectionState::Closed) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::getAllTargets(): database not open.");
		return nullptr;
	}

	
	lock l(_database);
	_command->Connection = _database;

	// form command
	String ^ commandText = "SELECT * FROM identifiedtargets";

	_command->CommandText = commandText;
	System::Diagnostics::Trace::WriteLine(_command->CommandText);

	// execute command
	try
	{
		_reader = _command->ExecuteReader();
		if (_reader->HasRows)
		{
			int i = 0;
			while (_reader->Read())
			{
				
				retArr[i++] = targetFromReader(_reader);

				if (i >= size) {
					size += 10;
					Array::Resize(retArr, size);
				}

			}

			Array::Resize(retArr, i);

		}
		else {
			retArr = nullptr;
		}
		_reader->Close();
	
	
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("DatabaseConnection::getAllTargets(): Executing command failed :( " + e);
		_reader->Close();
		return nullptr;
	}

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::getAllTargets() finished");
	return retArr;
}


void DatabaseConnection::clearCandidatesTable()
{
	System::Diagnostics::Trace::WriteLine("DatabaseConnection::clearCandidatesTable() running");

	if (_database->State == System::Data::ConnectionState::Closed) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::clearCandidatesTable(): database not open.");
		return;
	}

	
	lock l(_database);
	_command->Connection = _database;

	// form command
	String ^ commandText = "DELETE FROM computeroutput";

	_command->CommandText = commandText;
	System::Diagnostics::Trace::WriteLine(_command->CommandText);
	// execute command
	try
	{
		_reader = _command->ExecuteReader();
		_reader->Close();
	
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("DatabaseConnection::clearCandidatesTable(): Executing command failed :( " + e);
		_reader->Close();
		return;
	}

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::clearCandidatesTable() finished");

}

void DatabaseConnection::clearTargetsTable()
{
	System::Diagnostics::Trace::WriteLine("DatabaseConnection::clearTargetsTable() running");

	if (_database->State == System::Data::ConnectionState::Closed) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::clearTargetsTable(): database not open.");
		return;
	}

	
	lock l(_database);
	_command->Connection = _database;

	// form command
	String ^ commandText = "DELETE FROM identifiedtargets";

	_command->CommandText = commandText;
	System::Diagnostics::Trace::WriteLine(_command->CommandText);
	// execute command
	try
	{
		_reader = _command->ExecuteReader();
		_reader->Close();
	
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("DatabaseConnection::clearTargetsTable(): Executing command failed :( " + e);
		_reader->Close();
		return;
	}

	System::Diagnostics::Trace::WriteLine("DatabaseConnection::clearTargetsTable() finished");

}

void DatabaseConnection::fillDatabase() 
{
	reset();

	for (int i = 0; i < 4; i++) {
		bool retVal = false;
		_command->Connection = _database;
			//inserting the array makes it mad.  FOR ARRAY USE '{0.0,1.1,2.2}'
		_command->CommandText = "INSERT INTO computeroutput (id, image, shape, shapecolor, letter, lettercolor, timestamp, centerx, centery, directionx, directiony) VALUES (" +
				i + ", '" + "2102403204320434036460350465" + "', " + "'square'" + ", " + "'blue'" + ", " +
				"'a'" + ", " + "'red'" + ", " + i*10.0 + ", " + 5 + ", " + 5 + ", " +
				100 + ", " + 100 + ")";
	
		//System::Diagnostics::Trace::WriteLine(_command->CommandText);
	
		try
		{
			OdbcDataReader ^ reader = _command->ExecuteReader();
			reader->Close();
			retVal = true;
		}
		catch( Exception ^ e)
		{
			System::Diagnostics::Trace::WriteLine("ERROR writing data: " + e);

		}
	}

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
	catch( Exception ^ e)
	{
		System::Diagnostics::Trace::WriteLine("ERROR in database.cpp: connect(): " + e);

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
		retVal[i] = (float)Convert::ToDouble( split[i] );
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