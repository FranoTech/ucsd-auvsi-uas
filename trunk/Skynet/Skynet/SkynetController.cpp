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


void SkynetController::saveCurrentImageAsTarget()
{
	// invoke saveCurrentImageAsTargetOnMainThread()
	
	Delegates::voidToVoid ^ saveDelegate = gcnew Delegates::voidToVoid(this, &SkynetController::saveCurrentImageAsTargetOnMainThread );

	((Form1 ^)form1View)->Invoke( saveDelegate );


}

void SkynetController::saveCurrentImageAsTargetOnMainThread()
{
	if (openGLView == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::saveCurrentImageAsTarget() ran, but openGLView == nullptr");
		return;
	}
	float *imageBuffer = new float[ theOpenGL->frameW * theOpenGL->frameH * sizeof(float) * 4 / 2];
	COpenGL ^theOpenGL = (COpenGL ^)openGLView;

	{
		lock l(theOpenGL);
		memcpy(imageBuffer, theOpenGL->buffer, theOpenGL->frameW * theOpenGL->frameH * sizeof(float) * 4 / 2); // 4 is numChannels, 2 is because deinterlacing cuts height in half	
	}

	saveTarget(imageBuffer, theOpenGL->frameW, theOpenGL->frameH/2, 4, 0, 0, theWatcher->stateOfCurrentImage());

	((Form1 ^)form1View)->printToConsole("Saved image to database", Color::Orange);
}

void SkynetController::saveTarget(float * data, int width, int height, int numChannels, int originX, int originY, PlaneState ^ stateOfPlane)
{
	if (theDatabase == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::saveTarget() ran, but theDatabase == nullptr");
		return;
	}


	if (width < 0 || height < 0 || originX < 0 || originY < 0) {
		System::Diagnostics::Trace::WriteLine("SkynetController::saveTarget() ran, but (width < 0 || height < 0 || originX < 0 || originY < 0)");
		return;
	}

	theDatabase->saveNewCandidate(data, width, height, numChannels, originX, originY, stateOfPlane);

	delete data;
	// for testing:
	//theDatabase->displayLastImageInCandidate();
}