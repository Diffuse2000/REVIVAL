#include <Base/FDS_VARS.h>
#include <Base/FDS_DECS.h>
#include "D3D8.h"
#include "D3D8i.h"


CD3DApplication *g_pD3DApp;
static VESA_Surface Secondary;
static VESA_Surface DD_MainSurf;


CD3DApplication::CD3DApplication()
{
    g_pD3DApp           = this;

    m_dwNumAdapters     = 0;
    m_dwAdapter         = 0L;
    m_pD3D              = NULL;
    m_pd3dDevice        = NULL;
    m_hWnd              = NULL;
    m_hWndFocus         = NULL;
    m_bActive           = FALSE;
    m_bReady            = FALSE;
    m_dwCreateFlags     = 0L;

    m_bFrameMoving      = TRUE;
    m_bSingleStep       = FALSE;
    m_fFPS              = 0.0f;
    m_strDeviceStats[0] = _T('\0');
    m_strFrameStats[0]  = _T('\0');

    m_strWindowTitle    = _T("D3D8 Application");
	m_dwDefaultWidth    = 800;
	m_dwDefaultHeight   = 600;
	m_dwDefaultBPP		= 32;
	m_dwDefaultFlags    = //D3DAppDef_FullScreen | 
						  //D3DAppDef_Notoggle |
						  //D3DAppDef_Nomenu |
						  //D3DAppDef_Noaccel;
						0;
    m_dwCreationWidth   = 0;
    m_dwCreationHeight  = 0;
    m_bUseDepthBuffer   = FALSE;
    m_dwMinDepthBits    = 16;
    m_dwMinStencilBits  = 0;
    m_bShowCursorWhenFullscreen = FALSE;
}

