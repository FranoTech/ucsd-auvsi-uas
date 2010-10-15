#pragma once

#include "GeoReference.h"

namespace Skynet {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for TargetDialog
	/// </summary>
	public ref class TargetDialog : public System::Windows::Forms::Form
	{
	public:
		TargetDialog( double centerLat, double centerLon, double latMap, double lonMap, array<float> ^ homo, Object ^ parent );
		//{
		//	_parent = parent;
		//	_centerLat = centerLat;
		//	_centerLon = centerLon;
		//	_latMap = latMap;
		//	_lonMap = lonMap;

		//	_markLat = false;
		//	_markHeading = false;
		//	InitializeComponent();
		//	//
		//	//TODO: Add the constructor code here
		//	//
		//}

		property int TargetID
		{
			int get()
			{
				return _targetID;
			}
			void set( int value )
			{
				_targetID = value;
				this->Text = "Image " + _targetID;
			}
		}

		property double Latitude
		{
			double get()
			{
				return _latitude;
			}
			void set( double value )
			{
				_latitude = value;
			}
		}

		property double Longitude
		{
			double get()
			{
				return _longitude;
			}
			void set( double value )
			{
				_longitude = value;
			}
		}	

		property double Heading
		{
			double get()
			{
				return _heading;
			}
			void set( double value )
			{
				_heading = value;
			}
		}	

		property int RowID
		{
			int get()
			{
				return _rowID;
			}
			void set( int value )
			{
				_rowID = value;
			}
		}

		property String ^ ImagePath
		{
			String ^ get()
			{
				return _path;
			}
			void set( String ^ value )
			{
				_path = value->Substring(0, value->IndexOf(".jpg"));
				try
				{
					_targetImage = Image::FromFile( _path + "_rectified.jpg" );
					imageBox->Image = _targetImage;
				}
				catch( Exception ^ )
				{
		System::Diagnostics::Trace::WriteLine("catch in target dialog");

				}
			}
		}

		static String ^ getHeadingString( double angle )
		{
			String ^ retVal;

			if( angle > -22.5 && angle <= 22.5 )
				retVal = "N";
			else if( angle > 22.5 && angle <= 67.5 )
				retVal = "NW";
			else if( angle > 67.5 && angle <= 112.5 )
				retVal = "W";
			else if( angle > 112.5 && angle <= 157.5 )
				retVal = "SW";
			else if( angle > 157.5 || angle < -157.5 )
				retVal = "S";
			else if( angle < -112.5 && angle >= -157.5 )
				retVal = "SE";
			else if( angle < -65.7 && angle >= -112.5 )
				retVal = "E";
			else
				retVal = "NE";

			return retVal;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~TargetDialog()
		{
			if (components)
			{
				delete components;
			}
		}

		double atan3( double x, double y )
		{
			double angle;

			if(x> 0 && y>0)
				angle = -Math::Atan(y/x);
			else if(x>0 && y<0)
			    angle = Math::Atan(-y/x);
			else if(x<0 && y<0)
				angle =   Math::PI/2.0 +Math::Atan(-x/-y);
			else if(x<0 && y>0)
			    angle = -Math::PI/2 -Math::Atan(-x/y);
			else if(x> 0 && y == 0)
			    angle = 0;
			else if(x<0 && y==0)
			    angle = Math::PI;
			else if(x==0 && y >0)
			    angle = -Math::PI/2.0;
			else if(x==0 && y<0)
			    angle = -Math::PI/2.0;
			else if(x==0 && y==0)
			    angle =0;

			return -angle;
		}
		
		void setHeading( int x, int y )
		{
			double angle = atan3( x, y ) * 180.0 / Math::PI;

			_heading = angle;
		}

	protected:
		int _targetID;
		int _rowID;
		String ^ _path;
		Image ^ _targetImage;
		double _latitude, _longitude;
		double _heading;

		double _centerLat, _centerLon;
		double _latMap, _lonMap;
		bool _markLat, _markHeading;
		Point _mouseDown;

		array<float> ^ _homography;

		Object ^ _parent;

	private: System::Windows::Forms::PictureBox^  imageBox;

	private: System::Windows::Forms::Button^  latlonButton;

	private: System::Windows::Forms::Button^  headingButton;

	private: System::Windows::Forms::Button^  okButton;
	private: System::Windows::Forms::Button^  cancelButton;
private: System::Windows::Forms::Label^  instructionLabel;


	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->imageBox = (gcnew System::Windows::Forms::PictureBox());
			this->latlonButton = (gcnew System::Windows::Forms::Button());
			this->headingButton = (gcnew System::Windows::Forms::Button());
			this->okButton = (gcnew System::Windows::Forms::Button());
			this->cancelButton = (gcnew System::Windows::Forms::Button());
			this->instructionLabel = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->imageBox))->BeginInit();
			this->SuspendLayout();
			// 
			// imageBox
			// 
			this->imageBox->Location = System::Drawing::Point(13, 12);
			this->imageBox->Name = L"imageBox";
			this->imageBox->Size = System::Drawing::Size(577, 448);
			this->imageBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->imageBox->TabIndex = 0;
			this->imageBox->TabStop = false;
			this->imageBox->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &TargetDialog::imageBox_MouseDown);
			this->imageBox->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &TargetDialog::imageBox_MouseUp);
			// 
			// latlonButton
			// 
			this->latlonButton->Location = System::Drawing::Point(13, 468);
			this->latlonButton->Name = L"latlonButton";
			this->latlonButton->Size = System::Drawing::Size(97, 23);
			this->latlonButton->TabIndex = 1;
			this->latlonButton->Text = L"Mark Lat/Lon";
			this->latlonButton->UseVisualStyleBackColor = true;
			this->latlonButton->Click += gcnew System::EventHandler(this, &TargetDialog::latlonButton_Click);
			// 
			// headingButton
			// 
			this->headingButton->Location = System::Drawing::Point(116, 468);
			this->headingButton->Name = L"headingButton";
			this->headingButton->Size = System::Drawing::Size(96, 23);
			this->headingButton->TabIndex = 2;
			this->headingButton->Text = L"Mark Heading";
			this->headingButton->UseVisualStyleBackColor = true;
			this->headingButton->Click += gcnew System::EventHandler(this, &TargetDialog::headingButton_Click);
			// 
			// okButton
			// 
			this->okButton->Location = System::Drawing::Point(434, 468);
			this->okButton->Name = L"okButton";
			this->okButton->Size = System::Drawing::Size(75, 23);
			this->okButton->TabIndex = 3;
			this->okButton->Text = L"OK";
			this->okButton->UseVisualStyleBackColor = true;
			this->okButton->Click += gcnew System::EventHandler(this, &TargetDialog::okButton_Click);
			// 
			// cancelButton
			// 
			this->cancelButton->Location = System::Drawing::Point(515, 468);
			this->cancelButton->Name = L"cancelButton";
			this->cancelButton->Size = System::Drawing::Size(75, 23);
			this->cancelButton->TabIndex = 4;
			this->cancelButton->Text = L"Cancel";
			this->cancelButton->UseVisualStyleBackColor = true;
			this->cancelButton->Click += gcnew System::EventHandler(this, &TargetDialog::cancelButton_Click);
			// 
			// instructionLabel
			// 
			this->instructionLabel->AutoSize = true;
			this->instructionLabel->Location = System::Drawing::Point(218, 473);
			this->instructionLabel->Name = L"instructionLabel";
			this->instructionLabel->Size = System::Drawing::Size(0, 13);
			this->instructionLabel->TabIndex = 5;
			// 
			// TargetDialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			this->ClientSize = System::Drawing::Size(602, 503);
			this->Controls->Add(this->instructionLabel);
			this->Controls->Add(this->cancelButton);
			this->Controls->Add(this->okButton);
			this->Controls->Add(this->headingButton);
			this->Controls->Add(this->latlonButton);
			this->Controls->Add(this->imageBox);
			this->MaximizeBox = false;
			this->Name = L"TargetDialog";
			this->Text = L"TargetDialog";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->imageBox))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

