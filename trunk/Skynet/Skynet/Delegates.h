#pragma once
#include "Comport.h"
#include "Database.h"

namespace Delegates
{
	delegate void comportDownstreamToVoid( Communications::ComportDownstream * data );
	delegate void voidToVoid( void );
	delegate void twointThreedoubleToVoid( int a, int b, double c, double d, double e );
	delegate void rowDataToVoid( Database::RowData ^ data );
	delegate void dataGridViewRowToVoid( DataGridViewRow ^ row );
}