// ----------------------------------------------------------------------------
// Name: AdjustWindowForChange()
// Desc: Prepare the window for a possible change between windowed mode and
//       fullscreen mode.  This function is virtual and thus can be overridden
//       to provide different behavior, such as switching to an entirely
//       different window for fullscreen mode (as in the MFC sample apps).
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::AdjustWindowForChange()
{
    if( m_bWindowed )
    {
        // Set windowed-mode style
        SetWindowLong( m_hWnd, GWL_STYLE, m_dwWindowStyle );
    }
    else
    {
        // Set fullscreen-mode style
        SetWindowLong( m_hWnd, GWL_STYLE, WS_POPUP|WS_SYSMENU|WS_VISIBLE );
    }
    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: DisplayErrorMsg()
// Desc: Displays error messages in a message box
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::DisplayErrorMsg( HRESULT hr, DWORD dwType )
{
    TCHAR strMsg[512];

    switch( hr )
    {
        case (HRESULT)D3DAPPERR_NODIRECT3D:
            _tcscpy( strMsg, _T("Could not initialize Direct3D. You may\n")
                             _T("want to check that the latest version of\n")
                             _T("DirectX is correctly installed on your\n")
                             _T("system.  Also make sure that this program\n")
                             _T("was compiled with header files that match\n")
                             _T("the installed DirectX DLLs.") );
            break;

        case (HRESULT)D3DAPPERR_NOCOMPATIBLEDEVICES:
            _tcscpy( strMsg, _T("Could not find any compatible Direct3D\n")
                             _T("devices.") );
            break;

        case (HRESULT)D3DAPPERR_NOWINDOWABLEDEVICES:
            _tcscpy( strMsg, _T("This sample cannot run in a desktop\n")
                             _T("window with the current display settings.\n")
                             _T("Please change your desktop settings to a\n")
                             _T("16- or 32-bit display mode and re-run this\n")
                             _T("sample.") );
            break;

        case (HRESULT)D3DAPPERR_NOHARDWAREDEVICE:
            _tcscpy( strMsg, _T("No hardware-accelerated Direct3D devices\n")
                             _T("were found.") );
            break;

        case (HRESULT)D3DAPPERR_HALNOTCOMPATIBLE:
            _tcscpy( strMsg, _T("This sample requires functionality that is\n")
                             _T("not available on your Direct3D hardware\n")
                             _T("accelerator.") );
            break;

        case (HRESULT)D3DAPPERR_NOWINDOWEDHAL:
            _tcscpy( strMsg, _T("Your Direct3D hardware accelerator cannot\n")
                             _T("render into a window.\n")
                             _T("Press F2 while the app is running to see a\n")
                             _T("list of available devices and modes.") );
            break;

        case (HRESULT)D3DAPPERR_NODESKTOPHAL:
            _tcscpy( strMsg, _T("Your Direct3D hardware accelerator cannot\n")
                             _T("render into a window with the current\n")
                             _T("desktop display settings.\n")
                             _T("Press F2 while the app is running to see a\n")
                             _T("list of available devices and modes.") );
            break;

        case (HRESULT)D3DAPPERR_NOHALTHISMODE:
            _tcscpy( strMsg, _T("This sample requires functionality that is\n")
                             _T("not available on your Direct3D hardware\n")
                             _T("accelerator with the current desktop display\n")
                             _T("settings.\n")
                             _T("Press F2 while the app is running to see a\n")
                             _T("list of available devices and modes.") );
            break;

        case (HRESULT)D3DAPPERR_MEDIANOTFOUND:
            _tcscpy( strMsg, _T("Could not load required media." ) );
            break;

        case (HRESULT)D3DAPPERR_RESIZEFAILED:
            _tcscpy( strMsg, _T("Could not reset the Direct3D device." ) );
            break;

        case (HRESULT)D3DAPPERR_NONZEROREFCOUNT:
            _tcscpy( strMsg, _T("A D3D object has a non-zero reference\n")
                             _T("count (meaning things were not properly\n")
                             _T("cleaned up).") );
            break;

        case (HRESULT)E_OUTOFMEMORY:
            _tcscpy( strMsg, _T("Not enough memory.") );
            break;

        case (HRESULT)D3DERR_OUTOFVIDEOMEMORY:
            _tcscpy( strMsg, _T("Not enough video memory.") );
            break;

        default:
            _tcscpy( strMsg, _T("Generic application error. Enable\n")
                             _T("debug output for detailed information.") );
    }

    if( MSGERR_APPMUSTEXIT == dwType )
    {
        _tcscat( strMsg, _T("\n\nThis sample will now exit.") );
        MessageBox( NULL, strMsg, m_strWindowTitle, MB_ICONERROR|MB_OK );

        // Close the window, which shuts down the app
        if( m_hWnd )
            SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
    }
    else
    {
        if( MSGWARN_SWITCHEDTOREF == dwType )
            _tcscat( strMsg, _T("\n\nSwitching to the reference rasterizer,\n")
                             _T("a software device that implements the entire\n")
                             _T("Direct3D feature set, but runs very slowly.") );
        MessageBox( NULL, strMsg, m_strWindowTitle, MB_ICONWARNING|MB_OK );
    }

    return hr;
}

//-----------------------------------------------------------------------------
// Name: SortModesCallback()
// Desc: Callback function for sorting display modes (used by BuildDeviceList).
//-----------------------------------------------------------------------------
static int SortModesCallback( const VOID* arg1, const VOID* arg2 )
{
    D3DDISPLAYMODE* p1 = (D3DDISPLAYMODE*)arg1;
    D3DDISPLAYMODE* p2 = (D3DDISPLAYMODE*)arg2;

    if( p1->Format > p2->Format )   return -1;
    if( p1->Format < p2->Format )   return +1;
    if( p1->Width  < p2->Width )    return -1;
    if( p1->Width  > p2->Width )    return +1;
    if( p1->Height < p2->Height )   return -1;
    if( p1->Height > p2->Height )   return +1;

    return 0;
}

//-----------------------------------------------------------------------------
// Name: FindDepthStencilFormat()
// Desc: Finds a depth/stencil format for the given device that is compatible
//       with the render target format and meets the needs of the app.
//-----------------------------------------------------------------------------
BOOL CD3DApplication::FindDepthStencilFormat( UINT iAdapter, D3DDEVTYPE DeviceType,
    D3DFORMAT TargetFormat, D3DFORMAT* pDepthStencilFormat )
{
    if( m_dwMinDepthBits <= 16 && m_dwMinStencilBits == 0 )
    {
        if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16 ) ) )
        {
            if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D16 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D16;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 15 && m_dwMinStencilBits <= 1 )
    {
        if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D15S1 ) ) )
        {
            if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D15S1 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D15S1;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 24 && m_dwMinStencilBits == 0 )
    {
        if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8 ) ) )
        {
            if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D24X8 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D24X8;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 24 && m_dwMinStencilBits <= 8 )
    {
        if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8 ) ) )
        {
            if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D24S8 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D24S8;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 24 && m_dwMinStencilBits <= 4 )
    {
        if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X4S4 ) ) )
        {
            if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D24X4S4 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D24X4S4;
                return TRUE;
            }
        }
    }

    if( m_dwMinDepthBits <= 32 && m_dwMinStencilBits == 0 )
    {
        if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
            TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32 ) ) )
        {
            if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
                TargetFormat, TargetFormat, D3DFMT_D32 ) ) )
            {
                *pDepthStencilFormat = D3DFMT_D32;
                return TRUE;
            }
        }
    }

    return FALSE;
}