private: System::Void okButton_Click(System::Object^  sender, System::EventArgs^  e); 
		 //{
			// // enter the data in the callbacks

			// //((Skynet::Form1 ^)_parent)->Invoke( &Skynet::Form1::imageDialogDelegate );

		 //}
private: System::Void cancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->Close();
		 }
private: System::Void headingButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 	try
				{
					_targetImage = Image::FromFile( _path + "_rectified.jpg" );
					imageBox->Image = _targetImage;
				}
				catch( Exception ^ )
				{
				
		System::Diagnostics::Trace::WriteLine("catch in target dialog");
				}

			 _markHeading = true;
			 _markLat = false;
			 instructionLabel->Text = "Click and drag heading";
		 }
private: System::Void latlonButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 	try
				{
					_targetImage = Image::FromFile( _path + ".jpg" );
					imageBox->Image = _targetImage;
				}
				catch( Exception ^ )
				{

				}

			 _markHeading = false;
			 _markLat = true;
			 instructionLabel->Text = "Click to mark lat/lon";
		 }
private: System::Void imageBox_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {			 			 
			 //if( e->Button != ::MouseButtons::Left )
				// return;

			 if( _markHeading )
				 _mouseDown = e->Location;
			 else if( _markLat )
			 {
				 double latReturn, lonReturn;
				 GeoReference::applyHomography( _homography, e->Location.X, e->Location.Y, _latMap, _lonMap, latReturn, lonReturn );

				 _latitude = latReturn;
				 _longitude = lonReturn;
				 
				 instructionLabel->Text = "Lat: " + _latitude + ", Lon: " + _longitude;

				 _markLat = false;
			 }
		 }
private: System::Void imageBox_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 //if( e->Button != ::MouseButtons::Left )
				// return;

			 if( _markHeading )
			 {
				 // Get pixel displacement from prior point
				 int deltaX, deltaY;
				 deltaX = e->Location.X - _mouseDown.X;
				 deltaY = e->Location.Y - _mouseDown.Y;

				 // adjust for North/East direction
				 deltaY *= -1;

				 // our atan3 function is written to expect X as the up/down axis, Y as the left/right, so pass in reverse order
				 setHeading( deltaY, deltaX );
				 instructionLabel->Text = "Heading set to " + getHeadingString( _heading );
				 
				 _markHeading = false;
			 }
		 }
};
}
