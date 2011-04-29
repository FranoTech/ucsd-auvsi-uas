#pragma once

#include "TelemetryStructures.h"
#include "PlaneWatcher.h"
#include "Database.h"
#include "SkynetControllerInterface.h"

#define HTTP_SERVER_TARGET_PATH "C:\\xampp\\htdocs\\targets\\"

using namespace System;
using namespace Communications;

namespace Skynet
{
	public ref class SkynetController : public SkynetControllerInterface
	{
	public:
		SkynetController(Object ^ mainView);
		~SkynetController(void);

		void setCameraView(Object ^ cameraView) { openGLView = cameraView; }
		void setDatabase(Database::DatabaseConnection ^ newDatabase) { theDatabase = newDatabase; }
		void setPlaneWatcher(PlaneWatcher ^ newWatcher) { theWatcher = newWatcher; }

		void loadAllTablesFromDisk();
		void loadCandidateTableFromDisk();
		void loadTargetTableFromDisk();
		void clearAllTables();

		virtual void saveCurrentFrameAsCandidate();	 // call this anywhere, any thread, any time
		void saveCandidate(float * data, int width, int height, int numChannels, int originX, int originY, PlaneState ^ stateOfPlane);
		void saveCurrentFrameAsCandidateOnMainThread(); // call this only from main thread
		void addCandidateToGUITable(Object ^ theObject);
		void addTargetToGUITable(Object ^ theObject);

		void addCandidate(Database::CandidateRowData ^ data);
		void addTarget(Database::TargetRowData ^ data);
		void addVerifiedTarget(Database::TargetRowData ^ data);
		
		
		void modifyCandidate(Database::CandidateRowData ^ data);
		void modifyTarget(Database::TargetRowData ^ data);

		void removeCandidate(Database::CandidateRowData ^ data);
		void removeCandidate(String ^ id);
		void removeTarget(Database::TargetRowData ^ data);
		void removeTarget(String ^ id);

		Database::CandidateRowData ^ candidateWithID(String ^ id);
		Database::TargetRowData ^ targetWithID(String ^ id);
		Database::VerifiedTargetRowData ^ verifiedTargetWithID(String ^ id);


	protected:
		Object ^ form1View;
		Object ^ openGLView;
		Database::DatabaseConnection ^ theDatabase;
		PlaneWatcher ^ theWatcher;

	private:
	};
}
