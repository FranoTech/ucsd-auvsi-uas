#pragma once

#include "OpenGLForm.h"
#include "DecklinkCallback.h"
#include "Comport.h"
#include "Joystick.h"
#include "Auvsi_Saliency.h"
#include "Database.h"
#include "Delegates.h"
#include "MapControl.h"

#ifndef OPENCV_DISABLED
#include "GeoReference.h"
#endif

#include "TargetDialog.h"
#include "HUDControl.h"
#include "Saliency.h"
#include "OCR.h"
//#include "VideoSimulator.h"
#include "SimHandler.h"
#include "Comms.h"
#include "PlaneWatcher.h"
#include "AutopilotComport.h"
#include "SkynetController.h"

#include <math.h>

#define GREEN_IMAGE_PATH	"D:\\Skynet Files\\greenled.png"
#define RED_IMAGE_PATH	"D:\\Skynet Files\\redled.png"
#define YELLOW_IMAGE_PATH	"D:\\Skynet Files\\yellowled.png"

	// row indexes for comport data
	const int A_ALT			= 1;
	const int A_LAT			= 2;
	const int A_LON			= 3;
	const int A_HEAD		= 4;
	const int A_ROLL		= 5;
	const int A_PITCH		= 6;
	const int G_ROLL		= 8;
	const int G_PITCH		= 9;
	const int V_ZOOM		= 11;
	const int V_FORMAT		= 12;


	

namespace Skynet {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;
	using namespace System::Threading;
	using namespace System::IO;
	using namespace System::Text;

	

	const int COL_ID = 0;
	const int COL_THUMB = 1;
	const int COL_LAT = 2;
	const int COL_LON = 3;
	const int COL_HEAD = 4;
	const int COL_LETTER = 5;
	const int COL_SHAPE = 6;
	const int COL_FG = 7;
	const int COL_BG = 8;
	const int COL_PROC = 9;
	const int COL_VERIFIED = 10;

	//[DllImport("AUVSI_SALIENCY", CharSet=CharSet::Ansi)]
	//extern "C" void Auvsi_Saliency();	

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
		///
		/// Stuff added manually
		///

	public:
		Delegates::voidToVoid ^ comportErrorDelegate;
		Delegates::voidToVoid ^ comportEstablishDelegate;
		Delegates::twointThreedoubleToVoid ^ imageDialogDelegate;
		Delegates::rowDataToVoid ^ saliencyAddTarget;
		Delegates::dataGridViewRowToVoid ^ ocrDelegate;
		Delegates::voidToVoid ^ saveImageDelegate;
	protected:
		OpenGLForm::COpenGL ^ openGLView;  	// video viewport
		Decklink::Callback * callback;		// decklink callback		

		SkynetController ^ appController;

		String ^ fileExtension;
		String ^ defaultMapCache;

		Communications::Comms ^ theComms;
		bool isconnected;
		bool isConnecting;
		//OpenGLForm::COpenGL ^ openGLView2; // airplane virtual cockpit viewport
		bool vidOptFolderDialogOpen;		// For video options directory
		Joystick  ^ m_joystick; 			// joystick init
		Database::DatabaseConnection ^ db;

		Thread ^ comReadThread;
		int imageNumber; // for saving images

		// Temporary logging stuff
		FileStream ^ logFile;

		// Video saving stuff
		int splitLength; // in seconds
		bool recording;
		DateTime recordStart;
		//String ^ fileExtensionVideo;

		//Database ID
		int incrId;

		//Map point
		Point mouseGPS;

		// Image details
		TargetDialog ^ imageDialog;

		Simulator::VideoSimulator ^ theVideoSimulator;
		Simulator::SimHandler ^ theSimHandler;

		Communications::PlaneWatcher ^ thePlaneWatcher;


		Image ^redImage;
		Image ^yellowImage;
		Image ^greenImage;

	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TabPage^  tabPage2;


	private: System::Windows::Forms::Button^  startRecordButton;

	private: System::Windows::Forms::Button^  vidOptChangeDirButton;

	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::ComboBox^  encodingComboBox;

	private: System::Windows::Forms::TextBox^  splitLengthTextBox;

	private: System::Windows::Forms::TextBox^  vidOptOutputDirText;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::CheckedListBox^  checkedListBox1;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;

	private: System::Windows::Forms::FolderBrowserDialog^  vidOptChangeDirBrowserDialog;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Property;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Value;
	private: System::Windows::Forms::Timer^  joystickTimer;

	private: System::Windows::Forms::TabControl^  metadataTabControl;
	private: System::Windows::Forms::TabPage^  tabPage3;
	private: System::Windows::Forms::DataGridView^  dataGridView1;
	private: System::Windows::Forms::TabPage^  tabPage4;
	private: System::Windows::Forms::TabPage^  tabPage5;
	private: System::Windows::Forms::DataGridView^  dataGridView2;
	private: System::Windows::Forms::DataGridView^  dataGridView3;

	private: System::Windows::Forms::CheckBox^  checkBox1;
	private: System::Windows::Forms::Timer^  videoSaveTimer;
	private: MapControl ^ mapView;
	private: System::Windows::Forms::Panel^  mapPanel;
	private: System::Windows::Forms::ToolStripMenuItem^  mapToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  providerToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  mapnikToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  osmarenderToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cyclemapToolStripMenuItem;
	private: System::Windows::Forms::Label^  mapCenterGPSLabel;

	private: System::Windows::Forms::Label^  mapMouseGPSLabel;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;


private: System::Windows::Forms::ToolStripMenuItem^  exportDataToolStripMenuItem;
private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator3;
private: System::Windows::Forms::ToolStripMenuItem^  databaseToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  resetToolStripMenuItem;
private: System::Windows::Forms::Timer^  mapUpdateTimer;
private: System::Windows::Forms::ToolStripMenuItem^  lockToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  downloadToolStripMenuItem;
private: System::Windows::Forms::ContextMenuStrip^  mapMenuStrip;
private: System::Windows::Forms::ToolStripMenuItem^  mapLookGPSToolStripMenuItem;

































private: System::Windows::Forms::ToolStripMenuItem^  simulatorToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  choosePathToolStripMenuItem;

private: System::Windows::Forms::ToolStripMenuItem^  pauseToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  stopToolStripMenuItem;
private: System::Windows::Forms::OpenFileDialog^  simReadVidDialog;
private: System::Windows::Forms::Button^  button1;
private: System::Windows::Forms::Button^  button2;
private: System::Windows::Forms::Button^  button3;
private: System::Windows::Forms::Button^  button4;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::PictureBox^  pictureBox1;
private: System::Windows::Forms::PictureBox^  pictureBox2;
private: System::Windows::Forms::PictureBox^  pictureBox3;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::Label^  label16;
private: System::Windows::Forms::Panel^  panel1;
private: System::Diagnostics::Process^  process1;
private: System::Windows::Forms::Panel^  gimbalHUDPanel;
private: HUDControl^ gimbalHUDView;






















private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_IDColumn;
private: System::Windows::Forms::DataGridViewImageColumn^  confirmed_ThumbnailColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_LatitudeColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_LongitudeColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_HeadingColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_LetterColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_LetterColor;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_Shape;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_ShapeColor;
private: System::Windows::Forms::DataGridViewCheckBoxColumn^  confirmed_ProcessedColumn;
private: System::Windows::Forms::DataGridViewCheckBoxColumn^  confirmed_VerifiedColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  candidate_IDColumn;
private: System::Windows::Forms::DataGridViewImageColumn^  candidate_ThumbnailColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  candidate_LatitudeColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  canidate_LongitudeColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  candidate_HeadingColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  candidate_LetterColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  candidate_LetterColor;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  candidate_Shape;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  candidate_ShapeColor;
private: System::Windows::Forms::DataGridViewCheckBoxColumn^  candidate_ProcessedColumn;
private: System::Windows::Forms::DataGridViewCheckBoxColumn^  candidate_VerifiedColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_IDColumn;
private: System::Windows::Forms::DataGridViewImageColumn^  unverified_ThumbnailColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_LatitudeColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_LongitudeColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_HeadingColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_CenterPixel;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverfied_LetterColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_LetterColor;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_Shape;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_ShapeColor;
private: System::Windows::Forms::DataGridViewCheckBoxColumn^  unverified_ProcessedColumn;
private: System::Windows::Forms::DataGridViewCheckBoxColumn^  unverified_VerifiedColumn;




























	private: System::Windows::Forms::Label^  label7;
			 