HRESULT CD3DApplication::BuildDeviceList()
{
    const DWORD dwNumDeviceTypes = 2;
    const TCHAR* strDeviceDescs[] = { _T("HAL"), _T("REF") };
    const D3DDEVTYPE DeviceTypes[] = { D3DDEVTYPE_HAL, D3DDEVTYPE_REF };

    BOOL bHALExists = FALSE;
    BOOL bHALIsWindowedCompatible = FALSE;
    BOOL bHALIsDesktopCompatible = FALSE;
    BOOL bHALIsSampleCompatible = FALSE;

	// Read Default VideoMode (x,y,BPP)
	

    // Loop through all the adapters on the system (usually, there's just one
    // unless more than one graphics card is present).
    for( UINT iAdapter = 0; iAdapter < m_pD3D->GetAdapterCount(); iAdapter++ )
    {
        // Fill in adapter info
        D3DAdapterInfo* pAdapter  = &m_Adapters[m_dwNumAdapters];
        m_pD3D->GetAdapterIdentifier( iAdapter, 0, &pAdapter->d3dAdapterIdentifier );
        m_pD3D->GetAdapterDisplayMode( iAdapter, &pAdapter->d3ddmDesktop );
        pAdapter->dwNumDevices    = 0;
        pAdapter->dwCurrentDevice = 0;

        // Enumerate all display modes on this adapter
        D3DDISPLAYMODE modes[100];
        D3DFORMAT      formats[20];
        DWORD dwNumFormats      = 0;
        DWORD dwNumModes        = 0;
        DWORD dwNumAdapterModes = m_pD3D->GetAdapterModeCount( iAdapter );

        // Add the adapter's current desktop format to the list of formats
        formats[dwNumFormats++] = pAdapter->d3ddmDesktop.Format;

        for( UINT iMode = 0; iMode < dwNumAdapterModes; iMode++ )
        {
            // Get the display mode attributes
            D3DDISPLAYMODE DisplayMode;
            m_pD3D->EnumAdapterModes( iAdapter, iMode, &DisplayMode );

            // Filter out low-resolution modes
            if( DisplayMode.Width  < 640 || DisplayMode.Height < 400 )
                continue;

			DWORD m;
            // Check if the mode already exists (to filter out refresh rates)
            for( m=0L; m<dwNumModes; m++ )
            {
                if( ( modes[m].Width  == DisplayMode.Width  ) &&
                    ( modes[m].Height == DisplayMode.Height ) &&
                    ( modes[m].Format == DisplayMode.Format ) )
                    break;
            }

            // If we found a new mode, add it to the list of modes
            if( m == dwNumModes )
            {
                modes[dwNumModes].Width       = DisplayMode.Width;
                modes[dwNumModes].Height      = DisplayMode.Height;
                modes[dwNumModes].Format      = DisplayMode.Format;
                modes[dwNumModes].RefreshRate = 0;
                dwNumModes++;

				DWORD f;
                // Check if the mode's format already exists
                for( f=0; f<dwNumFormats; f++ )
                {
                    if( DisplayMode.Format == formats[f] )
                        break;
                }

                // If the format is new, add it to the list
                if( f== dwNumFormats )
                    formats[dwNumFormats++] = DisplayMode.Format;
            }
        }

        // Sort the list of display modes (by format, then width, then height)
        qsort( modes, dwNumModes, sizeof(D3DDISPLAYMODE), SortModesCallback );

        // Add devices to adapter
        for( UINT iDevice = 0; iDevice < dwNumDeviceTypes; iDevice++ )
        {
            // Fill in device info
            D3DDeviceInfo* pDevice;
            pDevice                 = &pAdapter->devices[pAdapter->dwNumDevices];
            pDevice->DeviceType     = DeviceTypes[iDevice];
            m_pD3D->GetDeviceCaps( iAdapter, DeviceTypes[iDevice], &pDevice->d3dCaps );
            pDevice->strDesc        = strDeviceDescs[iDevice];
            pDevice->dwNumModes     = 0;
            pDevice->dwCurrentMode  = 0;
            pDevice->bCanDoWindowed = FALSE;
            pDevice->bWindowed      = FALSE;
            pDevice->MultiSampleType = D3DMULTISAMPLE_NONE;

            // Examine each format supported by the adapter to see if it will
            // work with this device and meets the needs of the application.
            BOOL  bFormatConfirmed[20];
            DWORD dwBehavior[20];
            D3DFORMAT fmtDepthStencil[20];

            for( DWORD f=0; f<dwNumFormats; f++ )
            {
                bFormatConfirmed[f] = FALSE;
                fmtDepthStencil[f] = D3DFMT_UNKNOWN;

                // Skip formats that cannot be used as render targets on this device
                if( FAILED( m_pD3D->CheckDeviceType( iAdapter, pDevice->DeviceType,
                                                     formats[f], formats[f], FALSE ) ) )
                    continue;

                if( pDevice->DeviceType == D3DDEVTYPE_HAL )
                {
                    // This system has a HAL device
                    bHALExists = TRUE;

                    if( pDevice->d3dCaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED )
                    {
                        // HAL can run in a window for some mode
                        bHALIsWindowedCompatible = TRUE;

                        if( f == 0 )
                        {
                            // HAL can run in a window for the current desktop mode
                            bHALIsDesktopCompatible = TRUE;
                        }
                    }
                }

                // Confirm the device/format for HW vertex processing
                if( pDevice->d3dCaps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT )
                {
                    if( pDevice->d3dCaps.DevCaps&D3DDEVCAPS_PUREDEVICE )
                    {
                        dwBehavior[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING |
                                        D3DCREATE_PUREDEVICE;

                        if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
                                                      formats[f] ) ) )
                            bFormatConfirmed[f] = TRUE;
                    }

                    if ( FALSE == bFormatConfirmed[f] )
                    {
                        dwBehavior[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING;

                        if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
                                                      formats[f] ) ) )
                            bFormatConfirmed[f] = TRUE;
                    }

                    if ( FALSE == bFormatConfirmed[f] )
                    {
                        dwBehavior[f] = D3DCREATE_MIXED_VERTEXPROCESSING;

                        if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
                                                      formats[f] ) ) )
                            bFormatConfirmed[f] = TRUE;
                    }
                }

                // Confirm the device/format for SW vertex processing
                if( FALSE == bFormatConfirmed[f] )
                {
                    dwBehavior[f] = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

                    if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
                                                  formats[f] ) ) )
                        bFormatConfirmed[f] = TRUE;
                }

                // Find a suitable depth/stencil buffer format for this device/format
                if( bFormatConfirmed[f] && m_bUseDepthBuffer )
                {
                    if( !FindDepthStencilFormat( iAdapter, pDevice->DeviceType,
                        formats[f], &fmtDepthStencil[f] ) )
                    {
                        bFormatConfirmed[f] = FALSE;
                    }
                }
            }

			DWORD m;
            // Add all enumerated display modes with confirmed formats to the
            // device's list of valid modes
            for( m=0L; m<dwNumModes; m++ )
            {
                for( DWORD f=0; f<dwNumFormats; f++ )
                {
                    if( modes[m].Format == formats[f] )
                    {
                        if( bFormatConfirmed[f] == TRUE )
                        {
                            // Add this mode to the device's list of valid modes
                            pDevice->modes[pDevice->dwNumModes].Width      = modes[m].Width;
                            pDevice->modes[pDevice->dwNumModes].Height     = modes[m].Height;
                            pDevice->modes[pDevice->dwNumModes].Format     = modes[m].Format;
                            pDevice->modes[pDevice->dwNumModes].dwBehavior = dwBehavior[f];
                            pDevice->modes[pDevice->dwNumModes].DepthStencilFormat = fmtDepthStencil[f];
                            pDevice->dwNumModes++;

                            if( pDevice->DeviceType == D3DDEVTYPE_HAL )
                                bHALIsSampleCompatible = TRUE;
                        }
                    }
                }
            }

            // Select any 640x480 mode for default (but prefer a 16-bit mode)
