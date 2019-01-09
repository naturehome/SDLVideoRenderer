#include "VideoRender.h"

CVideoRender::CVideoRender(LPUNKNOWN pUnk, HRESULT* phr)
	: CBaseVideoRenderer(CLSID_SDLVideoRenderer, FILTER_NAME, pUnk, phr),
	m_hWnd(0), m_converter(0)
{
    m_sdlRenderer = new CSDLRenderer();
}

CVideoRender::~CVideoRender(void)
{
	delete m_converter;
    delete m_sdlRenderer;
}

HRESULT CVideoRender::DoRenderSample(IMediaSample *pMediaSample)
{
	BYTE* pBuffer = NULL;	
	pMediaSample->GetPointer(&pBuffer);

	if(m_mediaType.subtype != MEDIASUBTYPE_RGB32)
	{
		const BYTE* rgbaBuffer = m_converter->convert_to_rgb32(pBuffer);
		return m_sdlRenderer->DrawSample(rgbaBuffer);
	}

    return m_sdlRenderer->DrawSample(pBuffer);
}

HRESULT CVideoRender::CheckMediaType(const CMediaType *pmt)
{
	CheckPointer(pmt, E_POINTER);

	if(pmt->majortype != MEDIATYPE_Video)
    {                                                  
        return E_FAIL;
    }
	
    const GUID *SubType = pmt->Subtype();
    if (SubType == NULL)
        return E_FAIL;

    if(*SubType != MEDIASUBTYPE_YV12 &&   
       *SubType != WMMEDIASUBTYPE_I420 &&
	   *SubType != MEDIASUBTYPE_IYUV && 
	   *SubType != MEDIASUBTYPE_NV12 &&
	   *SubType != MEDIASUBTYPE_YUY2 &&
	   *SubType != MEDIASUBTYPE_RGB555 &&
	   *SubType != MEDIASUBTYPE_RGB565 &&
	   *SubType != MEDIASUBTYPE_RGB24 &&
	   *SubType != MEDIASUBTYPE_RGB32)
    {
        return E_FAIL;
    }
	
	if (pmt->formattype != FORMAT_VideoInfo && pmt->formattype != FORMAT_VideoInfo2)
	{
		return E_FAIL;
	}

	return S_OK;
}

/*
HRESULT CVideoRender::IsDefaultTargetRect()
{
	return S_OK;
}

HRESULT CVideoRender::SetDefaultTargetRect()
{
	return S_OK;
}

HRESULT CVideoRender::SetTargetRect(RECT *pTargetRect)
{
	return S_OK;
}

HRESULT CVideoRender::GetTargetRect(RECT *pTargetRect)
{
	return S_OK;
}

HRESULT CVideoRender::IsDefaultSourceRect()
{
	return S_OK;
}

HRESULT CVideoRender::SetDefaultSourceRect()
{
	return S_OK;
}

HRESULT CVideoRender::SetSourceRect(RECT *pSourceRect)
{
	return S_OK;
}

HRESULT CVideoRender::GetSourceRect(RECT *pSourceRect)
{
	return S_OK;
}

HRESULT CVideoRender::GetStaticImage(long *pBufferSize, long *pDIBImage)
{
	return S_OK;
}

VIDEOINFOHEADER* CVideoRender::GetVideoFormat()
{
	return nullptr;
}
*/
void CVideoRender::CreateDefaultWindow()
{
	CVideoWindow wnd;
	m_hWnd = wnd.InitInstance(L"Direct2D Video Window", m_bmpInfo.biWidth, m_bmpInfo.biHeight);
	::SetEvent(m_event);

	wnd.ShowWindowSync();
}

DWORD WINAPI RunWindowInBackgroundThread(LPVOID target)
{
    CVideoRender* filter = (CVideoRender*)target;
	if(filter != NULL)
	{
		filter->CreateDefaultWindow();
	}

	return 1;
}

HRESULT CVideoRender::StartStreaming()
{
	::ShowWindowAsync(m_hWnd, SW_SHOW);

	return CBaseVideoRenderer::StartStreaming();
}

HRESULT CVideoRender::SetMediaType(const CMediaType *pmt)
{
	CheckPointer(pmt, E_POINTER);
	CAutoLock m_lock(this->m_pLock);
	
	m_mediaType = *pmt;
	VIDEOINFOHEADER* vInfo = NULL;
	VIDEOINFOHEADER2* vInfo2 = NULL;

	if(pmt->formattype == FORMAT_VideoInfo)
	{
		vInfo = (VIDEOINFOHEADER*)pmt->pbFormat;
		if(vInfo == NULL || vInfo->bmiHeader.biSize != sizeof(BITMAPINFOHEADER))
		{
			return E_INVALIDARG;
		}
		m_bmpInfo = vInfo->bmiHeader;
	}
	else if(pmt->formattype == FORMAT_VideoInfo2)
	{
		vInfo2 = (VIDEOINFOHEADER2*)pmt->pbFormat;
		if(vInfo2 == NULL || vInfo2->bmiHeader.biSize != sizeof(BITMAPINFOHEADER))
		{
			return E_INVALIDARG;
		}
		m_bmpInfo = vInfo2->bmiHeader;
	}
	else
	{
		return E_INVALIDARG;
	}


	bool hFlip = (pmt->subtype == MEDIASUBTYPE_RGB24 || pmt->subtype == MEDIASUBTYPE_RGB32 || 
				  pmt->subtype == MEDIASUBTYPE_RGB555 || pmt->subtype == MEDIASUBTYPE_RGB565);
	
	m_converter = new CColorSpaceConverter(pmt->subtype, m_bmpInfo.biWidth, m_bmpInfo.biHeight);

	char szDebugInfo[256] = { 0 };
	sprintf(szDebugInfo, "CVideoRender::SetMediaType = %p hFlip = %d", m_hWnd, hFlip);
	OutputDebugStringA(szDebugInfo);

    return m_sdlRenderer->PrepareRenderTarget(m_bmpInfo, m_hWnd, hFlip);
}