	public:
		Form1(void)
		{
			InitializeComponent();

			// Debug 
			
			//GeoReference::computeHomography( 0, 0, 400, 0, Math::PI/4, 0, 0, -1.5708, 1 );

			appController = gcnew SkynetController(this);

			// Set up Map
			this->mapView = gcnew MapControl();
			this->mapView->Location = this->mapPanel->Location;
			this->mapView->Name = L"mapView";
			this->mapView->Size = this->mapPanel->Size;			
			this->mapView->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::mapView_MouseMove);
			this->mapView->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::mapView_MouseDown);

			// Set up HUD
			this->gimbalHUDView = gcnew HUDControl();
			this->gimbalHUDView->Location = this->gimbalHUDPanel->Location;
			this->gimbalHUDView->Name = L"gimbalHUDView";
			this->gimbalHUDView->Size = this->gimbalHUDPanel->Size;
			this->gimbalHUDView->Refresh();
			this->gimbalHUDView->BackColor = System::Drawing::Color::Transparent;
			
			//this->mapView->ContextMenuStrip = mapMenuStrip;
			
			this->Controls->Add(this->mapView);
			this->Controls->Add(this->gimbalHUDView);

			//hide map
			this->mapPanel->Location = System::Drawing::Point(0, 0);
			this->mapPanel->Name = L"mapPanel";
			this->mapPanel->Size = System::Drawing::Size(0, 0);

			//hide HUD
			this->gimbalHUDPanel->Location = System::Drawing::Point(0, 0);
			this->gimbalHUDPanel->Name = L"gimbalHUDPanel";
			this->gimbalHUDPanel->Size = System::Drawing::Size(0, 0);

			this->ResumeLayout(false);
			this->PerformLayout();

			// Set default map provider
			defaultMapCache = "D:\\Skynet Files\\Map Cache";			
			mapView->SetCacheDirectory( defaultMapCache + "\\Mapnik", false );
			mapView->SetTileProvider( Mapnik );
			mapnikToolStripMenuItem->Checked = true;
			osmarenderToolStripMenuItem->Checked = false;
			cyclemapToolStripMenuItem->Checked = false;
			// end set up Map
			
			thePlaneWatcher = gcnew Communications::PlaneWatcher(this);
			appController->setPlaneWatcher( thePlaneWatcher );

			// Set up openGL						
			// old todo: in order to have two openGL windows, you really only get one OpenGL scene and give each one a different viewport
			wglMakeCurrent( NULL, NULL ); // this causees a delay which allows wglCreateContext to work properly
			//openGLView2 = gcnew OpenGLForm::COpenGL( this->openGLPanel2, this->openGLPanel2->Width, this->openGLPanel2->Height );
			openGLView = gcnew OpenGLForm::COpenGL( this->openGLPanel, this->openGLPanel->Width, this->openGLPanel->Height, this, thePlaneWatcher );
			appController->setCameraView( openGLView );


			// Set up DeckLink
			callback = new Decklink::Callback( openGLView );
			
			theVideoSimulator = gcnew Simulator::VideoSimulator( openGLView );

			// Comport Stuff
			//theComport = gcnew Communications::Comport( this );
			//comPortStripComboBox->Items->AddRange( theComport->getPortNames() );
			theSimHandler = gcnew Simulator::SimHandler(theVideoSimulator, callback, openGLView);

			//set up Joystick
			m_joystick = gcnew Joystick( this );
			m_joystick->init( NULL );
			m_joystick->theWatcher = thePlaneWatcher;
			m_joystick->setDelegate( appController );

			//set up database
			db = gcnew Database::DatabaseConnection();
			bool connec = db->connect();
			incrId = 0;
			appController->setDatabase( db );

			// Video options
			vidOptFolderDialogOpen = false;
			recording = false;
			//fileExtensionVideo = ".avi";

			// Image options
			imageNumber = 0;
			fileExtension = ".jpg";

			// Logging			
			DateTime timeTemp = DateTime::Now;
			logFile = File::Create( "D:\\Skynet Files\\Flight Logs\\" + timeTemp.ToString("o")->Replace(":", "-") + ".txt" );

			// Set up metadata Table
			this->metadataTable->Rows->Add("Airplane Data", "---");
			this->metadataTable->Rows->Add("    Altitude");
			this->metadataTable->Rows->Add("    Latitude");
			this->metadataTable->Rows->Add("    Longitude");
			this->metadataTable->Rows->Add("    Heading");
			this->metadataTable->Rows->Add("    Roll");
			this->metadataTable->Rows->Add("    Pitch");
			this->metadataTable->Rows->Add("Gimbal Data", "---");
			this->metadataTable->Rows->Add("    Roll");
			this->metadataTable->Rows->Add("    Pitch");
			this->metadataTable->Rows->Add("Video Data", "---");
			this->metadataTable->Rows->Add("    Zoom");
		//	this->metadataTable->Rows->Add("    Format");
		//	this->metadataTable[1, V_FORMAT]->Value = "NTSC";

			// Delegates
			comportErrorDelegate = gcnew Delegates::voidToVoid( this, &Form1::noComportData );
			comportEstablishDelegate = gcnew Delegates::voidToVoid( this, &Form1::comportEstablish );
			saveImageDelegate = gcnew Delegates::voidToVoid( this, &Form1::saveImage );
			imageDialogDelegate = gcnew Delegates::twointThreedoubleToVoid( this, &Form1::imageDialogCallback );


			// Delegates::rowDataToVoid ^ saliencyAddTarget = gcnew Delegates::rowDataToVoid( this, &Form1::addRawTarget );
			saliencyAddTarget = gcnew Delegates::rowDataToVoid( this, &Form1::addRawTarget );	
			// Delegates::rowDataToVoid == delegate void rowDataToVoid( Database::RowData ^ data );
			// delegate void rowDataToVoid( Database::RowData ^ data );


			ocrDelegate = gcnew Delegates::dataGridViewRowToVoid( this, &Form1::ocrUpdateData );

			theComms = gcnew Communications::Comms(theSimHandler->theTelSimulator, this);
			isconnected = false;
			isConnecting = false;

			m_joystick->comm = theComms;
			theSimHandler->theComms = theComms;
			((TelemetrySimulator ^)theSimHandler->theTelSimulator)->theAutopilotComport = theComms->autopilot;

			redImage = Image::FromFile( RED_IMAGE_PATH );
			yellowImage = Image::FromFile( YELLOW_IMAGE_PATH );
			greenImage = Image::FromFile( GREEN_IMAGE_PATH );

			pictureBox1->SizeMode = PictureBoxSizeMode::StretchImage;
			pictureBox2->SizeMode = PictureBoxSizeMode::StretchImage;
			pictureBox3->SizeMode = PictureBoxSizeMode::StretchImage;
			pictureBox1->Image = dynamic_cast<Image^>(redImage);
			pictureBox2->Image = dynamic_cast<Image^>(redImage);
			pictureBox3->Image = dynamic_cast<Image^>(redImage);

			
			//theComms->connectAll();
			//theComms->gotoLatLon(534.0f, 2878.0f);

			consoleMessage("... Skynet online", Color::Orange);
			
			gimbalHUDView->BringToFront();

			appController->loadAllTablesFromDisk();
		}

	public:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if ( logFile )
            	delete (IDisposable^)logFile;

			if( callback )
			{
				delete callback;
				System::Diagnostics::Trace::WriteLine("Callback");
			}
			if (theComms)
			{
				//if( comReadThread )
					//comReadThread->Abort();
				//delete theComms;
				System::Diagnostics::Trace::WriteLine("Comport Deleted via Garbage Collecting");
			}
			if( m_joystick )
			{
				delete m_joystick;
				System::Diagnostics::Trace::WriteLine("Joystick Deleted");
			}

			if (components)
			{
				delete components;
				System::Diagnostics::Trace::WriteLine("Components Deleted");
			}

		}
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::Timer^  openGLTimer;
	private: System::Windows::Forms::Panel^  openGLPanel;
	private: System::Windows::Forms::RichTextBox^  errorLogTextBox;
	public: System::Windows::Forms::DataGridView^  metadataTable;


	private: System::Windows::Forms::ToolStripMenuItem^  serialCommunicationsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  connectToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;

	
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
	private: System::Windows::Forms::ToolStripMenuItem^  disconnectToolStripMenuItem;
	private: System::Windows::Forms::ToolStripComboBox^  comPortStripComboBox;
	private: System::Windows::Forms::ToolStripComboBox^  baudRateStripComboBox;

	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle7 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle8 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle9 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exportDataToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->serialCommunicationsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->connectToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->comPortStripComboBox = (gcnew System::Windows::Forms::ToolStripComboBox());
			this->baudRateStripComboBox = (gcnew System::Windows::Forms::ToolStripComboBox());
			this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->disconnectToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mapToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->providerToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mapnikToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->osmarenderToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cyclemapToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->lockToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->downloadToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->databaseToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->resetToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->simulatorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->choosePathToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->pauseToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->stopToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openGLTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->openGLPanel = (gcnew System::Windows::Forms::Panel());
			this->gimbalHUDPanel = (gcnew System::Windows::Forms::Panel());
			this->errorLogTextBox = (gcnew System::Windows::Forms::RichTextBox());
			this->metadataTable = (gcnew System::Windows::Forms::DataGridView());
			this->Property = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Value = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->checkedListBox1 = (gcnew System::Windows::Forms::CheckedListBox());
			this->vidOptChangeDirButton = (gcnew System::Windows::Forms::Button());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->encodingComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->splitLengthTextBox = (gcnew System::Windows::Forms::TextBox());
			this->vidOptOutputDirText = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->startRecordButton = (gcnew System::Windows::Forms::Button());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->vidOptChangeDirBrowserDialog = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->joystickTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->metadataTabControl = (gcnew System::Windows::Forms::TabControl());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
			this->dataGridView2 = (gcnew System::Windows::Forms::DataGridView());
			this->tabPage5 = (gcnew System::Windows::Forms::TabPage());
			this->dataGridView3 = (gcnew System::Windows::Forms::DataGridView());
			this->videoSaveTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->mapPanel = (gcnew System::Windows::Forms::Panel());
			this->mapCenterGPSLabel = (gcnew System::Windows::Forms::Label());
			this->mapMouseGPSLabel = (gcnew System::Windows::Forms::Label());
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->mapUpdateTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->mapMenuStrip = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->mapLookGPSToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->simReadVidDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBox3 = (gcnew System::Windows::Forms::PictureBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->process1 = (gcnew System::Diagnostics::Process());
			this->confirmed_IDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_ThumbnailColumn = (gcnew System::Windows::Forms::DataGridViewImageColumn());
			this->confirmed_LatitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_LongitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_HeadingColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_LetterColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_LetterColor = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_Shape = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_ShapeColor = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_ProcessedColumn = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->confirmed_VerifiedColumn = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->candidate_IDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->candidate_ThumbnailColumn = (gcnew System::Windows::Forms::DataGridViewImageColumn());
			this->candidate_LatitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->canidate_LongitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->candidate_HeadingColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->candidate_LetterColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->candidate_LetterColor = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->candidate_Shape = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->candidate_ShapeColor = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->candidate_ProcessedColumn = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->candidate_VerifiedColumn = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->unverified_IDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_ThumbnailColumn = (gcnew System::Windows::Forms::DataGridViewImageColumn());
			this->unverified_LatitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_LongitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_HeadingColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_CenterPixel = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverfied_LetterColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_LetterColor = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_Shape = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_ShapeColor = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_ProcessedColumn = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->unverified_VerifiedColumn = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->menuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->metadataTable))->BeginInit();
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->tabPage2->SuspendLayout();
			this->metadataTabControl->SuspendLayout();
			this->tabPage3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->BeginInit();
			this->tabPage4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView2))->BeginInit();
			this->tabPage5->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView3))->BeginInit();
			this->tableLayoutPanel1->SuspendLayout();
			this->mapMenuStrip->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox3))->BeginInit();
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->BackColor = System::Drawing::Color::Gray;
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->toolStripMenuItem1, 
				this->serialCommunicationsToolStripMenuItem, this->mapToolStripMenuItem, this->databaseToolStripMenuItem, this->simulatorToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(2987, 24);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// toolStripMenuItem1
			// 
			this->toolStripMenuItem1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->exportDataToolStripMenuItem, 
				this->toolStripSeparator3, this->exitToolStripMenuItem});
			this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
			this->toolStripMenuItem1->Size = System::Drawing::Size(37, 20);
			this->toolStripMenuItem1->Text = L"&File";
			// 
			// exportDataToolStripMenuItem
			// 
			this->exportDataToolStripMenuItem->Name = L"exportDataToolStripMenuItem";
			this->exportDataToolStripMenuItem->Size = System::Drawing::Size(143, 22);
			this->exportDataToolStripMenuItem->Text = L"Export &Data...";
			this->exportDataToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exportDataToolStripMenuItem_Click);
			// 
			// toolStripSeparator3
			// 
			this->toolStripSeparator3->Name = L"toolStripSeparator3";
			this->toolStripSeparator3->Size = System::Drawing::Size(140, 6);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::F4));
			this->exitToolStripMenuItem->Size = System::Drawing::Size(143, 22);
			this->exitToolStripMenuItem->Text = L"&Exit";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exitToolStripMenuItem_Click);
			// 
			// serialCommunicationsToolStripMenuItem
			// 
			this->serialCommunicationsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->connectToolStripMenuItem, 
				this->toolStripSeparator1, this->comPortStripComboBox, this->baudRateStripComboBox, this->toolStripSeparator2, this->disconnectToolStripMenuItem});
			this->serialCommunicationsToolStripMenuItem->Name = L"serialCommunicationsToolStripMenuItem";
			this->serialCommunicationsToolStripMenuItem->Size = System::Drawing::Size(24, 20);
			this->serialCommunicationsToolStripMenuItem->Text = L"-";
			this->serialCommunicationsToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::serialCommunicationsToolStripMenuItem_Click);
			// 
			// connectToolStripMenuItem
			// 
			this->connectToolStripMenuItem->Name = L"connectToolStripMenuItem";
			this->connectToolStripMenuItem->Size = System::Drawing::Size(181, 22);
			this->connectToolStripMenuItem->Text = L"Connect";
			this->connectToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::connectToolStripMenuItem_Click);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(178, 6);
			// 
			// comPortStripComboBox
			// 
			this->comPortStripComboBox->Name = L"comPortStripComboBox";
			this->comPortStripComboBox->Size = System::Drawing::Size(121, 23);
			this->comPortStripComboBox->Text = L"Com Port";
			this->comPortStripComboBox->TextChanged += gcnew System::EventHandler(this, &Form1::comPortStripComboBox_TextChanged);
			// 
			// baudRateStripComboBox
			// 
			this->baudRateStripComboBox->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"2400", L"4800", L"9600", L"19200", 
				L"38400", L"57600", L"115200", L"31250"});
			this->baudRateStripComboBox->Name = L"baudRateStripComboBox";
			this->baudRateStripComboBox->Size = System::Drawing::Size(121, 23);
			this->baudRateStripComboBox->Text = L"Baud Rate";
			this->baudRateStripComboBox->TextChanged += gcnew System::EventHandler(this, &Form1::baudRateStripComboBox_TextChanged);
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(178, 6);
			// 
			// disconnectToolStripMenuItem
			// 
			this->disconnectToolStripMenuItem->Name = L"disconnectToolStripMenuItem";
			this->disconnectToolStripMenuItem->Size = System::Drawing::Size(181, 22);
			this->disconnectToolStripMenuItem->Text = L"Disconnect";
			this->disconnectToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::disconnectToolStripMenuItem_Click);
			// 
			// mapToolStripMenuItem
			// 
			this->mapToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->providerToolStripMenuItem, 
				this->lockToolStripMenuItem, this->downloadToolStripMenuItem});
			this->mapToolStripMenuItem->Name = L"mapToolStripMenuItem";
			this->mapToolStripMenuItem->Size = System::Drawing::Size(43, 20);
			this->mapToolStripMenuItem->Text = L"Map";
			// 
			// providerToolStripMenuItem
			// 
			this->providerToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->mapnikToolStripMenuItem, 
				this->osmarenderToolStripMenuItem, this->cyclemapToolStripMenuItem});
			this->providerToolStripMenuItem->Name = L"providerToolStripMenuItem";
			this->providerToolStripMenuItem->Size = System::Drawing::Size(128, 22);
			this->providerToolStripMenuItem->Text = L"Provider";
			// 
			// mapnikToolStripMenuItem
			// 
			this->mapnikToolStripMenuItem->Name = L"mapnikToolStripMenuItem";
			this->mapnikToolStripMenuItem->Size = System::Drawing::Size(139, 22);
			this->mapnikToolStripMenuItem->Text = L"Mapnik";
			this->mapnikToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::mapnikToolStripMenuItem_Click);
			// 
			// osmarenderToolStripMenuItem
			// 
			this->osmarenderToolStripMenuItem->Name = L"osmarenderToolStripMenuItem";
			this->osmarenderToolStripMenuItem->Size = System::Drawing::Size(139, 22);
			this->osmarenderToolStripMenuItem->Text = L"Osmarender";
			this->osmarenderToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::osmarenderToolStripMenuItem_Click);
			// 
			// cyclemapToolStripMenuItem
			// 
			this->cyclemapToolStripMenuItem->Name = L"cyclemapToolStripMenuItem";
			this->cyclemapToolStripMenuItem->Size = System::Drawing::Size(139, 22);
			this->cyclemapToolStripMenuItem->Text = L"Cyclemap";
			this->cyclemapToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cyclemapToolStripMenuItem_Click);
			// 
			// lockToolStripMenuItem
			// 
			this->lockToolStripMenuItem->Name = L"lockToolStripMenuItem";
			this->lockToolStripMenuItem->Size = System::Drawing::Size(128, 22);
			this->lockToolStripMenuItem->Text = L"Lock";
			this->lockToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::lockToolStripMenuItem_Click);
			// 
			// downloadToolStripMenuItem
			// 
			this->downloadToolStripMenuItem->Name = L"downloadToolStripMenuItem";
			this->downloadToolStripMenuItem->Size = System::Drawing::Size(128, 22);
			this->downloadToolStripMenuItem->Text = L"Download";
			this->downloadToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::downloadToolStripMenuItem_Click);
			// 
			// databaseToolStripMenuItem
			// 
			this->databaseToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->resetToolStripMenuItem});
			this->databaseToolStripMenuItem->Name = L"databaseToolStripMenuItem";
			this->databaseToolStripMenuItem->Size = System::Drawing::Size(67, 20);
			this->databaseToolStripMenuItem->Text = L"Database";
			// 
			// resetToolStripMenuItem
			// 
			this->resetToolStripMenuItem->Name = L"resetToolStripMenuItem";
			this->resetToolStripMenuItem->Size = System::Drawing::Size(102, 22);
			this->resetToolStripMenuItem->Text = L"Reset";
			this->resetToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::resetToolStripMenuItem_Click);
			// 
			// simulatorToolStripMenuItem
			// 
			this->simulatorToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->choosePathToolStripMenuItem, 
				this->pauseToolStripMenuItem, this->stopToolStripMenuItem});
			this->simulatorToolStripMenuItem->Name = L"simulatorToolStripMenuItem";
			this->simulatorToolStripMenuItem->Size = System::Drawing::Size(70, 20);
			this->simulatorToolStripMenuItem->Text = L"Simulator";
			// 
			// choosePathToolStripMenuItem
			// 
			this->choosePathToolStripMenuItem->Name = L"choosePathToolStripMenuItem";
			this->choosePathToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->choosePathToolStripMenuItem->Text = L"Choose Path";
			this->choosePathToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::choosePathToolStripMenuItem_Click);
			// 
			// pauseToolStripMenuItem
			// 
			this->pauseToolStripMenuItem->Name = L"pauseToolStripMenuItem";
			this->pauseToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->pauseToolStripMenuItem->Text = L"Pause";
			this->pauseToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::pauseToolStripMenuItem_Click);
			// 
			// stopToolStripMenuItem
			// 
			this->stopToolStripMenuItem->Name = L"stopToolStripMenuItem";
			this->stopToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->stopToolStripMenuItem->Text = L"Stop";
			this->stopToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::stopToolStripMenuItem_Click);
			// 
			// openGLTimer
			// 
			this->openGLTimer->Enabled = true;
			this->openGLTimer->Interval = 30;
			this->openGLTimer->Tick += gcnew System::EventHandler(this, &Form1::openGLTimer_Tick);
			// 
			// openGLPanel
			// 
			this->openGLPanel->Location = System::Drawing::Point(385, 55);
			this->openGLPanel->Name = L"openGLPanel";
			this->openGLPanel->Size = System::Drawing::Size(1280, 720);
			this->openGLPanel->TabIndex = 1;
			this->openGLPanel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::openGLPanel_Paint);
			// 
			// gimbalHUDPanel
			// 
			this->gimbalHUDPanel->BackColor = System::Drawing::Color::Transparent;
			this->gimbalHUDPanel->Location = System::Drawing::Point(997, 790);
			this->gimbalHUDPanel->Name = L"gimbalHUDPanel";
			this->gimbalHUDPanel->Size = System::Drawing::Size(134, 134);
			this->gimbalHUDPanel->TabIndex = 34;
			// 
			// errorLogTextBox
			// 
			this->errorLogTextBox->AutoWordSelection = true;
			this->errorLogTextBox->BackColor = System::Drawing::Color::Black;
			this->errorLogTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->errorLogTextBox->ForeColor = System::Drawing::SystemColors::Window;
			this->errorLogTextBox->Location = System::Drawing::Point(12, 492);
			this->errorLogTextBox->Name = L"errorLogTextBox";
			this->errorLogTextBox->ReadOnly = true;
			this->errorLogTextBox->Size = System::Drawing::Size(292, 283);
			this->errorLogTextBox->TabIndex = 2;
			this->errorLogTextBox->Text = L"";
			this->errorLogTextBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::errorLogTextBox_KeyPress);
			// 
			// metadataTable
			// 
			this->metadataTable->AllowUserToAddRows = false;
			this->metadataTable->AllowUserToDeleteRows = false;
			this->metadataTable->AllowUserToResizeColumns = false;
			this->metadataTable->AllowUserToResizeRows = false;
			dataGridViewCellStyle7->BackColor = System::Drawing::Color::DimGray;
			dataGridViewCellStyle7->ForeColor = System::Drawing::Color::White;
			dataGridViewCellStyle7->SelectionBackColor = System::Drawing::Color::White;
			dataGridViewCellStyle7->SelectionForeColor = System::Drawing::Color::Black;
			this->metadataTable->AlternatingRowsDefaultCellStyle = dataGridViewCellStyle7;
			this->metadataTable->BackgroundColor = System::Drawing::Color::Black;
			dataGridViewCellStyle8->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle8->BackColor = System::Drawing::Color::DimGray;
			dataGridViewCellStyle8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			dataGridViewCellStyle8->ForeColor = System::Drawing::Color::White;
			dataGridViewCellStyle8->SelectionBackColor = System::Drawing::Color::White;
			dataGridViewCellStyle8->SelectionForeColor = System::Drawing::Color::Black;
			dataGridViewCellStyle8->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->metadataTable->ColumnHeadersDefaultCellStyle = dataGridViewCellStyle8;
			this->metadataTable->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->metadataTable->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {this->Property, 
				this->Value});
			this->metadataTable->EditMode = System::Windows::Forms::DataGridViewEditMode::EditProgrammatically;
			this->metadataTable->GridColor = System::Drawing::Color::Black;
			this->metadataTable->Location = System::Drawing::Point(12, 55);
			this->metadataTable->Name = L"metadataTable";
			this->metadataTable->ReadOnly = true;
			dataGridViewCellStyle9->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle9->BackColor = System::Drawing::Color::Black;
			dataGridViewCellStyle9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			dataGridViewCellStyle9->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle9->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle9->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle9->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->metadataTable->RowHeadersDefaultCellStyle = dataGridViewCellStyle9;
			this->metadataTable->RowHeadersWidthSizeMode = System::Windows::Forms::DataGridViewRowHeadersWidthSizeMode::DisableResizing;
			this->metadataTable->RowTemplate->DefaultCellStyle->BackColor = System::Drawing::Color::Black;
			this->metadataTable->RowTemplate->DefaultCellStyle->ForeColor = System::Drawing::Color::White;
			this->metadataTable->RowTemplate->DefaultCellStyle->NullValue = L"none";
			this->metadataTable->RowTemplate->DefaultCellStyle->SelectionBackColor = System::Drawing::Color::White;
			this->metadataTable->RowTemplate->DefaultCellStyle->SelectionForeColor = System::Drawing::Color::Black;
			this->metadataTable->Size = System::Drawing::Size(292, 406);
			this->metadataTable->TabIndex = 3;
			this->metadataTable->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::metadataTable_CellContentClick);
			// 
			// Property
			// 
			this->Property->HeaderText = L"Property";
			this->Property->Name = L"Property";
			this->Property->ReadOnly = true;
			this->Property->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->Property->ToolTipText = L"Categories for metadata information.";
			// 
			// Value
			// 
			this->Value->HeaderText = L"Value";
			this->Value->Name = L"Value";
			this->Value->ReadOnly = true;
			this->Value->Width = 149;
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Location = System::Drawing::Point(1172, 781);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(493, 153);
			this->tabControl1->TabIndex = 5;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->checkBox1);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(485, 127);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Filters";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(6, 6);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(103, 17);
			this->checkBox1->TabIndex = 0;
			this->checkBox1->Text = L"Display Saliency";
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// tabPage2
			// 
			this->tabPage2->Controls->Add(this->label4);
			this->tabPage2->Controls->Add(this->checkedListBox1);
			this->tabPage2->Controls->Add(this->vidOptChangeDirButton);
			this->tabPage2->Controls->Add(this->label3);
			this->tabPage2->Controls->Add(this->label2);
			this->tabPage2->Controls->Add(this->encodingComboBox);
			this->tabPage2->Controls->Add(this->splitLengthTextBox);
			this->tabPage2->Controls->Add(this->vidOptOutputDirText);
			this->tabPage2->Controls->Add(this->label1);
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(485, 127);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"DONT USE THIS";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(352, 6);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(78, 13);
			this->label4->TabIndex = 10;
			this->label4->Text = L"Output Options";
			// 
			// checkedListBox1
			// 
			this->checkedListBox1->CheckOnClick = true;
			this->checkedListBox1->FormattingEnabled = true;
			this->checkedListBox1->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"Raw Input", L"Filtered Output", L"Text Overlay", 
				L"Disp Saliency"});
			this->checkedListBox1->Location = System::Drawing::Point(355, 22);
			this->checkedListBox1->Name = L"checkedListBox1";
			this->checkedListBox1->Size = System::Drawing::Size(120, 64);
			this->checkedListBox1->TabIndex = 9;
			this->checkedListBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::checkedListBox1_SelectedIndexChanged);
			// 
			// vidOptChangeDirButton
			// 
			this->vidOptChangeDirButton->Location = System::Drawing::Point(204, 3);
			this->vidOptChangeDirButton->Name = L"vidOptChangeDirButton";
			this->vidOptChangeDirButton->Size = System::Drawing::Size(100, 23);
			this->vidOptChangeDirButton->TabIndex = 6;
			this->vidOptChangeDirButton->Text = L"Change Directory";
			this->vidOptChangeDirButton->UseVisualStyleBackColor = true;
			this->vidOptChangeDirButton->Click += gcnew System::EventHandler(this, &Form1::vidOptChangeDirButton_Click);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(8, 63);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(52, 13);
			this->label3->TabIndex = 5;
			this->label3->Text = L"Encoding";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(8, 36);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(63, 13);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Split Length";
			// 
			// encodingComboBox
			// 
			this->encodingComboBox->FormattingEnabled = true;
			this->encodingComboBox->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"None", L"XviD", L"MJPG"});
			this->encodingComboBox->Location = System::Drawing::Point(98, 60);
			this->encodingComboBox->Name = L"encodingComboBox";
			this->encodingComboBox->Size = System::Drawing::Size(100, 21);
			this->encodingComboBox->TabIndex = 3;
			this->encodingComboBox->Text = L"None";
			this->encodingComboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::encodingComboBox_SelectedIndexChanged);
			// 
			// splitLengthTextBox
			// 
			this->splitLengthTextBox->Location = System::Drawing::Point(98, 33);
			this->splitLengthTextBox->Name = L"splitLengthTextBox";
			this->splitLengthTextBox->Size = System::Drawing::Size(100, 20);
			this->splitLengthTextBox->TabIndex = 2;
			this->splitLengthTextBox->Text = L"300";
			this->splitLengthTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->splitLengthTextBox->Validated += gcnew System::EventHandler(this, &Form1::splitLengthTextBox_Validated);
			// 
			// vidOptOutputDirText
			// 
			this->vidOptOutputDirText->Location = System::Drawing::Point(98, 6);
			this->vidOptOutputDirText->Name = L"vidOptOutputDirText";
			this->vidOptOutputDirText->ReadOnly = true;
			this->vidOptOutputDirText->Size = System::Drawing::Size(100, 20);
			this->vidOptOutputDirText->TabIndex = 1;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(8, 10);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(84, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Output Directory";
			// 
			// startRecordButton
			// 
			this->startRecordButton->Location = System::Drawing::Point(997, 31);
			this->startRecordButton->Name = L"startRecordButton";
			this->startRecordButton->Size = System::Drawing::Size(100, 23);
			this->startRecordButton->TabIndex = 7;
			this->startRecordButton->Text = L"Start Recording";
			this->startRecordButton->UseVisualStyleBackColor = true;
			this->startRecordButton->Click += gcnew System::EventHandler(this, &Form1::startRecordButton_Click);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(13, 473);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(58, 13);
			this->label5->TabIndex = 6;
			this->label5->Text = L"Status Log";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(16, 36);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(52, 13);
			this->label6->TabIndex = 7;
			this->label6->Text = L"Metadata";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(385, 35);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(34, 13);
			this->label7->TabIndex = 8;
			this->label7->Text = L"Video";
			// 
			// vidOptChangeDirBrowserDialog
			// 
			this->vidOptChangeDirBrowserDialog->Description = L"Select Video Output Directory";
			// 
			// joystickTimer
			// 
			this->joystickTimer->Enabled = true;
			this->joystickTimer->Interval = 125;
			this->joystickTimer->Tick += gcnew System::EventHandler(this, &Form1::joystickTimer_Tick);
			// 
			// metadataTabControl
			// 
			this->metadataTabControl->Controls->Add(this->tabPage3);
			this->metadataTabControl->Controls->Add(this->tabPage4);
			this->metadataTabControl->Controls->Add(this->tabPage5);
			this->metadataTabControl->Location = System::Drawing::Point(1698, 627);
			this->metadataTabControl->Name = L"metadataTabControl";
			this->metadataTabControl->SelectedIndex = 0;
			this->metadataTabControl->Size = System::Drawing::Size(1274, 307);
			this->metadataTabControl->TabIndex = 10;
			// 
			// tabPage3
			// 
			this->tabPage3->Controls->Add(this->dataGridView1);
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Padding = System::Windows::Forms::Padding(3);
			this->tabPage3->Size = System::Drawing::Size(1266, 281);
			this->tabPage3->TabIndex = 0;
			this->tabPage3->Text = L"Candidate Regions";
			this->tabPage3->UseVisualStyleBackColor = true;
			// 
			// dataGridView1
			// 
			this->dataGridView1->AllowUserToAddRows = false;
			this->dataGridView1->AllowUserToDeleteRows = false;
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(11) {this->candidate_IDColumn, 
				this->candidate_ThumbnailColumn, this->candidate_LatitudeColumn, this->canidate_LongitudeColumn, this->candidate_HeadingColumn, 
				this->candidate_LetterColumn, this->candidate_LetterColor, this->candidate_Shape, this->candidate_ShapeColor, this->candidate_ProcessedColumn, 
				this->candidate_VerifiedColumn});
			this->dataGridView1->Location = System::Drawing::Point(6, 6);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->RowTemplate->Height = 64;
			this->dataGridView1->Size = System::Drawing::Size(1254, 269);
			this->dataGridView1->TabIndex = 0;
			this->dataGridView1->CellDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::dataGridView1_CellDoubleClick);
			this->dataGridView1->DataError += gcnew System::Windows::Forms::DataGridViewDataErrorEventHandler(this, &Form1::dataGridView1_DataError);
			// 
			// tabPage4
			// 
			this->tabPage4->Controls->Add(this->dataGridView2);
			this->tabPage4->Location = System::Drawing::Point(4, 22);
			this->tabPage4->Name = L"tabPage4";
			this->tabPage4->Padding = System::Windows::Forms::Padding(3);
			this->tabPage4->Size = System::Drawing::Size(1266, 281);
			this->tabPage4->TabIndex = 1;
			this->tabPage4->Text = L"Unconfirmed Targets";
			this->tabPage4->UseVisualStyleBackColor = true;
			// 
			// dataGridView2
			// 
			this->dataGridView2->AllowUserToAddRows = false;
			this->dataGridView2->AllowUserToDeleteRows = false;
			this->dataGridView2->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView2->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(12) {this->unverified_IDColumn, 
				this->unverified_ThumbnailColumn, this->unverified_LatitudeColumn, this->unverified_LongitudeColumn, this->unverified_HeadingColumn, 
				this->unverified_CenterPixel, this->unverfied_LetterColumn, this->unverified_LetterColor, this->unverified_Shape, this->unverified_ShapeColor, 
				this->unverified_ProcessedColumn, this->unverified_VerifiedColumn});
			this->dataGridView2->Location = System::Drawing::Point(6, 6);
			this->dataGridView2->Name = L"dataGridView2";
			this->dataGridView2->RowTemplate->Height = 64;
			this->dataGridView2->Size = System::Drawing::Size(1254, 269);
			this->dataGridView2->TabIndex = 0;
			this->dataGridView2->CellDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::dataGridView2_CellDoubleClick);
			this->dataGridView2->CurrentCellDirtyStateChanged += gcnew System::EventHandler(this, &Form1::dataGridView2_CurrentCellDirtyStateChanged);
			this->dataGridView2->DataError += gcnew System::Windows::Forms::DataGridViewDataErrorEventHandler(this, &Form1::dataGridView2_DataError);
			// 
			// tabPage5
			// 
			this->tabPage5->Controls->Add(this->dataGridView3);
			this->tabPage5->Location = System::Drawing::Point(4, 22);
			this->tabPage5->Name = L"tabPage5";
			this->tabPage5->Padding = System::Windows::Forms::Padding(3);
			this->tabPage5->Size = System::Drawing::Size(1266, 281);
			this->tabPage5->TabIndex = 2;
			this->tabPage5->Text = L"Confirmed Targets";
			this->tabPage5->UseVisualStyleBackColor = true;
			// 
			// dataGridView3
			// 
			this->dataGridView3->AllowUserToAddRows = false;
			this->dataGridView3->AllowUserToDeleteRows = false;
			this->dataGridView3->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView3->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(11) {this->confirmed_IDColumn, 
				this->confirmed_ThumbnailColumn, this->confirmed_LatitudeColumn, this->confirmed_LongitudeColumn, this->confirmed_HeadingColumn, 
				this->confirmed_LetterColumn, this->confirmed_LetterColor, this->confirmed_Shape, this->confirmed_ShapeColor, this->confirmed_ProcessedColumn, 
				this->confirmed_VerifiedColumn});
			this->dataGridView3->Location = System::Drawing::Point(6, 6);
			this->dataGridView3->MultiSelect = false;
			this->dataGridView3->Name = L"dataGridView3";
			this->dataGridView3->RowTemplate->Height = 64;
			this->dataGridView3->Size = System::Drawing::Size(1254, 269);
			this->dataGridView3->TabIndex = 0;
			this->dataGridView3->CellDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::dataGridView3_CellDoubleClick);
			this->dataGridView3->CurrentCellDirtyStateChanged += gcnew System::EventHandler(this, &Form1::dataGridView2_CurrentCellDirtyStateChanged);
			this->dataGridView3->DataError += gcnew System::Windows::Forms::DataGridViewDataErrorEventHandler(this, &Form1::dataGridView3_DataError);
			// 
			// videoSaveTimer
			// 
			this->videoSaveTimer->Enabled = true;
			this->videoSaveTimer->Interval = 1000;
			this->videoSaveTimer->Tick += gcnew System::EventHandler(this, &Form1::videoSaveTimer_Tick);
			// 
			// mapPanel
			// 
			this->mapPanel->Location = System::Drawing::Point(1698, 55);
			this->mapPanel->Name = L"mapPanel";
			this->mapPanel->Size = System::Drawing::Size(1270, 525);
			this->mapPanel->TabIndex = 11;
			// 
			// mapCenterGPSLabel
			// 
			this->mapCenterGPSLabel->AutoSize = true;
			this->mapCenterGPSLabel->Location = System::Drawing::Point(1699, 583);
			this->mapCenterGPSLabel->Name = L"mapCenterGPSLabel";
			this->mapCenterGPSLabel->Size = System::Drawing::Size(66, 13);
			this->mapCenterGPSLabel->TabIndex = 12;
			this->mapCenterGPSLabel->Text = L"Center GPS:";
			// 
			// mapMouseGPSLabel
			// 
			this->mapMouseGPSLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->mapMouseGPSLabel->AutoSize = true;
			this->mapMouseGPSLabel->Location = System::Drawing::Point(3, 0);
			this->mapMouseGPSLabel->Name = L"mapMouseGPSLabel";
			this->mapMouseGPSLabel->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->mapMouseGPSLabel->Size = System::Drawing::Size(95, 13);
			this->mapMouseGPSLabel->TabIndex = 13;
			this->mapMouseGPSLabel->Text = L"Mouse GPS:sdfsdf";
			this->mapMouseGPSLabel->TextAlign = System::Drawing::ContentAlignment::TopRight;
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->tableLayoutPanel1->AutoSize = true;
			this->tableLayoutPanel1->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->tableLayoutPanel1->ColumnCount = 1;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel1->Controls->Add(this->mapMouseGPSLabel, 0, 0);
			this->tableLayoutPanel1->GrowStyle = System::Windows::Forms::TableLayoutPanelGrowStyle::FixedSize;
			this->tableLayoutPanel1->Location = System::Drawing::Point(2870, 586);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 1;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(101, 13);
			this->tableLayoutPanel1->TabIndex = 14;
			// 
			// mapUpdateTimer
			// 
			this->mapUpdateTimer->Enabled = true;
			this->mapUpdateTimer->Interval = 250;
			this->mapUpdateTimer->Tick += gcnew System::EventHandler(this, &Form1::mapUpdateTimer_Tick);
			// 
			// mapMenuStrip
			// 
			this->mapMenuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->mapLookGPSToolStripMenuItem});
			this->mapMenuStrip->Name = L"mapMenuStrip";
			this->mapMenuStrip->Size = System::Drawing::Size(129, 26);
			// 
			// mapLookGPSToolStripMenuItem
			// 
			this->mapLookGPSToolStripMenuItem->Name = L"mapLookGPSToolStripMenuItem";
			this->mapLookGPSToolStripMenuItem->Size = System::Drawing::Size(128, 22);
			this->mapLookGPSToolStripMenuItem->Text = L"Look Here";
			this->mapLookGPSToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::mapLookGPSToolStripMenuItem_Click);
			// 
			// simReadVidDialog
			// 
			this->simReadVidDialog->FileName = L"simReadVidDialog";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(550, 31);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 15;
			this->button1->Text = L"Zoom in";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(649, 31);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 16;
			this->button2->Text = L"Stop Zoom";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(749, 31);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(75, 23);
			this->button3->TabIndex = 17;
			this->button3->Text = L"Zoom Out";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(1266, 30);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(100, 23);
			this->button4->TabIndex = 18;
			this->button4->Text = L"Connect";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &Form1::connectButton_Click);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->BackColor = System::Drawing::Color::DimGray;
			this->label8->Enabled = false;
			this->label8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label8->Location = System::Drawing::Point(305, 76);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(80, 16);
			this->label8->TabIndex = 19;
			this->label8->Text = L"Recording";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Enabled = false;
			this->label11->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label11->Location = System::Drawing::Point(310, 176);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(69, 16);
			this->label11->TabIndex = 22;
			this->label11->Text = L"Autopilot";
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Enabled = false;
			this->label12->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label12->Location = System::Drawing::Point(312, 276);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(57, 16);
			this->label12->TabIndex = 23;
			this->label12->Text = L"Gimbal";
			// 
			// pictureBox1
			// 
			this->pictureBox1->Enabled = false;
			this->pictureBox1->Location = System::Drawing::Point(310, 95);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(69, 69);
			this->pictureBox1->TabIndex = 24;
			this->pictureBox1->TabStop = false;
			// 
			// pictureBox2
			// 
			this->pictureBox2->Enabled = false;
			this->pictureBox2->Location = System::Drawing::Point(310, 195);
			this->pictureBox2->Name = L"pictureBox2";
			this->pictureBox2->Size = System::Drawing::Size(69, 69);
			this->pictureBox2->TabIndex = 25;
			this->pictureBox2->TabStop = false;
			// 
			// pictureBox3
			// 
			this->pictureBox3->Enabled = false;
			this->pictureBox3->Location = System::Drawing::Point(310, 295);
			this->pictureBox3->Name = L"pictureBox3";
			this->pictureBox3->Size = System::Drawing::Size(69, 69);
			this->pictureBox3->TabIndex = 26;
			this->pictureBox3->TabStop = false;
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label9->ForeColor = System::Drawing::Color::Chartreuse;
			this->label9->Location = System::Drawing::Point(4, 5);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(151, 13);
			this->label9->TabIndex = 27;
			this->label9->Text = L"Green = Running / Connected";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label10->ForeColor = System::Drawing::Color::Yellow;
			this->label10->Location = System::Drawing::Point(4, 26);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(109, 13);
			this->label10->TabIndex = 28;
			this->label10->Text = L"Yellow = Packet Loss";
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label13->ForeColor = System::Drawing::Color::DarkRed;
			this->label13->Location = System::Drawing::Point(4, 49);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(176, 13);
			this->label13->TabIndex = 29;
			this->label13->Text = L"Red = Not Running / Disconnected";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label14->ForeColor = System::Drawing::SystemColors::ControlLightLight;
			this->label14->Location = System::Drawing::Point(4, 96);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(180, 13);
			this->label14->TabIndex = 30;
			this->label14->Text = L"Autopilot = Telemetry Data from UAV";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label15->ForeColor = System::Drawing::SystemColors::ControlLightLight;
			this->label15->Location = System::Drawing::Point(4, 119);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(223, 13);
			this->label15->TabIndex = 31;
			this->label15->Text = L"Gimbal = Communications for Pointing Camera";
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label16->ForeColor = System::Drawing::SystemColors::ControlLightLight;
			this->label16->Location = System::Drawing::Point(4, 72);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(217, 13);
			this->label16->TabIndex = 32;
			this->label16->Text = L"Recording = Recording Telemetry and Video";
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::Black;
			this->panel1->Controls->Add(this->label13);
			this->panel1->Controls->Add(this->label9);
			this->panel1->Controls->Add(this->label10);
			this->panel1->Controls->Add(this->label16);
			this->panel1->Controls->Add(this->label14);
			this->panel1->Controls->Add(this->label15);
			this->panel1->Location = System::Drawing::Point(12, 790);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(292, 140);
			this->panel1->TabIndex = 33;
			// 
			// process1
			// 
			this->process1->StartInfo->Domain = L"";
			this->process1->StartInfo->LoadUserProfile = false;
			this->process1->StartInfo->Password = nullptr;
			this->process1->StartInfo->StandardErrorEncoding = nullptr;
			this->process1->StartInfo->StandardOutputEncoding = nullptr;
			this->process1->StartInfo->UserName = L"";
			this->process1->SynchronizingObject = this;
			// 
			// confirmed_IDColumn
			// 
			this->confirmed_IDColumn->HeaderText = L"ID";
			this->confirmed_IDColumn->Name = L"confirmed_IDColumn";
			this->confirmed_IDColumn->ReadOnly = true;
			this->confirmed_IDColumn->Width = 70;
			// 
			// confirmed_ThumbnailColumn
			// 
			this->confirmed_ThumbnailColumn->HeaderText = L"Thumbnail";
			this->confirmed_ThumbnailColumn->Name = L"confirmed_ThumbnailColumn";
			this->confirmed_ThumbnailColumn->ReadOnly = true;
			this->confirmed_ThumbnailColumn->Width = 64;
			// 
			// confirmed_LatitudeColumn
			// 
			this->confirmed_LatitudeColumn->HeaderText = L"Latitude";
			this->confirmed_LatitudeColumn->Name = L"confirmed_LatitudeColumn";
			this->confirmed_LatitudeColumn->ReadOnly = true;
			// 
			// confirmed_LongitudeColumn
			// 
			this->confirmed_LongitudeColumn->HeaderText = L"Longitude";
			this->confirmed_LongitudeColumn->Name = L"confirmed_LongitudeColumn";
			this->confirmed_LongitudeColumn->ReadOnly = true;
			// 
			// confirmed_HeadingColumn
			// 
			this->confirmed_HeadingColumn->HeaderText = L"Heading";
			this->confirmed_HeadingColumn->Name = L"confirmed_HeadingColumn";
			this->confirmed_HeadingColumn->ReadOnly = true;
			// 
			// confirmed_LetterColumn
			// 
			this->confirmed_LetterColumn->HeaderText = L"Letter";
			this->confirmed_LetterColumn->Name = L"confirmed_LetterColumn";
			this->confirmed_LetterColumn->ReadOnly = true;
			// 
			// confirmed_LetterColor
			// 
			this->confirmed_LetterColor->HeaderText = L"Letter Color";
			this->confirmed_LetterColor->Name = L"confirmed_LetterColor";
			this->confirmed_LetterColor->ReadOnly = true;
			// 
			// confirmed_Shape
			// 
			this->confirmed_Shape->HeaderText = L"Shape";
			this->confirmed_Shape->Name = L"confirmed_Shape";
			this->confirmed_Shape->ReadOnly = true;
			// 
			// confirmed_ShapeColor
			// 
			this->confirmed_ShapeColor->HeaderText = L"Shape Color";
			this->confirmed_ShapeColor->Name = L"confirmed_ShapeColor";
			this->confirmed_ShapeColor->ReadOnly = true;
			// 
			// confirmed_ProcessedColumn
			// 
			this->confirmed_ProcessedColumn->HeaderText = L"Processed";
			this->confirmed_ProcessedColumn->Name = L"confirmed_ProcessedColumn";
			this->confirmed_ProcessedColumn->ReadOnly = true;
			this->confirmed_ProcessedColumn->ThreeState = true;
			this->confirmed_ProcessedColumn->TrueValue = L"true";
			// 
			// confirmed_VerifiedColumn
			// 
			this->confirmed_VerifiedColumn->HeaderText = L"Verified";
			this->confirmed_VerifiedColumn->Name = L"confirmed_VerifiedColumn";
			this->confirmed_VerifiedColumn->ThreeState = true;
			this->confirmed_VerifiedColumn->TrueValue = L"true";
			// 
			// candidate_IDColumn
			// 
			this->candidate_IDColumn->HeaderText = L"ID";
			this->candidate_IDColumn->Name = L"candidate_IDColumn";
			this->candidate_IDColumn->ReadOnly = true;
			this->candidate_IDColumn->Width = 70;
			// 
			// candidate_ThumbnailColumn
			// 
			this->candidate_ThumbnailColumn->HeaderText = L"Thumbnail";
			this->candidate_ThumbnailColumn->Name = L"candidate_ThumbnailColumn";
			this->candidate_ThumbnailColumn->ReadOnly = true;
			this->candidate_ThumbnailColumn->Width = 64;
			// 
			// candidate_LatitudeColumn
			// 
			this->candidate_LatitudeColumn->HeaderText = L"Latitude";
			this->candidate_LatitudeColumn->Name = L"candidate_LatitudeColumn";
			this->candidate_LatitudeColumn->ReadOnly = true;
			// 
			// canidate_LongitudeColumn
			// 
			this->canidate_LongitudeColumn->HeaderText = L"Longitude";
			this->canidate_LongitudeColumn->Name = L"canidate_LongitudeColumn";
			this->canidate_LongitudeColumn->ReadOnly = true;
			// 
			// candidate_HeadingColumn
			// 
			this->candidate_HeadingColumn->HeaderText = L"Heading";
			this->candidate_HeadingColumn->Name = L"candidate_HeadingColumn";
			this->candidate_HeadingColumn->ReadOnly = true;
			// 
			// candidate_LetterColumn
			// 
			this->candidate_LetterColumn->HeaderText = L"Letter";
			this->candidate_LetterColumn->Name = L"candidate_LetterColumn";
			this->candidate_LetterColumn->ReadOnly = true;
			// 
			// candidate_LetterColor
			// 
			this->candidate_LetterColor->HeaderText = L"Letter Color";
			this->candidate_LetterColor->Name = L"candidate_LetterColor";
			this->candidate_LetterColor->ReadOnly = true;
			// 
			// candidate_Shape
			// 
			this->candidate_Shape->HeaderText = L"Shape";
			this->candidate_Shape->Name = L"candidate_Shape";
			this->candidate_Shape->ReadOnly = true;
			// 
			// candidate_ShapeColor
			// 
			this->candidate_ShapeColor->HeaderText = L"Shape Color";
			this->candidate_ShapeColor->Name = L"candidate_ShapeColor";
			this->candidate_ShapeColor->ReadOnly = true;
			// 
			// candidate_ProcessedColumn
			// 
			this->candidate_ProcessedColumn->HeaderText = L"Processed";
			this->candidate_ProcessedColumn->Name = L"candidate_ProcessedColumn";
			this->candidate_ProcessedColumn->ReadOnly = true;
			// 
			// candidate_VerifiedColumn
			// 
			this->candidate_VerifiedColumn->HeaderText = L"Verified";
			this->candidate_VerifiedColumn->Name = L"candidate_VerifiedColumn";
			this->candidate_VerifiedColumn->ReadOnly = true;
			// 
			// unverified_IDColumn
			// 
			this->unverified_IDColumn->HeaderText = L"ID";
			this->unverified_IDColumn->Name = L"unverified_IDColumn";
			this->unverified_IDColumn->ReadOnly = true;
			this->unverified_IDColumn->Width = 70;
			// 
			// unverified_ThumbnailColumn
			// 
			this->unverified_ThumbnailColumn->HeaderText = L"Thumbnail";
			this->unverified_ThumbnailColumn->Name = L"unverified_ThumbnailColumn";
			this->unverified_ThumbnailColumn->ReadOnly = true;
			this->unverified_ThumbnailColumn->Width = 64;
			// 
			// unverified_LatitudeColumn
			// 
			this->unverified_LatitudeColumn->HeaderText = L"Latitude";
			this->unverified_LatitudeColumn->Name = L"unverified_LatitudeColumn";
			this->unverified_LatitudeColumn->ReadOnly = true;
			// 
			// unverified_LongitudeColumn
			// 
			this->unverified_LongitudeColumn->HeaderText = L"Longitude";
			this->unverified_LongitudeColumn->Name = L"unverified_LongitudeColumn";
			this->unverified_LongitudeColumn->ReadOnly = true;
			// 
			// unverified_HeadingColumn
			// 
			this->unverified_HeadingColumn->HeaderText = L"Heading";
			this->unverified_HeadingColumn->Name = L"unverified_HeadingColumn";
			this->unverified_HeadingColumn->ReadOnly = true;
			// 
			// unverified_CenterPixel
			// 
			this->unverified_CenterPixel->HeaderText = L"Center Pixel";
			this->unverified_CenterPixel->Name = L"unverified_CenterPixel";
			this->unverified_CenterPixel->ReadOnly = true;
			// 
			// unverfied_LetterColumn
			// 
			this->unverfied_LetterColumn->HeaderText = L"Letter";
			this->unverfied_LetterColumn->Name = L"unverfied_LetterColumn";
			this->unverfied_LetterColumn->ReadOnly = true;
			// 
			// unverified_LetterColor
			// 
			this->unverified_LetterColor->HeaderText = L"Letter Color";
			this->unverified_LetterColor->Name = L"unverified_LetterColor";
			this->unverified_LetterColor->ReadOnly = true;
			// 
			// unverified_Shape
			// 
			this->unverified_Shape->HeaderText = L"Shape";
			this->unverified_Shape->Name = L"unverified_Shape";
			this->unverified_Shape->ReadOnly = true;
			// 
			// unverified_ShapeColor
			// 
			this->unverified_ShapeColor->HeaderText = L"Shape Color";
			this->unverified_ShapeColor->Name = L"unverified_ShapeColor";
			this->unverified_ShapeColor->ReadOnly = true;
			// 
			// unverified_ProcessedColumn
			// 
			this->unverified_ProcessedColumn->HeaderText = L"Processed";
			this->unverified_ProcessedColumn->Name = L"unverified_ProcessedColumn";
			this->unverified_ProcessedColumn->ReadOnly = true;
			// 
			// unverified_VerifiedColumn
			// 
			this->unverified_VerifiedColumn->HeaderText = L"Verified";
			this->unverified_VerifiedColumn->Name = L"unverified_VerifiedColumn";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoValidate = System::Windows::Forms::AutoValidate::EnableAllowFocusChange;
			this->BackColor = System::Drawing::Color::DimGray;
			this->ClientSize = System::Drawing::Size(2987, 946);
			this->Controls->Add(this->gimbalHUDPanel);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->pictureBox3);
			this->Controls->Add(this->pictureBox2);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->label12);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->tableLayoutPanel1);
			this->Controls->Add(this->mapCenterGPSLabel);
			this->Controls->Add(this->mapPanel);
			this->Controls->Add(this->metadataTabControl);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->metadataTable);
			this->Controls->Add(this->startRecordButton);
			this->Controls->Add(this->errorLogTextBox);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->openGLPanel);
			this->Controls->Add(this->menuStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"Form1";
			this->Text = L"UCSD Skynet";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->Click += gcnew System::EventHandler(this, &Form1::Form1_Click);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::Form1_KeyDown);
			this->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::Form1_KeyPress);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->metadataTable))->EndInit();
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			this->metadataTabControl->ResumeLayout(false);
			this->tabPage3->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->EndInit();
			this->tabPage4->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView2))->EndInit();
			this->tabPage5->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView3))->EndInit();
			this->tableLayoutPanel1->ResumeLayout(false);
			this->tableLayoutPanel1->PerformLayout();
			this->mapMenuStrip->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox3))->EndInit();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) 
			 {
			 }

	/// <summary>
	/// Timer that handles rendering of OpenGL - what is actually rendered is handled in
	/// OpenGLForm.h
	/// </summary>
	private: System::Void openGLTimer_Tick(System::Object^  sender, System::EventArgs^  e) 
			 {
				 UNREFERENCED_PARAMETER(sender);
				 UNREFERENCED_PARAMETER(e);

				 if( openGLView == nullptr )
					 return;

				 openGLView->Render();
				 openGLView->SwapOpenGLBuffers();
			 }
