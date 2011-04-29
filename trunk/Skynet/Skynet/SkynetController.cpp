#include "StdAfx.h"

#include "SkynetController.h"

#include "OpenGLForm.h"
#include "Form1.h"
#include "Delegates.h"

using namespace System;
using namespace System::Data::Odbc;
using namespace Skynet;
using namespace OpenGLForm;
using namespace Database;
using namespace msclr;


SkynetController::SkynetController(Object ^ mainView)
{
	openGLView = nullptr;
	theDatabase = nullptr;
	form1View = mainView;
}

SkynetController::~SkynetController()
{
	form1View = nullptr;
	openGLView = nullptr;
	theDatabase = nullptr;
}


void SkynetController::saveCurrentFrameAsCandidate()
{
	// invoke saveCurrentFrameAsCandidateOnMainThread()
	if (openGLView == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::saveCurrentFrameAsCandidate() ran, but openGLView == nullptr");
		return;
	}

	COpenGL ^theOpenGL = (COpenGL ^)openGLView;

	// TODO:  get plane information
	int width = theOpenGL->frameW; 
	int height = theOpenGL->frameH;
	int numChannels = 4;
	int originX = 0;
	int originY = 0;
	PlaneState ^ stateOfPlane = theWatcher->stateOfCurrentImage();

	// save current image to a file
	String ^ pathbase = HTTP_SERVER_TARGET_PATH;
	String ^ filename = "img_" + DateTime::Now.ToString("o")->Replace(":", "-") + ".jpg";
	theOpenGL->saveImage( pathbase + filename );

	// insert data into database
	CandidateRowData ^ imageData = gcnew CandidateRowData(stateOfPlane);
	imageData->imageName = "targets/" + filename;
	imageData->dataWidth = width;
	imageData->dataHeight = height;
	imageData->dataNumChannels = numChannels;
	imageData->originX = originX;
	imageData->originY = originY;

	Delegates::candidateRowDataToVoid ^ saveDelegate = gcnew Delegates::candidateRowDataToVoid(this, &SkynetController::addCandidate );

	try {
		((Form1 ^)form1View)->Invoke( saveDelegate, gcnew array<Object ^>{imageData} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::saveCurrentFrameAsCandidate(): Failed to add image to target - " + e);
	}

}

void SkynetController::saveCurrentFrameAsCandidateOnMainThread()
{
	if (openGLView == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::saveCurrentFrameAsCandidate() ran, but openGLView == nullptr");
		return;
	}

	COpenGL ^theOpenGL = (COpenGL ^)openGLView;

	float *imageBuffer = new float[ theOpenGL->frameW * theOpenGL->frameH * sizeof(float) * 4 / 2];
	{
		lock l(theOpenGL);
		memcpy(imageBuffer, theOpenGL->buffer, theOpenGL->frameW * theOpenGL->frameH * sizeof(float) * 4 / 2); // 4 is numChannels, 2 is because deinterlacing cuts height in half	
	}

	saveCandidate(imageBuffer, theOpenGL->frameW, theOpenGL->frameH/2, 4, 0, 0, theWatcher->stateOfCurrentImage());

	((Form1 ^)form1View)->printToConsole("Saved image to database", Color::Orange);
}

// theObject is actually an array of one CandidateRowData object
void SkynetController::addCandidateToGUITable(Object ^ theObject)
{
	array<CandidateRowData ^>^ theArr = (array<CandidateRowData ^> ^)theObject;
	CandidateRowData ^ data = (CandidateRowData ^)(theArr[0]);
	Delegates::candidateRowDataToVoid ^ blahdelegate = gcnew Delegates::candidateRowDataToVoid((Form1 ^)form1View, &Form1::insertCandidateData );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{data} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::addCandidateToGUITable(): Failed to add image to GUI Table - " + e);
	}
}


void SkynetController::addTargetToGUITable(Object ^ theObject)
{	
	array<TargetRowData ^>^ theArr = (array<TargetRowData ^> ^)theObject;
	TargetRowData ^ data = (TargetRowData ^)(theArr[0]);
	Delegates::targetRowDataToVoid ^ blahdelegate = gcnew Delegates::targetRowDataToVoid((Form1 ^)form1View, &Form1::insertTargetData );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{data} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::targetRowDataToVoid(): Failed to add image to GUI Table - " + e);
	}

}

void SkynetController::loadAllTablesFromDisk()
{
	loadCandidateTableFromDisk();
	loadTargetTableFromDisk();
}

void SkynetController::loadCandidateTableFromDisk()
{
	array<CandidateRowData ^>^ theRows = theDatabase->getAllCandidates();
	
	if (theRows == nullptr)
		return;

	for (int i = 0; i < theRows->Length; i++)
		((Form1 ^)form1View)->insertCandidateData(theRows[i]);
}

void SkynetController::loadTargetTableFromDisk()
{
	array<TargetRowData ^>^ theRows = theDatabase->getAllTargets();

	if (theRows == nullptr)
		return;

	for (int i = 0; i < theRows->Length; i++)
		((Form1 ^)form1View)->insertTargetData(theRows[i]);
}

