
// TesterAppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TesterApp.h"
#include "TesterAppDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT CreateFilter(BSTR bstrLibName, REFCLSID rclsid, LPUNKNOWN pUnkOuter, REFIID riid, LPVOID* ppv)
{
	// Load the library (bstrlibname should have the fullpath)
	HINSTANCE hDLL = CoLoadLibrary(bstrLibName, TRUE);

	if (hDLL == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	// Get the function pointer
	typedef HRESULT(WINAPI* PFNDllGetClassObject)(REFCLSID rclsid, REFIID riid, LPVOID* ppv);

	PFNDllGetClassObject pfnDllGetClassObject = (PFNDllGetClassObject)GetProcAddress(hDLL, "DllGetClassObject");
	if (!pfnDllGetClassObject)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	// Get the class faftory
	CComPtr<IClassFactory> pFactory;
	HRESULT hr = pfnDllGetClassObject(rclsid, IID_IClassFactory, (LPVOID*)&pFactory);
	if (hr != S_OK)
	{
		return hr;
	}

	// Create object instance
	return pFactory->CreateInstance(pUnkOuter, riid, ppv);
}
// CAboutDlg dialog used for App About
static void ReportError(LPCWSTR msg, HRESULT hr)
{
	CString temp;
	temp.Format(L"Failure code : %x", hr);

	::MessageBox(NULL, msg, temp, MB_ICONERROR);
	::PostQuitMessage(1);
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTesterAppDlg dialog




CTesterAppDlg::CTesterAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTesterAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTesterAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, m_videoWnd);
}

BEGIN_MESSAGE_MAP(CTesterAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CTesterAppDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CTesterAppDlg message handlers

BOOL CTesterAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	HRESULT hr = CoInitialize(NULL);

	hr = m_graph.CoCreateInstance(CLSID_FilterGraph);
	if(FAILED(hr))
	{
		ReportError(L"Failed to create filter manager", hr);
	}

	hr = m_graph->QueryInterface(IID_IFilterGraph2, (void**)&m_filterGraph2);
	if(FAILED(hr))
	{
		ReportError(L"Failed to query for IFilterGraph2 interface", hr);
	}

	hr = m_graph->QueryInterface(IID_IMediaControl, (void**)&m_mediaCtrl);
	if(FAILED(hr))
	{
		ReportError(L"Failed to query for IMediaControl interface", hr);
	}

	CComBSTR bstrSDLVideoRendererDllPath = _T("SDLVideoRenderer.dll");

	IBaseFilter* pSDLVideoRender = NULL;
	hr = CreateFilter(bstrSDLVideoRendererDllPath, CLSID_SDLVideoRenderer, NULL, IID_IBaseFilter, (LPVOID*)&pSDLVideoRender);
	if (FAILED(hr) || pSDLVideoRender == NULL)
	{
		return hr;
	}

	hr = pSDLVideoRender->QueryInterface(IID_IQualProp, (void**)&m_quality);
	if(FAILED(hr))
	{
		ReportError(L"Failed to query for IQualProp interface", hr);
	}

	ISDLWindowlessControl* pWc = NULL;
	hr = pSDLVideoRender->QueryInterface(IID_ISDLWindowlessControl, (void**)&pWc);
	if(FAILED(hr))
	{
		ReportError(L"Failed to set video window", hr);
	}

	pWc->SetVideoClippingWindow(m_videoWnd.m_hWnd);

	hr = m_filterGraph2->AddFilter(pSDLVideoRender, FILTER_NAME);
	if(FAILED(hr))
	{
		ReportError(L"Failed to add video renderer to graph", hr);
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTesterAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTesterAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CTesterAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTesterAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	int fps = 0;
	if(SUCCEEDED(m_quality->get_AvgFrameRate(&fps)))
	{
		TRACE(_T("Frame rate = %d\n"), fps);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CTesterAppDlg::OnBnClickedButton1()
{
	m_mediaCtrl->Stop();
	CFileDialog fOpenDlg(TRUE);
  
	if(fOpenDlg.DoModal() == IDOK)
	{
		CString path = fOpenDlg.GetPathName();
		HRESULT hr = m_filterGraph2->RenderFile(path, NULL);
	    if(SUCCEEDED(hr))
		{
			m_mediaCtrl->Run();
			SetTimer(0, 1000, 0);
		}
		else
		{
			ReportError(L"Failed to render file", hr);
		}
	}
}