private: System::Void metadataTable_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
		 }
private: System::Void serialCommunicationsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 Application::Exit();
		 }

private: System::Void vidOptChangeDirButton_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 String ^ temp;
		 
		 	 System::Windows::Forms::DialogResult result = vidOptChangeDirBrowserDialog->ShowDialog();

			 if ( result == ::DialogResult::OK )
			 {
			 	temp = vidOptChangeDirBrowserDialog->SelectedPath;

				vidOptOutputDirText->Text = temp;
				consoleMessage( "Video output directory changed to: " + temp, Color::Gray );
			 }

			 return;
		 
		 }		

	/// <summary>
	/// Outputs a message to the console in the specified color.
	/// Prefixes the message with the current date and time, postfixes with a newline
	/// </summary>
private: System::Void consoleMessage( String ^ message, Color col )
		 {
			 DateTime time = DateTime::Now;
			 errorLogTextBox->SelectionColor = Color::White;
			 errorLogTextBox->AppendText( time.ToString() + "\n" );
			 errorLogTextBox->SelectionColor = col;
			 errorLogTextBox->AppendText( message );
			 errorLogTextBox->AppendText( "\n" );

			 // Always scroll to the bottom of the text field.
			 errorLogTextBox->Select(errorLogTextBox->Text->Length, 0);
			 errorLogTextBox->ScrollToCaret();

		 	 return;
		 }
