#include "StdAfx.h"

#define DIRECTINPUT_VERSION 0x0800
#define _CRT_SECURE_NO_DEPRECATE
#define JOYSTICK_PI 3.14159265358979323846264338327950288
#ifndef _WIN32_DCOM
#define _WIN32_DCOM
#endif

#include <windows.h>
#include <commctrl.h>
//#include <basetsd.h>
#include <dinput.h>
#include <dinputd.h>
//#include <assert.h>
//#include <oleauto.h>
//#include <shellapi.h>
#include <math.h>
#include <wbemidl.h>

#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )

#include "Form1.h"
#include "Joystick.h"

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

//button bindings
#define BTN_SQUARE 0
#define BTN_X 1
#define BTN_CIRCLE 2
#define BTN_TRIANGLE 3
#define BTN_L1 4
#define BTN_R1 5
#define BTN_L2 6
#define BTN_R2 7
#define BTN_SELECT 8
#define BTN_START 9
#define BTN_L3 10
#define BTN_R3 11


// Function decls
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
                                     VOID* pContext );
BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                   VOID* pContext );

bool IsXInputDevice( const GUID* pGuidProductFromDirectInput );


bool                    g_bFilterOutXinputDevices;
XINPUT_DEVICE_NODE*     g_pXInputDeviceList;

LPDIRECTINPUT8          g_pDI;
LPDIRECTINPUTDEVICE8    g_pJoystick;


static Communications::ComportUpstream * lastPacket;

Joystick::Joystick( Object ^ theParent )
{
	g_bFilterOutXinputDevices = false;
	g_pXInputDeviceList = NULL;

	g_pDI = NULL;
	g_pJoystick = NULL;
	tracker = gcnew array<int>(12);
	for(int i = 0; i < 12; i++)
	{
		tracker[i] = 0;
	}
	zoom_level = 1;

	parent = theParent;

	manualMode = false;
	manualModeCounter = 0;
	lastPacket = NULL;
}

Joystick::~Joystick(void)
{
	FreeDirectInput();
}

int
Joystick::getZoom(void)
{
	return zoom_level;
}

void
Joystick::setZoom( int level )
{
	if( level > 0 )
		zoom_level = level;
}

HRESULT
Joystick::init(HWND hDlg)
{
	HRESULT hr;

    // Register with the DirectInput subsystem and get a pointer
    // to a IDirectInput interface we can use.
    // Create a DInput object
    if( FAILED( hr = DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION,
                                         IID_IDirectInput8, ( VOID** )&g_pDI, NULL ) ) )
        return hr;

    if( g_bFilterOutXinputDevices )
        SetupForIsXInputDevice();

    DIJOYCONFIG PreferredJoyCfg = {0};
    DI_ENUM_CONTEXT enumContext;
    enumContext.pPreferredJoyCfg = &PreferredJoyCfg;
    enumContext.bPreferredJoyCfgValid = false;

    IDirectInputJoyConfig8* pJoyConfig = NULL;
    if( FAILED( hr = g_pDI->QueryInterface( IID_IDirectInputJoyConfig8, ( void** )&pJoyConfig ) ) )
        return hr;

    PreferredJoyCfg.dwSize = sizeof( PreferredJoyCfg );
    if( SUCCEEDED( pJoyConfig->GetConfig( 0, &PreferredJoyCfg, DIJC_GUIDINSTANCE ) ) ) // This function is expected to fail if no Joystick is attached
        enumContext.bPreferredJoyCfgValid = true;
    SAFE_RELEASE( pJoyConfig );

    // Look for a simple Joystick we can use for this sample program.
    if( FAILED( hr = g_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL,
                                         EnumJoysticksCallback,
                                         &enumContext, DIEDFL_ATTACHEDONLY ) ) )
        return hr;

    if( g_bFilterOutXinputDevices )
        CleanupForIsXInputDevice();

    // Make sure we got a Joystick
    if( NULL == g_pJoystick )
    {
        // joystick not found
		
        return S_OK;
    }

    // Set the data format to "simple Joystick" - a predefined data format 
    //
    // A data format specifies which controls on a device we are interested in,
    // and how they should be reported. This tells DInput that we will be
    // passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
	if( FAILED( hr = g_pJoystick->SetDataFormat( &c_dfDIJoystick2 ) ) )
	{
		System::Diagnostics::Trace::WriteLine("returned1");
        return hr;
	}

    // Set the cooperative level to let DInput know how this device should
    // interact with the system and with other DInput applications.
    /*if( FAILED( hr = g_pJoystick->SetCooperativeLevel( hDlg, DISCL_EXCLUSIVE |
                                                       DISCL_FOREGROUND ) ) )
	{
		System::Diagnostics::Trace::WriteLine("returned2");
        return hr;
	}*/

    // Enumerate the Joystick objects. The callback function enabled user
    // interface elements for objects that are found, and sets the min/max
    // values property for discovered axes.
    if( FAILED( hr = g_pJoystick->EnumObjects( EnumObjectsCallback, ( VOID* )hDlg, DIDFT_ALL ) ) )
	{
		System::Diagnostics::Trace::WriteLine("\nEnumCallback not called");	
        return hr;
	}

    return S_OK;
}

