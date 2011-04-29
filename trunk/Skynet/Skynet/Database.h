#pragma once

#include "TelemetryStructures.h"
#include "DatabaseStructures.h"

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

		array<CandidateRowData ^>^ getAllCandidates();
		array<TargetRowData ^>^ getAllTargets();
		
		void clearCandidatesTable();
		void clearTargetsTable();

		void saveNewCandidate( CandidateRowData ^ data);
		void saveNewTarget( TargetRowData ^ data);

		void modifyCandidate( CandidateRowData ^ data);
		void modifyTarget( TargetRowData ^ data);

		void removeCandidate( String ^ id);
		void removeTarget( String ^id );

		
		CandidateRowData ^ candidateWithID(String ^ id);
		TargetRowData ^ targetWithID(String ^ id);

		///////////////////// no longer used /////////////////////
		bool insertData( TableName table, RowData ^ data );
		bool updateValue( TableName table, String ^ field, String ^ value, String ^ rowID );
		String ^ formatHomography( array<float> ^ input );
		bool move( TableName source, TableName dest, String ^ verifiedStatus );		
		bool moveProcessed( TableName source, TableName dest, String ^ processedStatus );

		array<float> ^ getHomography( TableName table, String ^ rowID );

	protected:
		String ^ getTableName( TableName table );
		
		CandidateRowData ^ candidateFromReader(OdbcDataReader ^ theReader);
		TargetRowData ^ targetFromReader(OdbcDataReader ^ theReader);

	private:
		OdbcConnection ^ _database;
		OdbcCommand ^ _command;
		OdbcDataReader ^ _reader;
		bool databaseOpen;


	};
}