public:  System::Void doNothing() {
			 System::Diagnostics::Trace::WriteLine("Doing nothing");
		 }

public:	 System::Void saveCurrentImage() {
			 appController->saveCurrentFrameAsCandidateOnMainThread();
		 }

public:	 System::Void printToConsole( String ^ message, Color ^ color)
		 {
			 array<Object ^> ^ retArr = gcnew array< Object^ >{message, color};
	
			 printToConsole( retArr );
		 }
public:  System::Void printToConsole( array<Object ^> ^ retArr ) 
		 {
			 
			//System::Diagnostics::Trace::WriteLine("Form1::printToConsole");
			//consoleMessage( (String ^)retArr[0] + ((Int32)retArr[1]), Color::Blue);
			consoleMessage( (String ^)retArr[0], ((Color)retArr[1]));//((ColorRef ^)retArr[1])->theColor );
		 }

public:	 System::Void updateCommsStatus( array<Object ^> ^ retArr )
		 {
			 if (!isconnected)
				 return;

			 Image ^theImage;
			 // get result (yellow vs green)
			 if ((Int32)retArr[1] == GREEN_STATUS)
				 theImage = greenImage;
			 else if ((Int32)retArr[1] == YELLOW_STATUS)
				 theImage = yellowImage;
			 else 
				 theImage = redImage;

			 // get type (rabbit vs autopilot) and update image
			 if (((String ^)retArr[0])->Equals("Autopilot"))
				 pictureBox2->Image = theImage;
			 else if (((String ^)retArr[0])->Equals("Rabbit"))
				 pictureBox3->Image = theImage;


		 }