HRESULT 
Joystick::SetupForIsXInputDevice()
{
    IWbemServices* pIWbemServices = NULL;
    IEnumWbemClassObject* pEnumDevices = NULL;
    IWbemLocator* pIWbemLocator = NULL;
    IWbemClassObject* pDevices[20] = {0};
    BSTR bstrDeviceID = NULL;
    BSTR bstrClassName = NULL;
    BSTR bstrNamespace = NULL;
    DWORD uReturned = 0;
    bool bCleanupCOM = false;
    UINT iDevice = 0;
    VARIANT var;
    HRESULT hr;

    // CoInit if needed
    hr = CoInitialize( NULL );
    bCleanupCOM = SUCCEEDED( hr );

    // Create WMI
    hr = CoCreateInstance( __uuidof( WbemLocator ),
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           __uuidof( IWbemLocator ),
                           ( LPVOID* )&pIWbemLocator );
    if( FAILED( hr ) || pIWbemLocator == NULL )
        goto LCleanup;

    // Create BSTRs for WMI
    bstrNamespace = SysAllocString( L"\\\\.\\root\\cimv2" ); if( bstrNamespace == NULL ) goto LCleanup;
    bstrDeviceID = SysAllocString( L"DeviceID" );           if( bstrDeviceID == NULL )  goto LCleanup;
    bstrClassName = SysAllocString( L"Win32_PNPEntity" );    if( bstrClassName == NULL ) goto LCleanup;

    // Connect to WMI 
    hr = pIWbemLocator->ConnectServer( bstrNamespace, NULL, NULL, 0L,
                                       0L, NULL, NULL, &pIWbemServices );
    if( FAILED( hr ) || pIWbemServices == NULL )
        goto LCleanup;

    // Switch security level to IMPERSONATE
    CoSetProxyBlanket( pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                       RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0 );

    // Get list of Win32_PNPEntity devices
    hr = pIWbemServices->CreateInstanceEnum( bstrClassName, 0, NULL, &pEnumDevices );
    if( FAILED( hr ) || pEnumDevices == NULL )
        goto LCleanup;

    // Loop over all devices
    for(; ; )
    {
        // Get 20 at a time
        hr = pEnumDevices->Next( 10000, 20, pDevices, &uReturned );
        if( FAILED( hr ) )
            goto LCleanup;
        if( uReturned == 0 )
            break;

        for( iDevice = 0; iDevice < uReturned; iDevice++ )
        {
            // For each device, get its device ID
            hr = pDevices[iDevice]->Get( bstrDeviceID, 0L, &var, NULL, NULL );
            if( SUCCEEDED( hr ) && var.vt == VT_BSTR && var.bstrVal != NULL )
            {
                // Check if the device ID contains "IG_".  If it does, then it�s an XInput device
                // Unfortunately this information can not be found by just using DirectInput 
                if( wcsstr( var.bstrVal, L"IG_" ) )
                {
                    // If it does, then get the VID/PID from var.bstrVal
                    DWORD dwPid = 0, dwVid = 0;
                    WCHAR* strVid = wcsstr( var.bstrVal, L"VID_" );
                    if( strVid && swscanf_s( strVid, L"VID_%4X", &dwVid ) != 1 )
                        dwVid = 0;
                    WCHAR* strPid = wcsstr( var.bstrVal, L"PID_" );
                    if( strPid && swscanf_s( strPid, L"PID_%4X", &dwPid ) != 1 )
                        dwPid = 0;

                    DWORD dwVidPid = MAKELONG( dwVid, dwPid );

                    // Add the VID/PID to a linked list
                    XINPUT_DEVICE_NODE* pNewNode = new XINPUT_DEVICE_NODE;
                    if( pNewNode )
                    {
                        pNewNode->dwVidPid = dwVidPid;
                        pNewNode->pNext = g_pXInputDeviceList;
                        g_pXInputDeviceList = pNewNode;
                    }
                }
            }
            SAFE_RELEASE( pDevices[iDevice] );
        }
    }

LCleanup:
    if( bstrNamespace )
        SysFreeString( bstrNamespace );
    if( bstrDeviceID )
        SysFreeString( bstrDeviceID );
    if( bstrClassName )
        SysFreeString( bstrClassName );
    for( iDevice = 0; iDevice < 20; iDevice++ )
    SAFE_RELEASE( pDevices[iDevice] );
    SAFE_RELEASE( pEnumDevices );
    SAFE_RELEASE( pIWbemLocator );
    SAFE_RELEASE( pIWbemServices );

    return hr;
}