/*            for( m=0; m<pDevice->dwNumModes; m++ )
            {
                if( pDevice->modes[m].Width==640 && pDevice->modes[m].Height==480 )
                {
                    pDevice->dwCurrentMode = m;
                    if( pDevice->modes[m].Format == D3DFMT_R5G6B5 ||
                        pDevice->modes[m].Format == D3DFMT_X1R5G5B5 ||
                        pDevice->modes[m].Format == D3DFMT_A1R5G5B5 )
                    {
                        break;
                    }
                }
            }*/
			// Select any mode with the specified default BPP, but
			// prefer modes matching default X,Y resolution.
			pDevice->dwCurrentMode =-1;
            for( m=0; m<pDevice->dwNumModes; m++ )
            {
				switch (m_dwDefaultBPP) 
				{
				case 16:
					if( pDevice->modes[m].Format != D3DFMT_R5G6B5 &&
						pDevice->modes[m].Format != D3DFMT_X1R5G5B5 &&
	                    pDevice->modes[m].Format != D3DFMT_A1R5G5B5 )
					{
						continue;
					}
				break;				
				case 32:										
					if( pDevice->modes[m].Format != D3DFMT_A8R8G8B8 &&// look for best bit usage (enabled alpha)
						pDevice->modes[m].Format != D3DFMT_X8R8G8B8
						)
					{
						continue;
					}
    
				break;
				}				
				if( pDevice->modes[m].Width==m_dwDefaultWidth && pDevice->modes[m].Height==m_dwDefaultHeight)
				{
					pDevice->dwCurrentMode = m;
		            break;	            
                }
            }
			// Second pass (w/o BPP requirement)
			if (pDevice->dwCurrentMode==-1)
			{
				for( m=0; m<pDevice->dwNumModes; m++ )
				{
					if( pDevice->modes[m].Width==m_dwDefaultWidth && pDevice->modes[m].Height==m_dwDefaultHeight)
					{
						pDevice->dwCurrentMode = m;
						break;	            
					}
				}
			}

            // Check if the device is compatible with the desktop display mode
            // (which was added initially as formats[0])
            if( bFormatConfirmed[0] && (pDevice->d3dCaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED) )
            {
                pDevice->bCanDoWindowed = TRUE;
                pDevice->bWindowed      = TRUE;
            }

            // If valid modes were found, keep this device
            if( pDevice->dwNumModes > 0 )
                pAdapter->dwNumDevices++;
        }

        // If valid devices were found, keep this adapter
        if( pAdapter->dwNumDevices > 0 )
            m_dwNumAdapters++;
    }

    // Return an error if no compatible devices were found
    if( 0L == m_dwNumAdapters )
        return D3DAPPERR_NOCOMPATIBLEDEVICES;

    // Pick a default device that can render into a window
    // (This code assumes that the HAL device comes before the REF
    // device in the device array).
    for( DWORD a=0; a<m_dwNumAdapters; a++ )
    {
        for( DWORD d=0; d < m_Adapters[a].dwNumDevices; d++ )
        {
            if((m_dwDefaultFlags&D3DAppDef_FullScreen) || m_Adapters[a].devices[d].bWindowed ) // Lazy eval, if default is FULLSCREEN
            {
                m_Adapters[a].dwCurrentDevice = d;
                m_dwAdapter = a;
                m_bWindowed = m_Adapters[a].devices[d].bWindowed = !(m_dwDefaultFlags&D3DAppDef_FullScreen);
                
				// Display a warning message
                if( m_Adapters[a].devices[d].DeviceType == D3DDEVTYPE_REF )
                {
                    if( !bHALExists )
                        DisplayErrorMsg( D3DAPPERR_NOHARDWAREDEVICE, MSGWARN_SWITCHEDTOREF );
                    else if( !bHALIsSampleCompatible )
                        DisplayErrorMsg( D3DAPPERR_HALNOTCOMPATIBLE, MSGWARN_SWITCHEDTOREF );
                    else if( !bHALIsWindowedCompatible )
                        DisplayErrorMsg( D3DAPPERR_NOWINDOWEDHAL, MSGWARN_SWITCHEDTOREF );
                    else if( !bHALIsDesktopCompatible )
                        DisplayErrorMsg( D3DAPPERR_NODESKTOPHAL, MSGWARN_SWITCHEDTOREF );
                    else // HAL is desktop compatible, but not sample compatible
                        DisplayErrorMsg( D3DAPPERR_NOHALTHISMODE, MSGWARN_SWITCHEDTOREF );
                }

                return S_OK;
            }
        }
    }

    return D3DAPPERR_NOWINDOWABLEDEVICES;

}