private: System::Void errorMessage( String ^ message )
		 {
			 consoleMessage( "ERROR: " + message, Color::Red );
		 	 return;
		 }

private: System::Void comPortStripComboBox_TextChanged(System::Object^  sender, System::EventArgs^  e) {

			 // dont run this function, autoconnect makes this obsolete

			/* String ^ port = comPortStripComboBox->Text;

			 bool result = theComport->setPortName( port );

			 if( result )
			 {
			 	consoleMessage( "Comport changed to: " + port, Color::Gray );
			 }
			 else
			 {
			 	errorMessage( "Failed changing comport to: " + port );
			 }*/
		 }

private: System::Void baudRateStripComboBox_TextChanged(System::Object^  sender, System::EventArgs^  e) {

			 
			 // dont run this function, autoconnect makes this obsolete


			 /*
			 String ^ baudRateString = baudRateStripComboBox->Text;
			 // convert string to int
			 int baudRate = Convert::ToInt32( baudRateString, 10 );

			 bool result = theComport->setBaudRate( baudRate );

			 if( result )
			 {
			 	consoleMessage( "Baud rate changed to: " + baudRate, Color::Gray );
			 }
			 else
			 {
			 	errorMessage( "Failed changing baud rate to: " + baudRate );
			 }*/

		 }

private: System::Void joystickTimer_Tick(System::Object^  sender, System::EventArgs^  e) {
			m_joystick->UpdateInputState( NULL );
		 }


private: System::Void connectToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
		 {		
			 
			 // dont run this function, autoconnect makes this obsolete
			 
			 /*bool result = theComport->connect();

			 if (result)
			 {
			 	consoleMessage( "Connected to comport " + comPortStripComboBox->Text + " with baud rate " + baudRateStripComboBox->Text, Color::Green );

				comReadThread = gcnew Thread(gcnew ThreadStart(theComport, &Communications::Comport::readThread));
				comReadThread->Name = "Comport Read Thread";
				comReadThread->Start();

			 	isReadingData = false;
			 }
			 else
			 	errorMessage( "Failed to connect" );*/
		 }

private: System::Void disconnectToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			 
			// dont run this function, autoconnect makes this obsolete


			 /*
			bool result = theComport->disconnect();

			if (result)
			{
				consoleMessage( "Disconnected from comport " + comPortStripComboBox->Text, Color::Green );
				comReadThread->Abort();			
			}
			else
				errorMessage( "Failed to disconnect" );
				*/

		 }

private: bool isReadingData;

public: System::Void insertCandidateData( Database::CandidateRowData ^ data) 
		{
			if (data == nullptr) {
				
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::insertCandidateData(): data is null");
				return;
			}

			try {
				int num = data->id;
			} 
			catch (Exception ^ ) {
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::insertCandidateData(): data is null");
				return;
			}
			int rowNum;
			DataGridView ^ grid = this->dataGridView1;
			
			rowNum = grid->Rows->Add();
			grid->Rows[rowNum]->Cells[0]->Value = "" + data->id;

			Image ^ thumbnail;
			try
			{
				String ^ path = HTTP_SERVER_TARGET_PATH + data->imageName->Remove(0, 8);
				thumbnail = Image::FromFile( path );
				thumbnail = thumbnail->GetThumbnailImage( 64, 64, nullptr, IntPtr::Zero );
			}
			catch( Exception ^ e)
			{
				System::Diagnostics::Trace::WriteLine("Failure to add image to table: " + e);
			}

			double lat, lon, alt;
			Vision::Saliency::getCenterGPSFromCandidateData(data, lat, lon, alt);

			grid->Rows[rowNum]->Cells[1]->Value = thumbnail;
			grid->Rows[rowNum]->Cells[2]->Value = "" + Single(lat).ToString("######.#######") + "*";
			grid->Rows[rowNum]->Cells[3]->Value = "" + Single(lon).ToString("######.#######") + "*";
			grid->Rows[rowNum]->Cells[4]->Value = "0*";
			grid->Rows[rowNum]->Cells[5]->Value = data->letter;
			grid->Rows[rowNum]->Cells[6]->Value = data->shape;
			grid->Rows[rowNum]->Cells[7]->Value = data->letterColor;
			grid->Rows[rowNum]->Cells[8]->Value = data->shapeColor;
			grid->Rows[rowNum]->Cells[9]->Value = true;

		}
public: System::Void insertTargetData( Database::TargetRowData ^ data) 
		{
			int rowNum;

			DataGridView ^ grid = this->dataGridView2;
			
			rowNum = grid->Rows->Add();
			grid->Rows[rowNum]->Cells[0]->Value = "" + data->id;

			Image ^ thumbnail;
			try
			{
				String ^ path = HTTP_SERVER_TARGET_PATH + data->imageName->Remove(0, 8);
				thumbnail = Image::FromFile( path );
				thumbnail = thumbnail->GetThumbnailImage( 64, 64, nullptr, IntPtr::Zero );
			}
			catch( Exception ^ e)
			{
				System::Diagnostics::Trace::WriteLine("Failure to add image to table: " + e);
			}

			double lat, lon, alt;
			Vision::Saliency::getTargetGPS(data, lat, lon, alt);

			grid->Rows[rowNum]->Cells[1]->Value = thumbnail;
			grid->Rows[rowNum]->Cells[2]->Value = "" + Single(lat).ToString("######.#######") + "*";
			grid->Rows[rowNum]->Cells[3]->Value = "" + Single(lon).ToString("######.#######") + "*";
			grid->Rows[rowNum]->Cells[4]->Value = "0*";
			grid->Rows[rowNum]->Cells[5]->Value = "("+ data->targetX + "," + data->targetY+")";
			grid->Rows[rowNum]->Cells[6]->Value = data->letter;
			grid->Rows[rowNum]->Cells[7]->Value = data->shape;
			grid->Rows[rowNum]->Cells[8]->Value = data->letterColor;
			grid->Rows[rowNum]->Cells[9]->Value = data->shapeColor;
			grid->Rows[rowNum]->Cells[10]->Value = true;

		}
public: System::Void insertVerifiedTargetData( Database::VerifiedTargetRowData ^ data) 
		{
			System::Diagnostics::Trace::WriteLine("ERROR in Form1::insertVerifiedTargetData(): function not implemented");
		}

public: System::Void modifyCandidateInTable( Database::CandidateRowData ^ data )
		{
			if (data == nullptr) {
				
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::modifyCandidateInTable(): data is null");
				return;
			}

			try {
				int num = data->id;
			} 
			catch (Exception ^ ) {
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::modifyCandidateInTable(): data is null");
				return;
			}

			DataGridView ^ grid = this->dataGridView1;

			for (int i = 0; i < grid->Rows->Count; i++) {
				if (grid->Rows[i]->Cells[0]->Value->Equals("" + data->id)) {
					// modify row
					int rowNum = i;
					grid->Rows[rowNum]->Cells[0]->Value = "" + data->id;

					Image ^ thumbnail;
					try
					{
						String ^ path = HTTP_SERVER_TARGET_PATH + data->imageName->Remove(0, 8);
						thumbnail = Image::FromFile( path );
						thumbnail = thumbnail->GetThumbnailImage( 64, 64, nullptr, IntPtr::Zero );
					}
					catch( Exception ^ e)
					{
						System::Diagnostics::Trace::WriteLine("Failure to add image to table: " + e);
					}

					double lat, lon, alt;
					Vision::Saliency::getCenterGPSFromCandidateData(data, lat, lon, alt);

					grid->Rows[rowNum]->Cells[1]->Value = thumbnail;
					grid->Rows[rowNum]->Cells[2]->Value = "" + Single(lat).ToString("######.#######") + "*";
					grid->Rows[rowNum]->Cells[3]->Value = "" + Single(lon).ToString("######.#######") + "*";
					grid->Rows[rowNum]->Cells[4]->Value = "0*";
					grid->Rows[rowNum]->Cells[5]->Value = data->letter;
					grid->Rows[rowNum]->Cells[6]->Value = data->shape;
					grid->Rows[rowNum]->Cells[7]->Value = data->letterColor;
					grid->Rows[rowNum]->Cells[8]->Value = data->shapeColor;
					grid->Rows[rowNum]->Cells[9]->Value = true;


				}
			}

		} 

