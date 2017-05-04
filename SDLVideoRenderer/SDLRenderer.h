#pragma once

#include "streams.h"
#include <sdl.h>
#include "IVideoRenderer.h"
#include "atlbase.h"
#include <math.h>


class CSDLRenderer
{
public:
    	CSDLRenderer(void);
    	virtual ~CSDLRenderer(void);

   	HRESULT PrepareRenderTarget(BITMAPINFOHEADER& bih, HWND hWnd, bool bFlipHorizontally);
    	HRESULT DrawSample(const BYTE* pRgb32Buffer);
	HRESULT	DisplayModeChanged();
	HRESULT GetAspectRatioMode(DWORD *lpAspectRatioMode);
	HRESULT GetBorderColor(COLORREF *lpClr);
	HRESULT GetCurrentImage(BYTE **lpDib);
	HRESULT GetMaxIdealVideoSize(LONG *lpWidth, LONG *lpHeight);
	HRESULT GetMinIdealVideoSize(LONG *lpWidth, LONG *lpHeight);
	HRESULT GetNativeVideoSize(LONG *lpWidth, LONG *lpHeight, LONG *lpARWidth, LONG *lpARHeight);
	HRESULT GetVideoPosition(LPRECT lpSRCRect, LPRECT lpDSTRect);
	HRESULT RepaintVideo(HWND hwnd, HDC  hdc);
	HRESULT SetAspectRatioMode(DWORD AspectRatioMode);
	HRESULT SetVideoClippingWindow(HWND hwnd);
	HRESULT SetBorderColor(COLORREF lpClr);
	HRESULT SetVideoPosition(const LPRECT lpSRCRect, const LPRECT lpDSTRect);

private:
    	HRESULT CreateResources();
    	void    DiscardResources();

private:
	SDL_Window*		m_pWindow;
	SDL_Renderer*		m_pRenderer;
    	HWND			m_hWndTarget;
    	SDL_Texture*		m_pbmp;
    	UINT32			m_pitch;
	VMR_ASPECT_RATIO_MODE	m_displayMode;
    	BITMAPINFOHEADER	m_pBitmapInfo;
	SDL_Rect		m_srcRect;
	SDL_Rect		m_dstRect;
	COLORREF		m_BorderColor;
    	bool			m_bFlipHorizontally;
    	bool			m_bWndChange;
	BYTE*			m_pRgb32Buffer;
};
