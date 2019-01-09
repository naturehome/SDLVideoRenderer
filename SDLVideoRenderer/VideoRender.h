#pragma once

//#include "D2DRenderer.h"
#include "SDLRenderer.h"
#include "IVideoRenderer.h"
#include "VideoWindow.h"
#include "Dvdmedia.h"
#include "ColorSpaceConverter.h"

class CVideoRender : public CBaseVideoRenderer/*, public  CBaseControlVideo*/, public ISDLWindowlessControl, public ISDLFilterConfig
{
public:
	DECLARE_IUNKNOWN;
	
	CVideoRender(LPUNKNOWN pUnk, HRESULT* phr);
    virtual ~CVideoRender(void);

	virtual HRESULT DoRenderSample(IMediaSample *pMediaSample);
    virtual HRESULT CheckMediaType(const CMediaType *pmt);
	virtual HRESULT SetMediaType(const CMediaType *pmt);
	virtual HRESULT StartStreaming();

	//virtual HRESULT IsDefaultTargetRect();
	//virtual HRESULT SetDefaultTargetRect();
	//virtual HRESULT SetTargetRect(RECT *pTargetRect);
	//virtual HRESULT GetTargetRect(RECT *pTargetRect);
	//virtual HRESULT IsDefaultSourceRect();
	//virtual HRESULT SetDefaultSourceRect();
	//virtual HRESULT SetSourceRect(RECT *pSourceRect);
	//virtual HRESULT GetSourceRect(RECT *pSourceRect);
	//virtual HRESULT GetStaticImage(__inout long *pBufferSize, __out_bcount_part(*pBufferSize, *pBufferSize) long *pDIBImage);
	//virtual VIDEOINFOHEADER *GetVideoFormat();

	static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr); 
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	STDMETHOD(DisplayModeChanged)();
	STDMETHOD(GetAspectRatioMode)(DWORD *lpAspectRatioMode);
	STDMETHOD(GetBorderColor)(COLORREF *lpClr);
	STDMETHOD(GetCurrentImage)(BYTE **lpDib);
	STDMETHOD(GetMaxIdealVideoSize)(LONG *lpWidth, LONG *lpHeight);
	STDMETHOD(GetMinIdealVideoSize)(LONG *lpWidth, LONG *lpHeight);
	STDMETHOD(GetNativeVideoSize)(LONG *lpWidth, LONG *lpHeight, LONG *lpARWidth, LONG *lpARHeight);
	STDMETHOD(GetVideoPosition)(LPRECT lpSRCRect, LPRECT lpDSTRect);
	STDMETHOD(RepaintVideo)(HWND hwnd, HDC  hdc);
	STDMETHOD(SetAspectRatioMode)(DWORD AspectRatioMode);
	STDMETHOD(SetVideoClippingWindow)(HWND hwnd);
	STDMETHOD(SetBorderColor)(COLORREF lpClr);
	STDMETHOD(SetVideoPosition)(const LPRECT lpSRCRect, const LPRECT lpDSTRect);
	STDMETHOD(SetRenderingMode)(DWORD Mode);

	void CreateDefaultWindow();

private:
	HWND m_hWnd;
    CSDLRenderer* m_sdlRenderer;
	HANDLE m_event;
	BITMAPINFOHEADER m_bmpInfo;
	CMediaType m_mediaType;
	CColorSpaceConverter* m_converter;
};