public: System::Void modifyTargetInTable( Database::TargetRowData ^ data )
		{
			if (data == nullptr) {
				
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::modifyTargetInTable(): data is null");
				return;
			}

			try {
				int num = data->id;
			} 
			catch (Exception ^ ) {
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::modifyTargetInTable(): data is null");
				return;
			}

			DataGridView ^ grid = this->dataGridView2;

			for (int i = 0; i < grid->Rows->Count; i++) {
				if (grid->Rows[i]->Cells[0]->Value->Equals( + data->id)) {
					// modify row
					int rowNum = i;
					grid->Rows[rowNum]->Cells[0]->Value = "" + data->id;

					Image ^ thumbnail;
					try
					{
						String ^ path = HTTP_SERVER_TARGET_PATH + data->imageName->Remove(0, 8);
						thumbnail = Image::FromFile( path );
						thumbnail = thumbnail->GetThumbnailImage( 64, 64, nullptr, IntPtr::Zero );
					}
					catch( Exception ^ e)
					{
						System::Diagnostics::Trace::WriteLine("Failure to add image to table: " + e);
					}

					double lat, lon, alt;
					Vision::Saliency::getTargetGPS(data, lat, lon, alt);

					grid->Rows[rowNum]->Cells[1]->Value = thumbnail;
					grid->Rows[rowNum]->Cells[2]->Value = "" + Single(lat).ToString("######.#######") + "*";
					grid->Rows[rowNum]->Cells[3]->Value = "" + Single(lon).ToString("######.#######") + "*";
					grid->Rows[rowNum]->Cells[4]->Value = "0*";
					grid->Rows[rowNum]->Cells[5]->Value = "("+ data->targetX + "," + data->targetY+")";
					grid->Rows[rowNum]->Cells[6]->Value = data->letter;
					grid->Rows[rowNum]->Cells[7]->Value = data->shape;
					grid->Rows[rowNum]->Cells[8]->Value = data->letterColor;
					grid->Rows[rowNum]->Cells[9]->Value = data->shapeColor;
					grid->Rows[rowNum]->Cells[10]->Value = true;


				}
			}

		}

public: System::Void removeCandidateFromTable(String ^ id)
		{
			DataGridView ^ grid = this->dataGridView1;

			for (int i = 0; i < grid->Rows->Count; i++) {
				if (grid->Rows[i]->Cells[0]->Value->Equals(id)) {
					// remove row
					grid->Rows->RemoveAt(i);
				}
			}

		}
public: System::Void removeCandidateFromTable(Database::CandidateRowData ^ data)
		{
			removeCandidateFromTable("" + data->id);
		}

public: System::Void removeTargetFromTable(String ^ id)
		{
			DataGridView ^ grid = this->dataGridView2;

			for (int i = 0; i < grid->Rows->Count; i++) {
				if (grid->Rows[i]->Cells[0]->Value->Equals(id)) {
					// remove row
					grid->Rows->RemoveAt(i);
				}
			}

		}
public: System::Void removeTargetFromTable(Database::TargetRowData ^ data)
		{
			removeTargetFromTable("" + data->id);
		}

public: System::Void removeVerifiedTargetFromTable(Database::VerifiedTargetRowData ^ data)
		{
			System::Diagnostics::Trace::WriteLine("ERROR in Form1::removeVerifiedTargetFromTable(): function not implemented");
			

		}

public: System::Void insertTable( Database::TableName table, Database::RowData ^ data )
		{
			int rowNum;
			DataGridView ^ grid;
			switch( table )
			{
				case Database::tableCandidateRegions:
					grid = this->dataGridView1;
					break;
				case Database::tableUnverifiedTargets:
					grid = this->dataGridView2;
					break;
				case Database::tableVerifiedTargets:
					grid = this->dataGridView3;
					break;
			}

			rowNum = grid->Rows->Add();
			grid->Rows[rowNum]->Cells[0]->Value = "" + data->id;

			Image ^ thumbnail;
			/*try //DEBUG: I removed this to test
			{
				String ^ path = data->path->Replace("\\\\", "\\");
				thumbnail = Image::FromFile( path );
				thumbnail = thumbnail->GetThumbnailImage( 64, 64, nullptr, IntPtr::Zero );
			}
			catch( Exception ^ )
			{
		System::Diagnostics::Trace::WriteLine("catch in form1");
			}*/

			grid->Rows[rowNum]->Cells[1]->Value = thumbnail;
			grid->Rows[rowNum]->Cells[2]->Value = "" + data->target_latitude;
			grid->Rows[rowNum]->Cells[3]->Value = "" + data->target_longitude;
			grid->Rows[rowNum]->Cells[4]->Value = "" + data->heading;
			grid->Rows[rowNum]->Cells[5]->Value = data->letter;
			grid->Rows[rowNum]->Cells[6]->Value = data->shape;
			grid->Rows[rowNum]->Cells[7]->Value = data->fg_color;
			grid->Rows[rowNum]->Cells[8]->Value = data->bg_color;
			grid->Rows[rowNum]->Cells[9]->Value = data->processed;

		}

public: System::Void saveImage(){
			DateTime time = DateTime::Now;

			String ^ path = vidOptOutputDirText->Text + "\\image_" + time.ToString("o")->Replace(":", "-") + fileExtension;
			String ^ pathBase = vidOptOutputDirText->Text + "\\image_" + time.ToString("o")->Replace(":", "-");

			Database::RowData ^ data = gcnew Database::RowData();
			data->id = this->incrId++;
			data->path = path->Replace("\\", "\\\\");
			data->target_latitude = 0;			// Latitude of point selected as target
			data->target_longitude = 0;			// Longitude of point selected as target
			data->target_X = 0;					// pixel X of target
			data->target_Y = 0;					// pixel Y of target
			data->heading = 0;					// heading of target
			data->letter = "A";				
			data->shape = "Circle";					
			data->fg_color = "White";				// foreground color
			data->bg_color = "Yellow";				// background color
			data->processed = FALSE;					// whether this has been processed by OpenCV
			//data->verified = FALSE;					// human verified
			data->center_latitude = 0;			// Latitude of center pixel
			data->center_longitude = 0;			// Longitude of center pixel
			data->mapping_latitude = (float)Convert::ToDouble( this->metadataTable[1, A_LAT]->Value );			// pixel to meter translation for latitude
			data->mapping_longitude = (float)Convert::ToDouble( this->metadataTable[1, A_LON]->Value );		// pixel to meter translation for longitude
			
			/*data->homography = GeoReference::computeHomography( Convert::ToDouble( this->metadataTable[1, A_LAT]->Value ),
				Convert::ToDouble( this->metadataTable[1, A_LON]->Value ),
				Convert::ToDouble( this->metadataTable[1, A_ALT]->Value ),
				Convert::ToDouble( this->metadataTable[1, A_ROLL]->Value ),
				Convert::ToDouble( this->metadataTable[1, A_PITCH]->Value ),
				Convert::ToDouble( this->metadataTable[1, A_HEAD]->Value ),
				Convert::ToDouble( this->metadataTable[1, G_AZIM]->Value ),
				Convert::ToDouble( this->metadataTable[1, G_ELEV]->Value ),
				Convert::ToDouble( this->metadataTable[1, V_ZOOM]->Value ) );	*/

			//openGLView->saveImage( path );
			openGLView->saveImage( path, pathBase, data->homography, Convert::ToDouble( this->metadataTable[1, A_HEAD]->Value ) );

			this->insertTable( Database::tableUnverifiedTargets, data);
			db->insertData( Database::tableUnverifiedTargets, data);
		}

public: System::Void addRawTarget(Database::RowData ^ data) {
			
			//System::Diagnostics::Trace::WriteLine( "FORM1 RAN addRawTarget");
			//System::Diagnostics::Trace::WriteLine( "data: ");
			data->id = this->incrId++;
			this->insertTable( Database::tableCandidateRegions, data);
			//db->insertData( Database::tableCandidateRegions, data);
			//System::Diagnostics::Trace::WriteLine( "FORM1 FINISHED addRawTarget");
		}

public: System::Void noComportData( ) {
			if( isReadingData )
			{
				errorMessage( "No COM link to airplane" );
				isReadingData = false;
			}
		 }

public: System::Void comportEstablish( ) {
			if( !isReadingData )
			{
				consoleMessage( "COM link established", Color::Green );
				isReadingData = true;
			}
		}

private: System::Void checkedListBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			  IEnumerator^ myEnum2 = checkedListBox1->CheckedItems->GetEnumerator();
		      while ( myEnum2->MoveNext() )
		      {
		         Object^ itemChecked = safe_cast<Object^>(myEnum2->Current);
				//if (Compare(itemChecked
		         // Use the IndexOf method to get the index of an item.
		         MessageBox::Show( String::Concat( "Item with title: \"", itemChecked, "\", is checked. Checked state is: ", checkedListBox1->GetItemCheckState( checkedListBox1->Items->IndexOf( itemChecked ) ), "." ) );
		      }
		 }
private: System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			
			 /*if (checkBox1->Checked) {
				 openGLView->switchSaliencyState( true );
				 
				 consoleMessage( "Displaying video and saliency", Color::Green );
			
			 } else {
				 openGLView->switchSaliencyState( false );
				 consoleMessage( "Displaying video but not saliency", Color::Green );
			
			 }*/
			 
		     
		 }

private: void AddText( Stream^ fs, String^ value )
{
   array<Byte>^info = (gcnew UTF8Encoding( true ))->GetBytes( value );
   fs->Write( info, 0, info->Length );
}
		 
public: System::Void reloadTable( ) {
			

			
			Communications::PlaneState ^ state = thePlaneWatcher->predictLocationAtTime(0.0);

			this->metadataTable[1, G_ROLL]->Value = "" + thePlaneWatcher->rawToDegrees(state->gimbalInfo->roll) + "*"; //azimuth;
			this->metadataTable[1, G_PITCH]->Value = "" + thePlaneWatcher->rawToDegrees(state->gimbalInfo->pitch) + "*"; //elevation;
			this->metadataTable[1, A_ALT]->Value = "" + Single(state->gpsData->gpsAltitude).ToString("F") + " (m)"; // altitude;
			this->metadataTable[1, A_LAT]->Value = "" + Single(state->gpsData->gpsLatitude).ToString("######.#######") + "*"; // latitude;
			this->metadataTable[1, A_LON]->Value = "" + Single(state->gpsData->gpsLongitude).ToString("######.#######") + "*"; // longitude; // ######.####
			this->metadataTable[1, A_HEAD]->Value = "" + Single(state->telemData->heading).ToString("F") + "*"; //heading;
			this->metadataTable[1, A_ROLL]->Value = "" + Single(state->telemData->roll).ToString("F") + "*"; // roll;
			this->metadataTable[1, A_PITCH]->Value = "" + Single(state->telemData->pitch).ToString("F") + "*"; //pitch;
			this->metadataTable[1, V_ZOOM]->Value = "" + state->gimbalInfo->zoom + "x"; // zoom;
			
			//System::Diagnostics::Trace::WriteLine("Form1::reloadTable() lat:" + state->gpsData->gpsLatitude + " lon:" + state->gpsData->gpsLongitude + " roll:" + state->telemData->roll  + 
			//								" pitch:" + state->telemData->pitch  + " heading:" + state->telemData->heading );
			 mapView->SetAirplaneLocation( state->gpsData->gpsLatitude, 
					 state->gpsData->gpsLongitude, 
					 state->telemData->heading );
			 gimbalHUDView->setGimbalPosition( thePlaneWatcher->rawToDegrees(state->gimbalInfo->roll),
				 thePlaneWatcher->rawToDegrees(state->gimbalInfo->pitch));
			//delete state;

			 // TESTING: write plane location to file

				//http://localhost/Test.kml

				String ^ file = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"+
					"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"+
					"<Document>\n"+
					"<Style id=\"plane\">\n"+
					"<IconStyle>\n"+
					"<heading>"+Single(state->telemData->heading).ToString("F")+"</heading>\n"+
					"<Icon>\n"+
					"<href>http://localhost/falco.png</href>\n"+
					"<scale>1.0</scale>\n"+
					"</Icon>\n"+   
					"</IconStyle>\n"+
					"</Style>\n"+
					"<Placemark>\n"+
					"<styleUrl>#plane</styleUrl>\n"+
					"<name>Falco @ "+System::DateTime::Now.Second+"</name>\n"+
					"<description>Attached to the ground. Intelligently places itself at the height of the underlying terrain.</description>\n"+
					"<Point>\n"+
					"<altitudeMode>absolute</altitudeMode>\n"+
					"<coordinates>"+Single(state->gpsData->gpsLongitude).ToString("######.#######")+
					","+Single(state->gpsData->gpsLatitude).ToString("######.#######")+
					","+Single(state->gpsData->gpsAltitude).ToString("F")+"</coordinates>\n"+
					"</Point>\n"+
					"</Placemark>\n"+
					"</Document>"+
					"</kml>\n";
				StreamWriter^ outfile = gcnew StreamWriter("C:\\xampp\\htdocs\\1.kml");
				outfile->Write(file);
				outfile->Close();

		}

public: System::Void updateGimbalInfo( Communications::GimbalInfo ^ data ) {
			//System::Diagnostics::Trace::WriteLine("Form1::updateGimbalInfo() BLAHBLAHBLAH");
			// save data for later
			thePlaneWatcher->updateGimbalInfo(data);



		}

public: System::Void updatePlaneGPSInfo( Communications::PlaneGPSPacket ^ data ) {
			thePlaneWatcher->updatePlaneGPSInfo(data);

			//TODO: update the table the user sees in el GUI
}

public: System::Void updatePlaneTelemInfo( Communications::PlaneTelemPacket ^ data ) {
			thePlaneWatcher->updatePlaneTelemInfo(data);

			//TODO: update the table the user sees in el GUI
}

public: System::Void updateComData( Communications::ComportDownstream * data ) {
			// TODO: split this function into many others

			/* if( !isReadingData )
			 {
			 	this->Invoke( this->comportEstablishDelegate );
			 }

			 // Airplane metadata
			 this->metadataTable[1, A_ALT]->Value = data->gps_alt;
			 this->metadataTable[1, A_LAT]->Value = data->gps_lat;
			 this->metadataTable[1, A_LON]->Value = data->gps_lon;
			 this->metadataTable[1, A_HEAD]->Value = data->airplane_heading * 180.0 / 3.14159;
			 this->metadataTable[1, A_ROLL]->Value = data->airplane_roll  * 180.0 / 3.14159;
			 this->metadataTable[1, A_PITCH]->Value = data->airplane_pitch  * 180.0 / 3.14159;

			 // Gimbal stuff
			 float azimuth = data->gimbal_azimuth % 1600;
			 azimuth *= 360.0f / 1600.0f;
			 azimuth *= -1.0f;
			 if( azimuth > 180.0f )
				 azimuth = (azimuth - 360.0f);
			 if( azimuth < -180.0f )
				 azimuth = (azimuth + 360.0f);

			 float elevation = data->gimbal_elevation % 1600;
			 elevation *= 360.0f / 1600.0f;
			 //elevation -= 180.0f;

			 this->metadataTable[1, G_AZIM]->Value = data->gimbal_roll; //azimuth;
			 this->metadataTable[1, G_ELEV]->Value = data->gimbal_pitch; //elevation;

			 // Camera Stuff
			 this->metadataTable[1, V_ZOOM]->Value = Communications::decodeZoomFloat( data->camera_zoom );			 

			 // Update local zoom
			 m_joystick->setZoom( Communications::decodeZoomInt( data->camera_zoom ) );
	
			 // Log data
			 try
			 {				 
				DateTime timeTemp = DateTime::Now;
				String ^ text = timeTemp.ToString("o")->Replace(":", "-") + " " + 
					this->metadataTable[1, A_ALT]->Value + " " +
					this->metadataTable[1, A_LAT]->Value + " " +
					this->metadataTable[1, A_LON]->Value + " " +
					this->metadataTable[1, A_HEAD]->Value + " " +
					this->metadataTable[1, A_ROLL]->Value + " " +
					this->metadataTable[1, A_PITCH]->Value + " " +
					azimuth + " " +
					elevation + " " +
					this->metadataTable[1, V_ZOOM]->Value + "\n";
				 AddText( logFile, text );			 	
			 }
			 catch( Exception ^ )
			 {
				System::Diagnostics::Trace::WriteLine("catch in comport");
			 }

			 delete data;*/
		 }
