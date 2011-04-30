#include "StdAfx.h"
#include "Form1.h"
#include "TargetDialog.h"

#include "SkynetController.h"
#include "DatabaseStructures.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace Database;
using namespace Skynet;

#define SHAPE_COLOR		textBox1
#define SHAPE			textBox2
#define LETTER_COLOR	textBox3
#define LETTER			textBox4


TargetDialog::TargetDialog( Object ^ parent, Object ^ newAppController)
{
	_parent = parent;
	appController = newAppController;
	_markLat = false;
	_markHeading = false;
	
                //      _centerLat = centerLat;
                //      _centerLon = centerLon;
                //      _latMap = latMap;
                //      _lonMap = lonMap;
	centerX = -1.0f;
	centerY = -1.0f;
	
	topOfTargetX = -1.0f;
	topOfTargetY = -1.0f;
	
	open = false;
	imageOpen = false;
	_targetImage = nullptr;
	candidate = nullptr;
	target = nullptr;
	data = nullptr;

	mode = DialogEditingCandidate;

	InitializeComponent();

}

void TargetDialog::showDialogForData(Database::CandidateRowData ^ theData)
{
	mode = DialogEditingCandidate;
	data = gcnew DialogEditingData(theData);
	candidate = theData;
	target = nullptr;
	open = true;
	_markLat = false;
	_markHeading = false;
	
}

void TargetDialog::showDialogForData(Database::TargetRowData ^ theData)
{
	mode = DialogEditingUnverified;

	data = gcnew DialogEditingData(theData);
	target = theData;
	candidate = nullptr;
	open = true;
	_markLat = false;
	_markHeading = false;

}

void TargetDialog::reloadData()
{
	// reload text fields
	if (data->shape->Equals("Unknown"))
		SHAPE->Text = "";
	else 
		SHAPE->Text = data->shape;

	if (data->shapeColor->Equals("Unknown"))
		SHAPE_COLOR->Text = "";
	else 
		SHAPE_COLOR->Text = data->shapeColor;

	if (data->letter->Equals("Unknown"))
		LETTER->Text = "";
	else 
		LETTER->Text = data->letter;

	if (data->letterColor->Equals("Unknown"))
		LETTER_COLOR->Text = "";
	else 
		LETTER_COLOR->Text = data->letterColor;

	// change name of ok button
	switch (mode) {
		case DialogEditingCandidate:
			okButton->Text = "Move to Unverified";
			break;
		case DialogEditingUnverified:
			okButton->Text = "Save Changes";
			break;
		default:
			okButton->Text = "Shit, bro";
			break;
	}

	centerX = (float)data->targetX;
	centerY = (float)data->targetY;

	topOfTargetX = (float)data->topOfTargetY;
	topOfTargetY = (float)data->topOfTargetY;

	setImage();

}

void TargetDialog::setImage()
{
	try
	{
		_targetImage = Image::FromFile( HTTP_SERVER_TARGET_PATH + data->imageName->Remove(0, 8) );
		imageBox->Image = _targetImage;
	}
	catch( Exception ^ )
	{

	}

}
void TargetDialog::getDataFromUI()
{
	// reload text fields
	if (!SHAPE->Text->Equals(""))
		data->shape = SHAPE->Text;

	if (!SHAPE_COLOR->Text->Equals(""))
		data->shapeColor = SHAPE_COLOR->Text;

	if (!LETTER->Text->Equals(""))
		data->letter = LETTER->Text;
	
	if (!LETTER_COLOR->Text->Equals(""))
		data->letterColor = LETTER_COLOR->Text;
	


	data->targetX = (int)centerX;
	data->targetY = (int)centerY;

	data->topOfTargetX = (int)topOfTargetX;
	data->topOfTargetY = (int)topOfTargetY;

}


System::Void 
TargetDialog::okButton_Click(System::Object^  sender, System::EventArgs^  e) 
{
	if (mode == DialogEditingCandidate) {
		TargetRowData ^newData = gcnew TargetRowData(candidate);
		newData->updateFrom(data);

		((SkynetController ^)appController)->removeCandidate(candidate);
		((SkynetController ^)appController)->addTarget(newData);

	}

	else if (mode == DialogEditingUnverified) {
		target->updateFrom(data);

		((SkynetController ^)appController)->modifyTarget(target);
	}

	open = false;
	_markLat = false;
	_markHeading = false;
	this->Close();
}