CUnknown * WINAPI CVideoRender::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    ASSERT(phr);

    CUnknown *punk = new CVideoRender(lpunk, phr);
    if(punk == NULL)
    {
        if(phr)
            *phr = E_OUTOFMEMORY;
    }
    
	return punk;
} 

HRESULT CVideoRender::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_ISDLWindowlessControl)
    {
        return GetInterface((ISDLWindowlessControl*)this, ppv);
    }
    else if (riid == IID_ISDLFilterConfig)
    {
        return GetInterface((ISDLFilterConfig*)this, ppv);
    }
	
	return CBaseVideoRenderer::NonDelegatingQueryInterface(riid, ppv);
}

STDMETHODIMP CVideoRender::SetVideoClippingWindow(HWND hwnd)
{
    if (!IsWindow(hwnd))
    {
        return E_INVALIDARG;
    }

    m_hWnd = hwnd;
	char szDebugInfo[256] = { 0 };
	sprintf(szDebugInfo, "CVideoRender::SetVideoClippingWindow = %p", hwnd);
	OutputDebugStringA(szDebugInfo);
    m_sdlRenderer->SetVideoClippingWindow(m_hWnd);
    return S_OK;
}

STDMETHODIMP CVideoRender::GetAspectRatioMode(DWORD *lpAspectRatioMode)
{
	m_sdlRenderer->GetAspectRatioMode(lpAspectRatioMode);
	return S_OK;
}


STDMETHODIMP CVideoRender::DisplayModeChanged()
{
	return m_sdlRenderer->DisplayModeChanged();
}


STDMETHODIMP CVideoRender::GetBorderColor(COLORREF *lpClr)
{
	return m_sdlRenderer->GetBorderColor(lpClr);
}

STDMETHODIMP CVideoRender::GetCurrentImage(BYTE **lpDib)
{
	//if (HaveCurrentSample())
	{
		IMediaSample * pMediaSample = GetCurrentSample();

		if (pMediaSample == NULL)
		{
			return E_UNEXPECTED;
		}
		pMediaSample->Release();
	}
	return S_OK;
}

STDMETHODIMP CVideoRender::GetMaxIdealVideoSize(LONG *lpWidth, LONG *lpHeight)
{
	return m_sdlRenderer->GetMaxIdealVideoSize(lpWidth, lpHeight);
}

STDMETHODIMP CVideoRender::GetMinIdealVideoSize(LONG *lpWidth, LONG *lpHeight)
{
	return m_sdlRenderer->GetMinIdealVideoSize(lpWidth, lpHeight);
}

STDMETHODIMP CVideoRender::GetNativeVideoSize(LONG *lpWidth, LONG *lpHeight, LONG *lpARWidth, LONG *lpARHeight)
{
	

	HRESULT hr =  m_sdlRenderer->GetNativeVideoSize(lpWidth, lpHeight, lpARWidth, lpARHeight);
	char szDebugInfo[256] = { 0 };
	sprintf(szDebugInfo, "CVideoRender::GetNativeVideoSize = %p lpWidth/lpHeight = %d:%d lpARWidth/lpARHeight=%d:%d", m_hWnd, *lpWidth, *lpHeight, *lpARWidth, *lpARHeight);
	OutputDebugStringA(szDebugInfo);
	return hr;
}

STDMETHODIMP CVideoRender::GetVideoPosition(LPRECT lpSRCRect, LPRECT lpDSTRect)
{
	long width = 0;
	long height = 0;
	return m_sdlRenderer->GetVideoPosition(lpSRCRect, lpDSTRect);
}


STDMETHODIMP CVideoRender::RepaintVideo(HWND hwnd, HDC  hdc)
{
	return m_sdlRenderer->RepaintVideo(hwnd, hdc);
}

STDMETHODIMP CVideoRender::SetAspectRatioMode(DWORD AspectRatioMode)
{
	return m_sdlRenderer->SetAspectRatioMode(AspectRatioMode);
}



STDMETHODIMP CVideoRender::SetBorderColor(COLORREF lpClr)
{
	return m_sdlRenderer->SetBorderColor(lpClr);
}


STDMETHODIMP CVideoRender::SetVideoPosition(const LPRECT lpSRCRect, const LPRECT lpDSTRect)
{
	char szDebugInfo[256] = { 0 };
	sprintf(szDebugInfo, "CVideoRender::SetVideoPosition = %p top/left = %d:%d bottpm/right=%d:%d", m_hWnd, lpDSTRect->top, lpDSTRect->left, lpDSTRect->bottom, lpDSTRect->right);
	OutputDebugStringA(szDebugInfo);
	HRESULT hr =  m_sdlRenderer->SetVideoPosition(lpSRCRect, lpDSTRect);
	return hr;
}

STDMETHODIMP CVideoRender::SetRenderingMode(DWORD Mode)
{
    return S_OK;
}
