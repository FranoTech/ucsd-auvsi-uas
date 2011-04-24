#pragma once

#include "AutopilotComport.h"
#include "PlaneWatcher.h"
#include "Database.h"

using namespace System;
using namespace Communications;

namespace Skynet
{
	public ref class SkynetController
	{
	public:
		SkynetController(Object ^ mainView);
		~SkynetController(void);

		void setCameraView(Object ^ cameraView) { openGLView = cameraView; }
		void setDatabase(Database::DatabaseConnection ^ newDatabase) { theDatabase = newDatabase; }
		void setPlaneWatcher(PlaneWatcher ^ newWatcher) { theWatcher = newWatcher; }

		void saveCurrentImageAsTarget();
		void saveCurrentImageAsTargetOnMainThread();
		void saveTarget(float * data, int width, int height, int numChannels, int originX, int originY, PlaneState ^ stateOfPlane);
		

	protected:
		Object ^ form1View;
		Object ^ openGLView;
		Database::DatabaseConnection ^ theDatabase;
		PlaneWatcher ^ theWatcher;

	private:

	};
}