private: System::Void splitLengthTextBox_Validated(System::Object^  sender, System::EventArgs^  e) {
			 try
			 {
				 int temp = Convert::ToInt32( this->splitLengthTextBox->Text );

				 if( temp < 0 )
					 throw gcnew Exception();

				 splitLength = temp;
				 
				 consoleMessage( "Video split length changed to " + splitLength, Color::Gray );			
			 }
			 catch( Exception ^ )
			 {
				 this->splitLengthTextBox->Text =  Convert::ToString( splitLength );
			 }
		 }

private: System::Void encodingComboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 
			// openGLView->changeEncoding( this->encodingComboBox->Text );
			// consoleMessage( "Video encoding changed to " + this->encodingComboBox->Text, Color::Gray );			
		 
		 }
private: System::Void startRecordButton_Click(System::Object^  sender, System::EventArgs^  e) {
		 	
			 // stop recording
			 if( recording ) {

				theSimHandler->endRecording();
				consoleMessage( "Stopped recording video and telemetry", Color::Green );	
				recording = false;
				System::Diagnostics::Trace::WriteLine("stopped recording in Form1");

				// update button
				((Windows::Forms::ButtonBase ^)sender)->Text = "Start Recording";
				
				pictureBox1->Image = dynamic_cast<Image^>(redImage);
			}

			// start recording
			else {
				DateTime time = DateTime::Now;
				String ^ path;//vidOptOutputDirText->Text + "\\video_" + time.ToString("o")->Replace(":", "-") + fileExtensionVideo;
			
				path = "D:\\Skynet Files\\video\\video_" + time.ToString("o")->Replace(":", "-");// + fileExtensionVideo;

				//recordStart = DateTime::Now;
				//openGLView->enableVideoRecording( path );
				// START TELEMETRY HERE
				if (!theSimHandler->beginRecording()) {
					// something went wrong
					consoleMessage( "Failed to Start Recording", Color::Red );

				}

				else {
					// recording began
					consoleMessage( "Started recording-telemetry", Color::Green );	
					recording = true;
					System::Diagnostics::Trace::WriteLine("started recording in Form1 " + path);
			
					
					// update button
					((Windows::Forms::ButtonBase ^)sender)->Text = "Stop Recording";
					pictureBox1->Image = dynamic_cast<Image^>(greenImage);
				}
			}
		 }
private: System::Void stopRecordButton_Click(System::Object^  sender, System::EventArgs^  e) {
		 	if( !recording )
				return;

			// no longer used (moved to startRecording)
		 }


private: System::Void videoSaveTimer_Tick(System::Object^  sender, System::EventArgs^  e) {
		// REMOVED
		return;
		


		if( !recording )
			return;

		DateTime time = DateTime::Now;
		time = time.AddSeconds( -Convert::ToInt32( SPLIT_LENGTH ) );

		if( DateTime::Compare( recordStart, time ) < 0 )
		{
			//recordStart = DateTime::Now;
			
			theSimHandler->beginRecording();
			consoleMessage( "Splitting video file", Color::Gray );	
			System::Diagnostics::Trace::WriteLine("split recording in Form1 ");
		}
	}
private: System::Void openGLPanel_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
		 }



private: System::Void mapnikToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 mapView->SetTileProvider( Mapnik );
			 mapView->SetCacheDirectory( defaultMapCache + "\\Mapnik", false );
			 mapnikToolStripMenuItem->Checked = true;
			 osmarenderToolStripMenuItem->Checked = false;
			 cyclemapToolStripMenuItem->Checked = false;
		 }

private: System::Void osmarenderToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 mapView->SetTileProvider( Osmarender );
			 mapView->SetCacheDirectory( defaultMapCache + "\\Osmarender", false );
			 mapnikToolStripMenuItem->Checked = false;
			 osmarenderToolStripMenuItem->Checked = true;
			 cyclemapToolStripMenuItem->Checked = false;
		 }

private: System::Void cyclemapToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 mapView->SetTileProvider( Cyclemap );
			 mapView->SetCacheDirectory( defaultMapCache + "\\Cyclemap", false );
			 mapnikToolStripMenuItem->Checked = false;
			 osmarenderToolStripMenuItem->Checked = false;
			 cyclemapToolStripMenuItem->Checked = true;
		 }

private: System::Void mapView_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			String ^ gps = mapView->GetPositionString( e->X, e->Y );
			mapMouseGPSLabel->Text = "Mouse GPS: " + gps;
		 }
private: System::Void dataGridView2_CurrentCellDirtyStateChanged(System::Object^  sender, System::EventArgs^  e) {
			static bool letterChanging = false;
			static bool boxChanges = false;
			String ^ field;
			String ^ value;
			String ^ rowID;

			DataGridView ^ table = dynamic_cast<DataGridView ^>(sender);
			DataGridViewCell ^ curCell = table->CurrentCell;
			System::Diagnostics::Trace::WriteLine(table->CurrentCell);

			if( letterChanging && curCell->ColumnIndex == 5 ) // handle letter
			{
				letterChanging = false;

				// Commit changes
				table->CommitEdit( DataGridViewDataErrorContexts::Commit );
				field = "letter";
				value = "" + curCell->Value;
				rowID = "" + curCell->OwningRow->Cells[0]->Value;

				//candidate
				if(table == dataGridView1)
				{
					db->updateValue( Database::tableCandidateRegions, field, value, rowID );
				}

				//unconfirmed
				else if (table == dataGridView2)
				{
					db->updateValue( Database::tableUnverifiedTargets, field, value, rowID );
				}

				//confirmed
				else if (table == dataGridView3)
				{
					db->updateValue( Database::tableVerifiedTargets, field, value, rowID );
				}
			}
			else if( curCell->ColumnIndex > 5 && curCell->ColumnIndex < 11  && !boxChanges ) // combo boxes and check boxes
			{
				
				switch( curCell->ColumnIndex )
				{
				case 6: // shape combo box
					field = "shape";
					break;
				case 7: // fg combo box
					field = "fg_color";
					break;
				case 8: // bg combo box
					field = "bg_color";
					break;
				case 9: // do nothing for this one
					break;
				case 10: // verified check box
					field = "verified";
					break;

				}

				boxChanges = true;

				// Commit changes
				table->CommitEdit( DataGridViewDataErrorContexts::Commit );
				value = "" + curCell->Value;
				rowID = "" + curCell->OwningRow->Cells[0]->Value;

				//table is candidate regions
				if (table == dataGridView1)
				{
					db->updateValue( Database::tableCandidateRegions, field, value, rowID );

					//move from candidate to confirmed if 'verified' is checked
					if(field == "verified")
					{
						DataGridViewRow ^ row = curCell->OwningRow;
						table->Rows->Remove(row);
					 	dataGridView3->Rows->Add(row);
						db->move( Database::tableUnverifiedTargets, Database::tableVerifiedTargets, value );					
					}
				}

				//table is unverified targets
				else if (table == dataGridView2)
				{
					db->updateValue( Database::tableUnverifiedTargets, field, value, rowID );

					//move from unconfirmed to confirmed if 'verified' is checked
					if(field == "verified")
					{
						DataGridViewRow ^ row = curCell->OwningRow;

						// Add to map
						mapView->AddTarget( Convert::ToDouble( row->Cells[2]->Value ), // lat
											Convert::ToDouble( row->Cells[3]->Value ), // lon
											Convert::ToInt32( row->Cells[0]->Value ) ); // id

						table->Rows->Remove(row);
				 		dataGridView3->Rows->Add(row);
						db->move( Database::tableUnverifiedTargets, Database::tableVerifiedTargets, value );					
					}
				}

				//table is verified targets
				else if (table == dataGridView3)
				{
					db->updateValue( Database::tableVerifiedTargets, field, value, rowID );

					//move from confirmed to unconfirmed if 'verified' is unchecked
					if(field == "verified")
					{
						DataGridViewRow ^ row = curCell->OwningRow;

						// Remove from map
						mapView->RemoveTarget( Convert::ToInt32( row->Cells[0]->Value ) ); // id

						table->Rows->Remove(row);
				 		dataGridView2->Rows->Add(row);
						db->move( Database::tableVerifiedTargets, Database::tableUnverifiedTargets, value );						
					}
				}
			}
			else
			{
				letterChanging = true;
				boxChanges = false;
			}
		 }
private: System::Void resetToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 appController->clearAllTables();

			 this->dataGridView1->Rows->Clear();
			 this->dataGridView2->Rows->Clear();
			 this->dataGridView3->Rows->Clear();
		 }

private: System::Void exportDataToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			 Stream^ myStream;
			 SaveFileDialog^ saveFileDialog1 = gcnew SaveFileDialog;
			 saveFileDialog1->Filter = "txt files (*.txt)|*.txt";
		     saveFileDialog1->FilterIndex = 2;
		     saveFileDialog1->RestoreDirectory = true;

			if ( saveFileDialog1->ShowDialog() == ::DialogResult::OK )
			{
				if ( (myStream = saveFileDialog1->OpenFile()) != nullptr )
				{					
					// File export here
					DataGridViewRowCollection ^ Rows = dataGridView3->Rows;
					for( int i = 0; i < Rows->Count; ++i )
					{
						DataGridViewCellCollection ^ curRow = Rows[i]->Cells;

						String ^ outputStr = "";
						
						// Format target number
						outputStr += String::Format( "{0:00}", i ) + " ";						

						// Format Lat
						double dLatitude = Convert::ToDouble(curRow[2]->Value);
						double dAbsLatitude = fabs(dLatitude);
					    int nLatitudeD = static_cast<int>(dAbsLatitude);
					    int nLatitudeM = static_cast<int>((dAbsLatitude - nLatitudeD) * 60);
					    int nLatitudeS = static_cast<int>((dAbsLatitude - nLatitudeD - nLatitudeM/60.0) * 3600);
						String ^ pszLatitudeType = (dLatitude > 0) ? "N" : "S";

						outputStr += pszLatitudeType + nLatitudeD + " " + nLatitudeM + " " + nLatitudeS + " ";

						// Format lon
						double dLongitude = Convert::ToDouble(curRow[3]->Value);
					    double dAbsLongitude = fabs(dLongitude);
					    int nLongitudeD = static_cast<int>(dAbsLongitude);
					    int nLongitudeM = static_cast<int>((dAbsLongitude - nLongitudeD) * 60);
					    int nLongitudeS = static_cast<int>((dAbsLongitude - nLongitudeD - nLongitudeM/60.0) * 3600);
			    		String ^ pszLongitudeType = (dLongitude > 0) ? "E" : "W";

						outputStr += pszLongitudeType + nLongitudeD + " " + nLongitudeM + " " + nLongitudeS + " ";

						// Format heading
						outputStr += TargetDialog::getHeadingString( Convert::ToDouble(curRow[4]->Value) );

						// Shape
						outputStr += curRow[6]->Value + " ";

						// Shape color
						outputStr += curRow[8]->Value + " ";

						// Letter
						outputStr += curRow[5]->Value + " ";

						// Letter color
						outputStr += curRow[7]->Value + " ";

						// Path (pull this from the database )
						String ^ path = db->getField(Database::tableVerifiedTargets, 1, Convert::ToString(curRow[0]->Value));
						String ^ destPath = "D:\\Skynet Files\\Turnin\\" + "image_" + i + ".jpg";
						System::IO::File::Copy( path, destPath, true );

						outputStr += "image_" + i + ".jpg";

						// Newline
						outputStr += "\n";						
						AddText( myStream, outputStr );
					}					
					
					// Code to write the stream goes here.
					myStream->Close();
					consoleMessage( "Data exported to " + saveFileDialog1->FileName, Color::Gray );	
				}
			}

			 


		 }
private: System::Void mapUpdateTimer_Tick(System::Object^  sender, System::EventArgs^  e) {	

			 // REMOVED

			 return;

			 // Update Map position of airplane
			 if( isReadingData )
			 {
				
				System::Diagnostics::Trace::WriteLine("Set new airplane location: " + Convert::ToDouble(this->metadataTable[1, A_LAT]->Value) + ", " + Convert::ToDouble(this->metadataTable[1, A_LON]->Value));
				 
				/* array<float> ^ homography = GeoReference::computeHomography( Convert::ToDouble(this->metadataTable[1, A_LAT]->Value),
					 Convert::ToDouble(this->metadataTable[1, A_LON]->Value),
					 Convert::ToDouble(this->metadataTable[1, A_LAT]->Value),
					 Convert::ToDouble(this->metadataTable[1, A_ROLL]->Value),
					 Convert::ToDouble(this->metadataTable[1, A_PITCH]->Value),
					 Convert::ToDouble(this->metadataTable[1, A_HEAD]->Value),
					 Convert::ToDouble(this->metadataTable[1, G_AZIM]->Value),
					 Convert::ToDouble(this->metadataTable[1, G_ELEV]->Value),
					 Convert::ToDouble(this->metadataTable[1, V_ZOOM]->Value) );*/

				 /*mapView->SetCameraLookLocation( homography, 
					 Convert::ToDouble(this->metadataTable[1, A_LAT]->Value), 
					 Convert::ToDouble(this->metadataTable[1, A_LON]->Value) );*/
			 }
		 }

private: System::Void lockToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if( lockToolStripMenuItem->Checked )
			 {
				 mapView->SetUILocked( false );
				 lockToolStripMenuItem->Checked = false;
			 }
			 else
			 {
				mapView->SetUILocked( true );
			 	lockToolStripMenuItem->Checked = true;
			 }
				

		 }
private: System::Void downloadToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if( downloadToolStripMenuItem->Checked )
			 {
				 mapView->SetDownload( false );
				 downloadToolStripMenuItem->Checked = false;
			 }
			 else
			 {
				mapView->SetDownload( true );
			 	downloadToolStripMenuItem->Checked = true;
			 }
		 }
