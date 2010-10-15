#include "StdAfx.h"
#include "Form1.h"
#include "TargetDialog.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace Skynet;

TargetDialog::TargetDialog( double centerLat, double centerLon, double latMap, double lonMap, array<float> ^ homo, Object ^ parent )
{
	_parent = parent;
	_centerLat = centerLat;
	_centerLon = centerLon;
	_latMap = latMap;
	_lonMap = lonMap;
	_homography = homo;

	_markLat = false;
	_markHeading = false;
	InitializeComponent();
	//
	//TODO: Add the constructor code here
	//
}

System::Void 
TargetDialog::okButton_Click(System::Object^  sender, System::EventArgs^  e) 
{
	 // enter the data in the callbacks
	((Skynet::Form1 ^)_parent)->imageDialogCallback( _rowID, _targetID, _latitude, _longitude, _heading );
	this->Close();
}