//-----------------------------------------------------------------------------
// Returns true if the DirectInput device is also an XInput device.
// Call SetupForIsXInputDevice() before, and CleanupForIsXInputDevice() after
//-----------------------------------------------------------------------------
bool 
IsXInputDevice( const GUID* pGuidProductFromDirectInput )
{
    // Check each xinput device to see if this device's vid/pid matches
    XINPUT_DEVICE_NODE* pNode = g_pXInputDeviceList;
    while( pNode )
    {
        if( pNode->dwVidPid == pGuidProductFromDirectInput->Data1 )
            return true;
        pNode = pNode->pNext;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name: EnumJoysticksCallback()
// Desc: Called once for each enumerated Joystick. If we find one, create a
//       device interface on it so we can play with it.
//-----------------------------------------------------------------------------
BOOL CALLBACK 
EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
                                     VOID* pContext )
{
    DI_ENUM_CONTEXT* pEnumContext = ( DI_ENUM_CONTEXT* )pContext;
    HRESULT hr;

    if( g_bFilterOutXinputDevices && IsXInputDevice( &pdidInstance->guidProduct ) )
        return DIENUM_CONTINUE;

    // Skip anything other than the perferred Joystick device as defined by the control panel.  
    // Instead you could store all the enumerated Joysticks and let the user pick.
    if( pEnumContext->bPreferredJoyCfgValid &&
        !IsEqualGUID( pdidInstance->guidInstance, pEnumContext->pPreferredJoyCfg->guidInstance ) )
        return DIENUM_CONTINUE;

    // Obtain an interface to the enumerated Joystick.
    hr = g_pDI->CreateDevice( pdidInstance->guidInstance, &g_pJoystick, NULL );

    // If it failed, then we can't use this Joystick. (Maybe the user unplugged
    // it while we were in the middle of enumerating it.)
    if( FAILED( hr ) )
        return DIENUM_CONTINUE;

    // Stop enumeration. Note: we're just taking the first Joystick we get. You
    // could store all the enumerated Joysticks and let the user pick.
    return DIENUM_STOP;
}

//-----------------------------------------------------------------------------
// Name: EnumObjectsCallback()
// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a 
//       Joystick. This function enables user interface elements for objects
//       that are found to exist, and scales axes min/max values.
//-----------------------------------------------------------------------------
BOOL CALLBACK 
EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                   VOID* pContext )
{
    HWND hDlg = ( HWND )pContext;

    static int nSliderCount = 0;  // Number of returned slider controls
    static int nPOVCount = 0;     // Number of returned POV controls

    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if( pdidoi->dwType & DIDFT_AXIS )
    {
        DIPROPRANGE diprg;
        diprg.diph.dwSize = sizeof( DIPROPRANGE );
        diprg.diph.dwHeaderSize = sizeof( DIPROPHEADER );
        diprg.diph.dwHow = DIPH_BYID;
        diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
        diprg.lMin = -1000;
        diprg.lMax = +1000;

        // Set the range for the axis
        if( FAILED( g_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
            return DIENUM_STOP;
    }

	//System::Diagnostics::Trace::WriteLine("EnumObjectsCallback\n");
    return DIENUM_CONTINUE;
}

//-----------------------------------------------------------------------------
// Name: FreeDirectInput()
// Desc: Initialize the DirectInput variables.
//-----------------------------------------------------------------------------
VOID 
Joystick::FreeDirectInput()
{
    // Unacquire the device one last time just in case 
    // the app tried to exit while the device is still acquired.
    if( g_pJoystick )
        g_pJoystick->Unacquire();

    // Release any DirectInput objects.
    SAFE_RELEASE( g_pJoystick );
    SAFE_RELEASE( g_pDI );
}

HRESULT 
Joystick::UpdateInputState( HWND hDlg )
{
    HRESULT hr;
    TCHAR strText[512] = {0}; // Device state text
    DIJOYSTATE2 js;           // DInput Joystick state 
	JOY_STATUS joyie;
	bool change = false;			  // flag for any changes

    if( NULL == g_pJoystick )
        return S_OK;

    // Poll the device to read the current state
    hr = g_pJoystick->Poll();
    if( FAILED( hr ) )
    {
        // DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        hr = g_pJoystick->Acquire();
        while( hr == DIERR_INPUTLOST )
            hr = g_pJoystick->Acquire();

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return S_OK;
    }

    // Get the input's device state
    if( FAILED( hr = g_pJoystick->GetDeviceState( sizeof( DIJOYSTATE2 ), &js ) ) )
        return hr; // The device should have been acquired during the Poll()
	
	Communications::ComportUpstream * packet = new Communications::ComportUpstream();

	/*********************
	 *	Non COM responses
	 *********************/	
	 // Button 1 = X = save picture
	if( js.rgbButtons[BTN_X] & 0x80 )
	{
		tracker[BTN_X]++;
	}
	else
	{
		tracker[BTN_X] = 0;
	}

	if( tracker[BTN_X] == 1 )
	{
		//((Skynet::Form1 ^)parent)->saveImage();
		((Skynet::Form1 ^)parent)->Invoke( ((Skynet::Form1 ^)parent)->saveImageDelegate );

		
	}


	/******************
	 *	COM responses (set change == TRUE to send packet)
	 ******************/		
	//Calculate JOY_STATUS things.
	joyie.thresholds = 1;
	int exponential = 1;
	double localX, localY;

	//null and rest noise
	if(js.lY > 200  || js.lX > 200 || js.lY < -200 || js.lX < -200){
		manualMode = true;
		manualModeCounter = 0;	
		packet->update_type = 0x0A;
		
		localX = js.lX;
		localY = js.lY;

		//System::Diagnostics::Trace::WriteLine("X (localX) = " + localX + "\n");
		//System::Diagnostics::Trace::WriteLine("Y (localY) = " + localY + "\n");

		double thresholdRange = 1000.00/joyie.thresholds;
		localX = js.lX / thresholdRange;
		localY = js.lY / thresholdRange;

		//System::Diagnostics::Trace::WriteLine("X (localX) = " + localX + "\n");
		//System::Diagnostics::Trace::WriteLine("Y (localY) = " + localY + "\n");

		change = true;

		
		if(exponential){
			if(localX < 0) {
				localX = -( pow(2.0, (-localX)));
				localX += 1;
			}
			else{			
				localX =  pow(2.0, localX);
				localX -= 1;
			}
			if(localY < 0) {
				localY = -( pow(2.0, (-localY)));
				localY += 1;
			}
			else{
				localY = pow(2.0, localY);
				localY -= 1;
			}

			localX = localX*50;
			localY = localY*50;
		}

		//System::Diagnostics::Trace::WriteLine("X (localX) = " + localX + "\n");
		//System::Diagnostics::Trace::WriteLine("Y (localY) = " + localY + "\n");

	joyie.stickAngle = atan((float) (-js.lY)/js.lX)* 180.0 / JOYSTICK_PI;
	//ANGLE CALCULATIONS
		//1st quadrant conversion
		if(js.lY <= 0 && js.lX > 0)
		{
			joyie.stickAngle = 90.0 - joyie.stickAngle;
		}
		//2nd quadrant
		else if (js.lY < 0 && js.lX < 0)
		{
			joyie.stickAngle = 360 - (joyie.stickAngle + 90);
		}
		//3rd quad
		else if (js.lY >= 0 && js.lX < 0)
		{
			joyie.stickAngle = 270 -joyie.stickAngle;
		}
		//4th quad
		else if (js.lY > 0 && js.lX >= 0)
		{
			joyie.stickAngle = (-joyie.stickAngle) + 90;
		}
		else
			joyie.stickAngle = 0.0;
	}
	else
	{
		localX = 0.0;
		localY = 0.0;
		joyie.stickAngle = 0.0;
	}
	
	//System::Diagnostics::Trace::WriteLine("X (localX) = " + localX + "\n");
	//System::Diagnostics::Trace::WriteLine("Y (localY) = " + localY + "\n");
	
	

	packet->gps_lat_gimbal_x.i = (int)localX;
	packet->gps_lon_gimbal_y.i = (int)localY;
	
	if( js.rgbButtons[BTN_SQUARE] & 0x80 )
	{
		tracker[BTN_SQUARE]++;
	}
	else
	{
		tracker[BTN_SQUARE] = 0;
	}

	if( tracker[BTN_SQUARE] == 1 )
	{
		change = true;
		packet->update_type = 0x0E;
	}

	if( js.rgbButtons[BTN_CIRCLE] & 0x80 )
	{
		tracker[BTN_CIRCLE]++;
	}
	else
	{
		tracker[BTN_CIRCLE] = 0;
	}

	if( tracker[BTN_CIRCLE] == 1 )
	{
		change = true;
		packet->update_type = 0x0C;
	}


	//poll L1
	if( js.rgbButtons[BTN_L1] & 0x80 )
	{
		tracker[BTN_L1]++;
	}
	else
	{
		tracker[BTN_L1] = 0;
	}
	
	//when L1 is pressed first
	if(tracker[BTN_L1] == 1)
	{
		//send zoom packet
		if(zoom_level != 1)
		{
			zoom_level--;
			change = true;
			//packet->update_type = 0x0A;
		}
		switch( zoom_level )
		{
			case 1:
				packet->camera_zoom = 0x00000000;
				break;
			case 2:
				packet->camera_zoom = 0x00080000;
				break;
			case 3:
				packet->camera_zoom = 0x01000000;
				break;
			case 4:
				packet->camera_zoom = 0x01080000;
				break;
			case 5:
				packet->camera_zoom = 0x02000000;
				break;
			case 6:
				packet->camera_zoom = 0x02080000;
				break;
			case 7:
				packet->camera_zoom = 0x03000000;
				break;
			case 8:
				packet->camera_zoom = 0x03080000;
				break;
			case 9:
				packet->camera_zoom = 0x04000000;
				break;
			default:
				packet->camera_zoom = 0xAAAAAAAA; // no zoom update
				break;
		}
		//comm->writeData(packet);
	}

	//poll R1
	if( js.rgbButtons[BTN_R1] & 0x80 )
	{
		tracker[BTN_R1]++;
	}
	else
	{
		tracker[BTN_R1] = 0;
	}
	
	//when R1 is pressed first
	if(tracker[BTN_R1] == 1)
	{
		//send zoom packet
		if(zoom_level != 9)
		{
			zoom_level++;
			change = true;
			//packet->update_type = 0x0A;
		}
		switch( zoom_level )
		{
			case 1:
				packet->camera_zoom = 0x00000000;
				break;
			case 2:
				packet->camera_zoom = 0x00080000;
				break;
			case 3:
				packet->camera_zoom = 0x01000000;
				break;
			case 4:
				packet->camera_zoom = 0x01080000;
				break;
			case 5:
				packet->camera_zoom = 0x02000000;
				break;
			case 6:
				packet->camera_zoom = 0x02080000;
				break;
			case 7:
				packet->camera_zoom = 0x03000000;
				break;
			case 8:
				packet->camera_zoom = 0x03080000;
				break;
			case 9:
				packet->camera_zoom = 0x04000000;
				break;
			default:
				packet->camera_zoom = 0xAAAAAAAA;  //no zoom update
				break;
		}
		//comm->writeData(packet);
	}

	if( change == false && manualMode )
	{
		manualMode = false;
		change = true;
		manualModeCounter = 7;
	}
	else if( change == false && manualModeCounter > 0 )
	{
		--manualModeCounter;
		change = true;
	}

	// if anything has changed, send packet.
	if(change == true)
	{
		//System::Diagnostics::Trace::WriteLine( /*"Stick Angle: " + joyie.stickAngle + */"; X distance: " + (int)localX + "; Y distance: " + (int)localY );
		/*
		packet->gps_lat_gimbal_x.i=0xAAAAAAAA;
		packet->gps_lon_gimbal_y.i=0xAAAAAAAA;
		packet->gps_alt.i=0xAAAAAAAA;
		packet->camera_zoom=0xAAAAAAAA;
		 ->update_type = 0xAA;
		*/
		if (lastPacket) {
			delete lastPacket;
			lastPacket = NULL;
		}
		lastPacket = packet;
		comm->writeData(packet);
		// This packet is deleted by the comport
	}
	else
	{
		comm->writeData(lastPacket);
		//delete packet;
	}

	//Fill up text w/ buttons
    for( int i = 0; i < 128; i++ )
    {
        if( js.rgbButtons[i] & 0x80 )
        {
			//System::Diagnostics::Trace::WriteLine("Button " + i + " pressed.");
            //TCHAR sz[128];
            //StringCchPrintf( sz, 128, TEXT( "%02d " ), i );
            //StringCchCat( strText, 512, sz );
        }
    }
    return S_OK;
}

void 
Joystick::CleanupForIsXInputDevice()
{
    // Cleanup linked list
    XINPUT_DEVICE_NODE* pNode = g_pXInputDeviceList;
    while( pNode )
    {
        XINPUT_DEVICE_NODE* pDelete = pNode;
        pNode = pNode->pNext;
        SAFE_DELETE( pDelete );
    }
}