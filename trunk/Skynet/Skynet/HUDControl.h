#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#define SIDE 134.0f
#define SCALE (SIDE/180.0f)

#include "Delegates.h"

namespace Skynet {

	/// <summary>
	/// Summary for HUDControl
	/// </summary>
	public ref class HUDControl : public System::Windows::Forms::UserControl
	{
	public:
		HUDControl(void)
		{
			InitializeComponent();
			gimbalX = SIDE/2;
			gimbalY = SIDE/2;
			_redrawHUDdelegate = gcnew Delegates::voidToVoid( this, &HUDControl::redraw );
			bkgd = Image::FromFile( "C:\\Users\\UCSD\\Documents\\Visual Studio 2010\\Projects\\Skynet\\Skynet\\HUDbg.png" );
			dot = Image::FromFile( "C:\\Users\\UCSD\\Documents\\Visual Studio 2010\\Projects\\Skynet\\Skynet\\HUDdot.png" );

			this->BackgroundImage = bkgd;
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler( this, &HUDControl::paintHUD );
		}

		
		System::Void paintHUD(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
			 this->DrawHUD( e->Graphics );
		}

		void redraw()
		{
			this->Refresh();
		}

				//newX and newY are gotten by gimbal's center, no need to center it.
		bool setGimbalPosition( float newX, float newY)
		{
			gimbalX = (newX + 90.0f)*SCALE;
			gimbalY = (newY + 90.0f)*SCALE;
			this->Invoke( this->_redrawHUDdelegate );
			return true;
		}

	protected:
		float gimbalX;
		float gimbalY;
		Image ^ bkgd;
		Image ^ dot;
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~HUDControl()
		{
			if (components)
			{
				delete components;
			}
		}

		bool DrawHUD( Graphics ^ g )
		{
			bool retVal = false;
			int height = (int)SIDE;
			int width = (int)SIDE;

			//Next lets scale down the measure tile width to a most significant digit unit
			try
			{
				//g->DrawImage(bkgd, 0.0f, 0.0f, 134.0f, 134.0f);
				g->DrawImage(dot, gimbalX-17.0f, gimbalY-12.5f, 34.0f, 25.0f);
			}
			catch (Exception ^ e)
			{
				System::Diagnostics::Trace::WriteLine("Could not draw your background :(");
				e = nullptr;
				return retVal;
			}
			return true;
		}



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
		Delegates::voidToVoid ^ _redrawHUDdelegate; // WAS PUBLIC, MADE WARNINGS

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		}
#pragma endregion
	};
}
