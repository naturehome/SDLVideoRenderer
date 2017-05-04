#pragma once


// {3865E38F-F54D-4EB6-8207-28441952E7EE}
static const GUID CLSID_SDLVideoRenderer = 
{ 0x3865e38f, 0xf54d, 0x4eb6, { 0x82, 0x7, 0x28, 0x44, 0x19, 0x52, 0xe7, 0xee } };

// {34E5B77C-CCBA-4EC0-88B5-BABF6CF3A1D2}
static const GUID IID_ISDLVideoRenderer =
{ 0x34e5b77c, 0xccba, 0x4ec0, { 0x88, 0xb5, 0xba, 0xbf, 0x6c, 0xf3, 0xa1, 0xd2 } };

// {E0B20D0A-DD25-4FDA-A6A7-B5E92EE24B6B}
static const GUID IID_ISDLWindowlessControl =
{ 0xe0b20d0a, 0xdd25, 0x4fda, { 0xa6, 0xa7, 0xb5, 0xe9, 0x2e, 0xe2, 0x4b, 0x6b } };

// {9E10640D-7C92-465C-AFB4-F57F93731A20}
static const GUID IID_ISDLFilterConfig =
{ 0x9e10640d, 0x7c92, 0x465c, { 0xaf, 0xb4, 0xf5, 0x7f, 0x93, 0x73, 0x1a, 0x20 } };

#define FILTER_NAME L"SDL Video Renderer"


DECLARE_INTERFACE_(ISDLWindowlessControl, IUnknown)
{
	STDMETHOD(DisplayModeChanged)() PURE;
	STDMETHOD(GetAspectRatioMode)(DWORD *lpAspectRatioMode) PURE;
	STDMETHOD(GetBorderColor)(COLORREF *lpClr)PURE;
	STDMETHOD(GetCurrentImage)(BYTE **lpDib) PURE;
	STDMETHOD(GetMaxIdealVideoSize)(LONG *lpWidth, LONG *lpHeight) PURE;
	STDMETHOD(GetMinIdealVideoSize)(LONG *lpWidth, LONG *lpHeight) PURE;
    STDMETHOD(GetNativeVideoSize)(LONG *lpWidth, LONG *lpHeight, LONG *lpARWidth, LONG *lpARHeight) PURE;
	STDMETHOD(GetVideoPosition)(LPRECT lpSRCRect, LPRECT lpDSTRect) PURE;
	STDMETHOD(RepaintVideo)(HWND hwnd, HDC  hdc) PURE;
	STDMETHOD(SetAspectRatioMode)(DWORD AspectRatioMode) PURE;
    STDMETHOD(SetVideoClippingWindow)(HWND hwnd) PURE;
	STDMETHOD(SetBorderColor)(COLORREF lpClr) PURE;
    STDMETHOD(SetVideoPosition)(const LPRECT lpSRCRect,const LPRECT lpDSTRect) PURE;
};

DECLARE_INTERFACE_(ISDLFilterConfig, IUnknown)
{
    STDMETHOD(SetRenderingMode)(DWORD Mode) PURE;
};