void SkynetController::clearAllTables()
{
	theDatabase->clearCandidatesTable();
	theDatabase->clearTargetsTable();
}


void SkynetController::addCandidate(CandidateRowData ^ data)
{	
	if (theDatabase == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::addCandidate() ran, but theDatabase == nullptr");
		return;
	}

	if (data == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::addCandidate() ran, but data == nullptr");
		return;
	}
	try {
		theDatabase->saveNewCandidate(data);

		TimerCallback^ tcb =
           gcnew TimerCallback(this, &SkynetController::addCandidateToGUITable);
		Threading::Timer^ addTargetTimer = gcnew Threading::Timer(tcb,  gcnew array<CandidateRowData ^>{data}, 250, Timeout::Infinite);

		//stateTimer->Start();
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::saveCandidate(): Failed to save imageS - " + e);
	}
}

void SkynetController::addTarget(Database::TargetRowData ^ data)
{	
	if (theDatabase == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::addTarget() ran, but theDatabase == nullptr");
		return;
	}

	if (data == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::addTarget() ran, but data == nullptr");
		return;
	}
	try {
		theDatabase->saveNewTarget(data);

		TimerCallback^ tcb =
           gcnew TimerCallback(this, &SkynetController::addTargetToGUITable);
		Threading::Timer^ addTargetTimer = gcnew Threading::Timer(tcb,  gcnew array<TargetRowData ^>{data}, 250, Timeout::Infinite);

		//stateTimer->Start();
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::addTarget(): Failed to save imageS - " + e);
	}

}

void SkynetController::addVerifiedTarget(Database::TargetRowData ^ data) // not yet
{
	System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::addVerifiedTarget(): Not implemented");
}

void SkynetController::removeCandidate(Database::CandidateRowData ^ data)
{
	removeCandidate("" + data->id);
}

void SkynetController::removeCandidate(String ^ id)
{	
	theDatabase->removeCandidate(id);

	Delegates::candidateRowDataToVoid ^ blahdelegate = gcnew Delegates::candidateRowDataToVoid((Form1 ^)form1View, &Form1::removeCandidateFromTable );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{id} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::removeCandidate(): Failed to remove candidate from GUI table - " + e);
	}

}

void SkynetController::removeTarget(Database::TargetRowData ^ data)
{
	removeTarget("" + data->id);
}

void SkynetController::removeTarget(String ^ id)
{
	theDatabase->removeCandidate(id);

	Delegates::targetRowDataToVoid ^ blahdelegate = gcnew Delegates::targetRowDataToVoid((Form1 ^)form1View, &Form1::removeTargetFromTable );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{id} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::removeTarget(): Failed to remove target from GUI table - " + e);
	}
}


void SkynetController::modifyCandidate(Database::CandidateRowData ^ data)
{
	theDatabase->modifyCandidate(data);

	Delegates::candidateRowDataToVoid ^ blahdelegate = gcnew Delegates::candidateRowDataToVoid((Form1 ^)form1View, &Form1::modifyCandidateInTable );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{data} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::modifyCandidate(): Failed to modify candidate in GUI table - " + e);
	}
}

void SkynetController::modifyTarget(Database::TargetRowData ^ data)
{
	theDatabase->modifyTarget(data);

	Delegates::targetRowDataToVoid ^ blahdelegate = gcnew Delegates::targetRowDataToVoid((Form1 ^)form1View, &Form1::modifyTargetInTable );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{data} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::modifyTarget(): Failed to modify target in GUI table - " + e);
	}
}


Database::CandidateRowData ^ SkynetController::candidateWithID(String ^ id)
{
	return theDatabase->candidateWithID(id);
}

Database::TargetRowData ^ SkynetController::targetWithID(String ^ id)
{
	return theDatabase->targetWithID(id);
}

Database::VerifiedTargetRowData ^ SkynetController::verifiedTargetWithID(String ^ id) // not yet
{
	System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::verifiedTargetWithID(): not implemented");

	return nullptr;
}


void SkynetController::saveCandidate(float * data, int width, int height, int numChannels, int originX, int originY, PlaneState ^ stateOfPlane)
{
	System::Diagnostics::Trace::WriteLine("ERROR SkynetController::saveCandidate() not implemented");
	return;
	/*
	if (theDatabase == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::saveCandidate() ran, but theDatabase == nullptr");
		return;
	}

	if (width < 0 || height < 0 || originX < 0 || originY < 0) {
		System::Diagnostics::Trace::WriteLine("SkynetController::saveCandidate() ran, but (width < 0 || height < 0 || originX < 0 || originY < 0)");
		return;
	}


	
	// TODO: get plane information
	// save current image to a file
	// insert data into database

try {
		theDatabase->saveNewCandidate(data, width, height, numChannels, originX, originY, stateOfPlane);
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::saveCandidate(): Failed to save imageS - " + e);
	}

	delete data;*/
	// for testing:
	//theDatabase->displayLastImageInCandidate();
}