//-----------------------------------------------------------------------------
// Name: Initialize3DEnvironment()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::Initialize3DEnvironment()
{
    HRESULT hr;

    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
    D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
    D3DModeInfo*    pModeInfo    = &pDeviceInfo->modes[pDeviceInfo->dwCurrentMode];

    // Prepare window for possible windowed/fullscreen change
    AdjustWindowForChange();

    // Set up the presentation parameters
    ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
    m_d3dpp.Windowed               = pDeviceInfo->bWindowed;
    m_d3dpp.BackBufferCount        = 1;
    m_d3dpp.MultiSampleType        = pDeviceInfo->MultiSampleType;
    m_d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    m_d3dpp.EnableAutoDepthStencil = m_bUseDepthBuffer;
    m_d3dpp.AutoDepthStencilFormat = pModeInfo->DepthStencilFormat;
    m_d3dpp.hDeviceWindow          = m_hWnd;
    if( m_bWindowed )
    {
        m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
        m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;
        m_d3dpp.BackBufferFormat = pAdapterInfo->d3ddmDesktop.Format;
    }
    else
    {
        m_d3dpp.BackBufferWidth  = pModeInfo->Width;
        m_d3dpp.BackBufferHeight = pModeInfo->Height;
        m_d3dpp.BackBufferFormat = pModeInfo->Format;
    }

    // Create the device
    hr = m_pD3D->CreateDevice( m_dwAdapter, pDeviceInfo->DeviceType,
                               m_hWndFocus, pModeInfo->dwBehavior, &m_d3dpp,
                               &m_pd3dDevice );
    if( SUCCEEDED(hr) )
    {
        // When moving from fullscreen to windowed mode, it is important to
        // adjust the window size after recreating the device rather than
        // beforehand to ensure that you get the window size you want.  For
        // example, when switching from 640x480 fullscreen to windowed with
        // a 1000x600 window on a 1024x768 desktop, it is impossible to set
        // the window size to 1000x600 until after the display mode has
        // changed to 1024x768, because windows cannot be larger than the
        // desktop.
        if( m_bWindowed )
        {
            SetWindowPos( m_hWnd, HWND_NOTOPMOST,
                          m_rcWindowBounds.left, m_rcWindowBounds.top,
                          ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
                          ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
                          SWP_SHOWWINDOW );
        }

        // Store device Caps
        m_pd3dDevice->GetDeviceCaps( &m_d3dCaps );
        m_dwCreateFlags = pModeInfo->dwBehavior;

        // Store device description
        if( pDeviceInfo->DeviceType == D3DDEVTYPE_REF )
            lstrcpy( m_strDeviceStats, TEXT("REF") );
        else if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
            lstrcpy( m_strDeviceStats, TEXT("HAL") );
        else if( pDeviceInfo->DeviceType == D3DDEVTYPE_SW )
            lstrcpy( m_strDeviceStats, TEXT("SW") );

        if( pModeInfo->dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING &&
            pModeInfo->dwBehavior & D3DCREATE_PUREDEVICE )
        {
            if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
                lstrcat( m_strDeviceStats, TEXT(" (pure hw vp)") );
            else
                lstrcat( m_strDeviceStats, TEXT(" (simulated pure hw vp)") );
        }
        else if( pModeInfo->dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
        {
            if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
                lstrcat( m_strDeviceStats, TEXT(" (hw vp)") );
            else
                lstrcat( m_strDeviceStats, TEXT(" (simulated hw vp)") );
        }
        else if( pModeInfo->dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING )
        {
            if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
                lstrcat( m_strDeviceStats, TEXT(" (mixed vp)") );
            else
                lstrcat( m_strDeviceStats, TEXT(" (simulated mixed vp)") );
        }
        else if( pModeInfo->dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING )
        {
            lstrcat( m_strDeviceStats, TEXT(" (sw vp)") );
        }

        if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
        {
            lstrcat( m_strDeviceStats, TEXT(": ") );
            lstrcat( m_strDeviceStats, pAdapterInfo->d3dAdapterIdentifier.Description );
        }

        // Store render target surface desc
        LPDIRECT3DSURFACE8 pBackBuffer;
        m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
        pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
        pBackBuffer->Release();

        // Set up the fullscreen cursor
        if( m_bShowCursorWhenFullscreen && !m_bWindowed )
        {
            HCURSOR hCursor;
#ifdef _WIN64
            hCursor = (HCURSOR)GetClassLongPtr( m_hWnd, GCLP_HCURSOR );
#else
            hCursor = (HCURSOR)GetClassLong( m_hWnd, GCL_HCURSOR );
#endif
            D3DUtil_SetDeviceCursor( m_pd3dDevice, hCursor );
            m_pd3dDevice->ShowCursor( TRUE );
        }

        // Initialize the app's device-dependent objects
        hr = InitDeviceObjects();
        if( SUCCEEDED(hr) )
        {
            hr = RestoreDeviceObjects();
            if( SUCCEEDED(hr) )
            {
                m_bActive = TRUE;
                return S_OK;
            }
        }

        // Cleanup before we try again
        InvalidateDeviceObjects();
        DeleteDeviceObjects();
        SAFE_RELEASE( m_pd3dDevice );
    }

    // If that failed, fall back to the reference rasterizer
    if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
    {
        // Let the user know we are switching from HAL to the reference rasterizer
        DisplayErrorMsg( hr, MSGWARN_SWITCHEDTOREF );

        // Select the default adapter
        m_dwAdapter = 0L;
        pAdapterInfo = &m_Adapters[m_dwAdapter];

        // Look for a software device
        for( UINT i=0L; i<pAdapterInfo->dwNumDevices; i++ )
        {
            if( pAdapterInfo->devices[i].DeviceType == D3DDEVTYPE_REF )
            {
                pAdapterInfo->dwCurrentDevice = i;
                pDeviceInfo = &pAdapterInfo->devices[i];
                m_bWindowed = pDeviceInfo->bWindowed;
                break;
            }
        }

        // Try again, this time with the reference rasterizer
        if( pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice].DeviceType ==
            D3DDEVTYPE_REF )
        {
            hr = Initialize3DEnvironment();
        }
    }

    return hr;
}
//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::Resize3DEnvironment()
{
    HRESULT hr;

    // Release all vidmem objects
    if( FAILED( hr = InvalidateDeviceObjects() ) )
        return hr;

    // Reset the device
    if( FAILED( hr = m_pd3dDevice->Reset( &m_d3dpp ) ) )
        return hr;

    // Store render target surface desc
    LPDIRECT3DSURFACE8 pBackBuffer;
    m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
    pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
    pBackBuffer->Release();

    // Set up the fullscreen cursor
    if( m_bShowCursorWhenFullscreen && !m_bWindowed )
    {
        HCURSOR hCursor;
#ifdef _WIN64
        hCursor = (HCURSOR)GetClassLongPtr( m_hWnd, GCLP_HCURSOR );
#else
        hCursor = (HCURSOR)GetClassLong( m_hWnd, GCL_HCURSOR );
#endif
        D3DUtil_SetDeviceCursor( m_pd3dDevice, hCursor );
        m_pd3dDevice->ShowCursor( TRUE );
    }

    // Initialize the app's device-dependent objects
    hr = RestoreDeviceObjects();
    if( FAILED(hr) )
        return hr;

    // If the app is paused, trigger the rendering of the current frame
    if( FALSE == m_bFrameMoving )
    {
        m_bSingleStep = TRUE;
        DXUtil_Timer( TIMER_START );
        DXUtil_Timer( TIMER_STOP );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ToggleFullScreen()
// Desc: Called when user toggles between fullscreen mode and windowed mode
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::ToggleFullscreen()
{
	// Sabotage (to force ONLY fullscreen or ONLY windowed)
	if (m_dwDefaultFlags & D3DAppDef_Notoggle)
		return S_OK;

    // Get access to current adapter, device, and mode
    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
    D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
    D3DModeInfo*    pModeInfo    = &pDeviceInfo->modes[pDeviceInfo->dwCurrentMode];

    // Need device change if going windowed and the current device
    // can only be fullscreen
    if( !m_bWindowed && !pDeviceInfo->bCanDoWindowed )
        return ForceWindowed();

    m_bReady = FALSE;

    // Toggle the windowed state
    m_bWindowed = !m_bWindowed;
    pDeviceInfo->bWindowed = m_bWindowed;

    // Prepare window for windowed/fullscreen change
    AdjustWindowForChange();

    // Set up the presentation parameters
    m_d3dpp.Windowed               = pDeviceInfo->bWindowed;
    m_d3dpp.MultiSampleType        = pDeviceInfo->MultiSampleType;
    m_d3dpp.AutoDepthStencilFormat = pModeInfo->DepthStencilFormat;
    m_d3dpp.hDeviceWindow          = m_hWnd;
    if( m_bWindowed )
    {
        m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
        m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;
        m_d3dpp.BackBufferFormat = pAdapterInfo->d3ddmDesktop.Format;
    }
    else
    {
        m_d3dpp.BackBufferWidth  = pModeInfo->Width;
        m_d3dpp.BackBufferHeight = pModeInfo->Height;
        m_d3dpp.BackBufferFormat = pModeInfo->Format;
    }

    // Resize the 3D device
    if( FAILED( Resize3DEnvironment() ) )
    {
        if( m_bWindowed )
            return ForceWindowed();
        else
            return E_FAIL;
    }

    // When moving from fullscreen to windowed mode, it is important to
    // adjust the window size after resetting the device rather than
    // beforehand to ensure that you get the window size you want.  For
    // example, when switching from 640x480 fullscreen to windowed with
    // a 1000x600 window on a 1024x768 desktop, it is impossible to set
    // the window size to 1000x600 until after the display mode has
    // changed to 1024x768, because windows cannot be larger than the
    // desktop.
    if( m_bWindowed )
    {
        SetWindowPos( m_hWnd, HWND_NOTOPMOST,
                      m_rcWindowBounds.left, m_rcWindowBounds.top,
                      ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
                      ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
                      SWP_SHOWWINDOW );
    }

    m_bReady = TRUE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ForceWindowed()
// Desc: Switch to a windowed mode, even if that means picking a new device
//       and/or adapter
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::ForceWindowed()
{
    HRESULT hr;
    D3DAdapterInfo* pAdapterInfoCur = &m_Adapters[m_dwAdapter];
    D3DDeviceInfo*  pDeviceInfoCur  = &pAdapterInfoCur->devices[pAdapterInfoCur->dwCurrentDevice];
    BOOL bFoundDevice = FALSE;

    if( pDeviceInfoCur->bCanDoWindowed )
    {
        bFoundDevice = TRUE;
    }
    else
    {
        // Look for a windowable device on any adapter
        D3DAdapterInfo* pAdapterInfo;
        DWORD dwAdapter;
        D3DDeviceInfo* pDeviceInfo;
        DWORD dwDevice;
        for( dwAdapter = 0; dwAdapter < m_dwNumAdapters; dwAdapter++ )
        {
            pAdapterInfo = &m_Adapters[dwAdapter];
            for( dwDevice = 0; dwDevice < pAdapterInfo->dwNumDevices; dwDevice++ )
            {
                pDeviceInfo = &pAdapterInfo->devices[dwDevice];
                if( pDeviceInfo->bCanDoWindowed )
                {
                    m_dwAdapter = dwAdapter;
                    pDeviceInfoCur = pDeviceInfo;
                    pAdapterInfo->dwCurrentDevice = dwDevice;
                    bFoundDevice = TRUE;
                    break;
                }
            }
            if( bFoundDevice )
                break;
        }
    }

    if( !bFoundDevice )
        return E_FAIL;

    pDeviceInfoCur->bWindowed = TRUE;
    m_bWindowed = TRUE;

    // Now destroy the current 3D device objects, then reinitialize

    m_bReady = FALSE;

    // Release all scene objects that will be re-created for the new device
    InvalidateDeviceObjects();
    DeleteDeviceObjects();

    // Release display objects, so a new device can be created
    if( m_pd3dDevice->Release() > 0L )
        return DisplayErrorMsg( D3DAPPERR_NONZEROREFCOUNT, MSGERR_APPMUSTEXIT );

    // Create the new device
    if( FAILED( hr = Initialize3DEnvironment() ) )
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    m_bReady = TRUE;

    return S_OK;
}


dword CD3DApplication::Create()
{
   	D3DDISPLAYMODE mode;
	HRESULT hr;
	
	// display parameter setup
//	m_hInst = g_DIP.hInst;
	m_hWnd = g_DIP.hWnd;

	m_dwDefaultWidth = g_DIP.x;
	m_dwDefaultHeight = g_DIP.y;
	m_dwDefaultBPP = g_DIP.bpp;
	m_dwDefaultFlags = 0;
	if (g_DIP.flags & DISP_FULLSCREEN) m_dwDefaultFlags |= D3DAppDef_FullScreen;

	
	// Create the Direct3D object
    m_pD3D = Direct3DCreate8( D3D_SDK_VERSION );
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&mode);

	// Does not use Default (X,Y) resolution
    m_dwCreationWidth   = mode.Width;
    m_dwCreationHeight  = mode.Height;	

    if( m_pD3D == NULL )
        return 1; //DisplayErrorMsg( D3DAPPERR_NODIRECT3D, MSGERR_APPMUSTEXIT );

    // Build a list of Direct3D adapters, modes and devices. The
    // ConfirmDevice() callback is used to confirm that only devices that
    // meet the app's requirements are considered.
	if( FAILED( hr = BuildDeviceList() ) )
	{
		SAFE_RELEASE( m_pD3D );
        return 1; //DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    }

    // Save window properties
    m_dwWindowStyle = GetWindowLong( m_hWnd, GWL_STYLE );
    GetWindowRect( m_hWnd, &m_rcWindowBounds );
    GetClientRect( m_hWnd, &m_rcWindowClient );

	ShowWindow(m_hWnd, SW_SHOW);

	return 0;
}

static dword V_Create(VESA_Surface *VS)
{
	VS->CPP = (VS->BPP+1)>>3;
	VS->BPSL = VS->CPP * VS->X;
	VS->PageSize = VS->BPSL * VS->Y;
	if (!(VS->Data = (byte *)_aligned_malloc(VS->PageSize, 32))) return 1;
	memset(VS->Data,0,VS->PageSize);
//	VS->Flags = VSurf_Exists;

	VS->Handle = 0;

	return 0;
}


static void V_Flip(VESA_Surface *VS)
{
}

dword D3D8_InitDisplay()
{
	g_pD3DApp = new CD3DApplication;
	mword ret = g_pD3DApp->Create();
	if (ret) return ret;

	// main system memory
 	DD_MainSurf.X = g_DIP.x;
	DD_MainSurf.Y = g_DIP.y;
	DD_MainSurf.BPP = (BYTE)g_DIP.bpp;

	V_Create(&DD_MainSurf);

	DD_MainSurf.Flip = V_Flip;

	VESA_VPageExternal(&DD_MainSurf);

	return 0;
}

dword D3D8_RemoveDisplay()
{
	delete g_pD3DApp;
	return 0;
}