private: System::Void mapLookGPSToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				double lat, lon;
				double north = 0.0f, east = 0.0f;
				//char zone[4];
				mapView->GetPosition(mouseGPS.X, mouseGPS.Y, lon, lat);


				// fix georeferencing
				//GeoReference::LLtoUTM(23, lat, lon, north, east, zone);


				Communications::ComportUpstream * packet = new Communications::ComportUpstream();
				packet->gps_lat_gimbal_x.f = (float)north;
				packet->gps_lon_gimbal_y.f = (float)east;
				packet->update_type = 0x0B;

				//for( int i = 0; i < 100; ++i ) //theComport->writeData(packet); // TODO: send data correctly

				delete packet;

		 }
private: System::Void mapView_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 	if( e->Button != ::MouseButtons::Right )
					return;

				mouseGPS = e->Location;
				mapMenuStrip->Show( mapView, mouseGPS );


		 }
		 
private: System::Void dataGridView1_CellDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
			 // if( e->ColumnIndex < 1 || e->ColumnIndex > 4 )
			//	 return;

			 if( e->RowIndex < 0 )
				 return;

			 String ^ id = Convert::ToString(dataGridView1->Rows[e->RowIndex]->Cells[0]->Value);
			 
			 editCandidateUsingDialog(appController->candidateWithID(id));

		 }

		 
private: System::Void dataGridView2_CellDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
			 //if( e->ColumnIndex < 1 || e->ColumnIndex > 4 )
			//	 return;

			 if( e->RowIndex < 0 )
				 return;

			 String ^ id = Convert::ToString(dataGridView2->Rows[e->RowIndex]->Cells[0]->Value);
			 
			 editUnverifiedTargetUsingDialog(appController->targetWithID(id));

		 }

private: System::Void dataGridView3_CellDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
			 if( e->ColumnIndex < 1 || e->ColumnIndex > 4 )
				 return;

			 if( e->RowIndex < 0 )
				 return;

			 String ^ id = Convert::ToString(dataGridView3->Rows[e->RowIndex]->Cells[0]->Value);
			 
			 System::Diagnostics::Trace::WriteLine("ERROR: dataGridView3_CellDoubleClick(): no methods in place to edit verified targets. tell developers to get back to work.");
		 }

public: System::Void editCandidateUsingDialog(Database::CandidateRowData ^ theData) 
		{
			 imageDialog = gcnew TargetDialog( this, appController );
			 
			 imageDialog->showDialogForData(theData);

			 //imageDialog->TargetID = Convert::ToInt32( dataGridView3->Rows[e->RowIndex]->Cells[0]->Value );
			 //imageDialog->RowID = e->RowIndex;
			 //imageDialog->ImagePath = db->getField(Database::tableVerifiedTargets, 1, id);
			 //imageDialog->Latitude = Convert::ToDouble( dataGridView3->Rows[e->RowIndex]->Cells[2]->Value );
			 //imageDialog->Longitude = Convert::ToDouble( dataGridView3->Rows[e->RowIndex]->Cells[3]->Value );
			 //imageDialog->Heading = Convert::ToDouble( dataGridView3->Rows[e->RowIndex]->Cells[4]->Value );
			 
		}

public: System::Void editUnverifiedTargetUsingDialog(Database::TargetRowData ^ data)
		{
			if (data == nullptr) {
				System::Diagnostics::Trace::WriteLine("ERROR: editUnverifiedTargetUsingDialog(): data == nullptr.");
				return;
			}
			imageDialog = gcnew TargetDialog( this, appController );
			 
			imageDialog->showDialogForData(data);

		}

public: System::Void editVerifiedTargetUsingDialog(Database::VerifiedTargetRowData ^ theData) 
		{
			// to get this working, change this function declaration and do other shit
			 System::Diagnostics::Trace::WriteLine("ERROR: editVerifiedTargetUsingDialog(): not yet implemented.");
		}

		 //public: Vision::ocrItem ^ getCandidateData( void )
		 //{
		 //	DataGridViewCellCollection ^ row;
			//try
			//{
			//	row = dataGridView1->Rows[0]->Cells;
			//}
			//catch( Exception ^ )
			//{
			//	// no data to take
			//	return nullptr;
			//}

			//// Set up return value
			//String ^ id = Convert::ToString( row[0]->Value );
			//Vision::ocrItem ^ retVal = gcnew Vision::ocrItem();			
			//retVal->_homography = db->getHomography( Database::tableCandidateRegions, id );
			//retVal->_row = dataGridView1->Rows[0];
			//retVal->_heading = Convert::ToDouble( row[4]->Value );
			//retVal->_path = db->getField(Database::tableCandidateRegions, 1, id);

			//// Remove row from this table
			//dataGridView1->Rows->Remove( dataGridView1->Rows[0] );

			////dataGridView3->Rows->Add(row);
			////db->move( Database::tableUnverifiedTargets, Database::tableVerifiedTargets, value );

			//return retVal;
		 //}

		 public: void ocrUpdateData( DataGridViewRow ^ row )
		 {
		 	DataGridViewCellCollection ^ cells = row->Cells;
			String ^ id = Convert::ToString( cells[0]->Value );
			
			// Update all values in old entry in database
			db->updateValue( Database::tableCandidateRegions, "shape", Convert::ToString( cells[COL_SHAPE]->Value ), id );
			db->updateValue( Database::tableCandidateRegions, "letter", Convert::ToString( cells[COL_LETTER]->Value ), id );
			db->updateValue( Database::tableCandidateRegions, "processed", "TRUE", id );

			// Move into unverified
			db->moveProcessed( Database::tableCandidateRegions, Database::tableUnverifiedTargets, "TRUE" );
			dataGridView2->Rows->Add(row);
		 }

		
		 public: void imageDialogCallback( int rowID, int tarID, double lat, double lon, double heading )
		 {
		 	DataGridViewCellCollection ^ row = dataGridView3->Rows[rowID]->Cells;

			// Update table
			row[2]->Value = Convert::ToString( lat );
			row[3]->Value = Convert::ToString( lon );
			row[4]->Value = Convert::ToString( heading );

			// Update database
			db->updateValue( Database::tableVerifiedTargets, "target_lat", Convert::ToString( lat ), Convert::ToString( tarID ) );
			db->updateValue( Database::tableVerifiedTargets, "target_lon", Convert::ToString( lon ), Convert::ToString( tarID ) );
			db->updateValue( Database::tableVerifiedTargets, "heading", Convert::ToString( heading ), Convert::ToString( tarID ) );

			// Update target in map
			mapView->AddTarget( lat, lon, tarID );
		 }
private: System::Void choosePathToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			 // ask user for filename
			String ^ temp;
		 
			System::Windows::Forms::DialogResult result = simReadVidDialog->ShowDialog();

			if ( result == ::DialogResult::OK )
			{
				temp = simReadVidDialog->FileName;
				
				theSimHandler->startPlayback(temp);

				//vidOptOutputDirText->Text = temp;
				System::Diagnostics::Trace::WriteLine( "Simulator input path changed to: " + temp);
			}

			// if user presses cancel, then DONT START VIDEO!
		 }
private: System::Void startToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 //theVideoSimulator->startVideo();
			 //callback->dontShow();

			 //THIS IS NEVER CALLED
		 }
private: System::Void pauseToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 
			 if (theSimHandler->isPaused()) {

				theSimHandler->resumePlayback();
				pauseToolStripMenuItem->Text = "Pause";

				//System::Diagnostics::Trace::WriteLine( "PAUSing AHHHHHH" );

			 } else {
				pauseToolStripMenuItem->Text = "Play";
				theSimHandler->pausePlayback();

			 }

		 }
private: System::Void stopToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 theSimHandler->endPlayback();
		 }
private: System::Void Form1_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			
			 System::Diagnostics::Trace::WriteLine( "form1 - keydown" );

			array<System::Byte> ^ buffer = nullptr;


			if (e->KeyCode == Keys::Q) {
				// send zoom in packet
				buffer = gcnew array<System::Byte>(6);
				buffer[0] = 0x81;
				buffer[0] = 0x01;
				buffer[0] = 0x04;
				buffer[0] = 0x07;
				buffer[0] = 0x24;
				buffer[0] = 0xFF;
				System::Diagnostics::Trace::WriteLine( "zoom in" );
			}

			else if (e->KeyCode == Keys::A) {
				// send stop zoom packet
				buffer = gcnew array<System::Byte>(6);
				buffer[0] = 0x81;
				buffer[0] = 0x01;
				buffer[0] = 0x04;
				buffer[0] = 0x07;
				buffer[0] = 0x00;
				buffer[0] = 0xFF;

			}

			else if (e->KeyCode == Keys::Z) {
				// send zoom out packet
				buffer = gcnew array<System::Byte>(6);
				buffer[0] = 0x81;
				buffer[0] = 0x01;
				buffer[0] = 0x04;
				buffer[0] = 0x07;
				buffer[0] = 0x34;
				buffer[0] = 0xFF;

			}

			//if (buffer != nullptr) //theComport->writeRawData(buffer); // TODO: fix this

		 }
private: System::Void Form1_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
			 System::Diagnostics::Trace::WriteLine( "form1 - keypress" );

		 }
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			/*array<System::Byte> ^ buffer = nullptr;


			// send zoom in packet
			buffer = gcnew array<System::Byte>(6);
			buffer[0] = 0x81;
			buffer[1] = 0x01;
			buffer[2] = 0x04;
			buffer[3] = 0x07;
			buffer[4] = 0x24;
			buffer[5] = 0xFF;
			System::Diagnostics::Trace::WriteLine( "zoom in" );
			

			if (buffer != nullptr)
				;//theComport->writeRawData(buffer); // TODO: fix this*/

			 //theComms->gotoLatLon(0,0);
			 theComms->sendHelloToRabbit();
		 }
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
			array<System::Byte> ^ buffer = nullptr;


			// send stop zoom packet
			buffer = gcnew array<System::Byte>(6);
			buffer[0] = 0x81;
			buffer[1] = 0x01;
			buffer[2] = 0x04;
			buffer[3] = 0x07;
			buffer[4] = 0x00;
			buffer[5] = 0xFF;


			//if (buffer != nullptr) //theComport->writeRawData(buffer); // TODO: fix this
		 }
private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
			array<System::Byte> ^ buffer = nullptr;


			// send zoom out packet
			buffer = gcnew array<System::Byte>(6);
			buffer[0] = 0x81;
			buffer[1] = 0x01;
			buffer[2] = 0x04;
			buffer[3] = 0x07;
			buffer[4] = 0x34;
			buffer[5] = 0xFF;

			

			//if (buffer != nullptr) //theComport->writeRawData(buffer); // TODO: fix this
		 }
private: System::Void Form1_Click(System::Object^  sender, System::EventArgs^  e) {
			 //System::Diagnostics::Trace::WriteLine( "click\nKEYPRESS\nKEYPRESS\nKEYPRESS\nKEYPRESS\nKEYPRESS\NKEYPRESS\nKEYPRESS\n" );
		 }
private: System::Void errorLogTextBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
			 System::Diagnostics::Trace::WriteLine( "errorlog - keypress" );
		 }
public:	 System::Void handleConnectionResult(array<Int32> ^ retArr) {

			int result = (int)retArr[0];
			isConnecting = false;

			/*if (result == RABBIT_CONNECTED) {
					consoleMessage( "Connected to Rabbit", Color::Green );
					pictureBox3->Image = dynamic_cast<Image^>(greenImage);
			} else if (result == AUTOPILOT_CONNECTED) {
					consoleMessage( "Failed to Connect to Rabbit", Color::Red );
					consoleMessage( "Connected to Autopilot", Color::Green );
					pictureBox2->Image = dynamic_cast<Image^>(greenImage);
					pictureBox3->Image = dynamic_cast<Image^>(redImage);
			}*/

			if (result == BOTH_FAILED) {
				// both failed to connect
					//consoleMessage( "Failed to Connect to Rabbit", Color::Red );
					consoleMessage( "Failed to Connect to Autopilot", Color::Red );
					pictureBox2->Image = dynamic_cast<Image^>(redImage);
					pictureBox3->Image = dynamic_cast<Image^>(redImage);

			}

			else {
				// recording began
				if (result == BOTH_CONNECTED) {
				//	consoleMessage( "Connected to Rabbit", Color::Green );
					consoleMessage( "Connected to Autopilot", Color::Green );
					pictureBox2->Image = dynamic_cast<Image^>(greenImage);
					pictureBox3->Image = dynamic_cast<Image^>(greenImage);
				}/* else if (result == RABBIT_CONNECTED) {
					consoleMessage( "Connected to Rabbit", Color::Green );
					consoleMessage( "Failed to Connect to Autopilot", Color::Red );
					pictureBox2->Image = dynamic_cast<Image^>(redImage);
					pictureBox3->Image = dynamic_cast<Image^>(greenImage);
				}*/ else if (result == AUTOPILOT_CONNECTED) {
					//consoleMessage( "Failed to Connect to Rabbit", Color::Red );
					consoleMessage( "Connected to Autopilot", Color::Green );
					pictureBox2->Image = dynamic_cast<Image^>(greenImage);
					pictureBox3->Image = dynamic_cast<Image^>(redImage);
				}

				isconnected = true;
				System::Diagnostics::Trace::WriteLine("Connected");
			
					
				// update button
				//this->button3 = (gcnew System::Windows::Forms::Button());
				this->button4->Text = "Disconnect";
				
			}



		 }
private: System::Void connectButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 // disconnect
			 if( isconnected ) {

				 
				theComms->disconnectAll();
				consoleMessage( "Disconnected from Comms", Color::Green );	
				isconnected = false;

				System::Diagnostics::Trace::WriteLine("Disconnected from Comms in Form1");
				
				// update button
				((Windows::Forms::ButtonBase ^)sender)->Text = "Connect";
				pictureBox2->Image = dynamic_cast<Image^>(redImage);
				pictureBox3->Image = dynamic_cast<Image^>(redImage);
			}

			// connect
			else {

				if (isConnecting)
					return;
				isConnecting = true;

				consoleMessage( "Connecting...", Color::Green );

				// spawn thread
				Thread ^ comConnectThread = gcnew Thread(gcnew ThreadStart(theComms, &Communications::Comms::connectAll));
				comConnectThread->Name = "Comms Connect Thread";
				comConnectThread->Start();

			}

		 }


private: System::Void dataGridView1_DataError(System::Object^  sender, System::Windows::Forms::DataGridViewDataErrorEventArgs^  e) {
			 //System::Diagnostics::Trace::WriteLine("Form1::dataGridView1_DataError: " + e);
		 }
private: System::Void dataGridView2_DataError(System::Object^  sender, System::Windows::Forms::DataGridViewDataErrorEventArgs^  e) {
			 //System::Diagnostics::Trace::WriteLine("Form1::dataGridView2_DataError: " + e);
		 }
private: System::Void dataGridView3_DataError(System::Object^  sender, System::Windows::Forms::DataGridViewDataErrorEventArgs^  e) {
			 //System::Diagnostics::Trace::WriteLine("Form1::dataGridView3_DataError: " + e);
		 }

};
}

