#include "SDLRenderer.h"


static int realloc_texture(SDL_Renderer* Renderer, SDL_Texture **texture, Uint32 new_format, int new_width, int new_height, SDL_BlendMode blendmode, int init_texture)
{
    Uint32 format;
    int access, w, h;
    if (SDL_QueryTexture(*texture, &format, &access, &w, &h) < 0 || new_width != w || new_height != h || new_format != format) {
        void *pixels;
        int pitch;
        SDL_DestroyTexture(*texture);
        if (!(*texture = SDL_CreateTexture(Renderer, new_format, SDL_TEXTUREACCESS_STREAMING, new_width, new_height)))
            return -1;
        if (SDL_SetTextureBlendMode(*texture, blendmode) < 0)
            return -1;
        if (init_texture) {
            if (SDL_LockTexture(*texture, NULL, &pixels, &pitch) < 0)
                return -1;
            memset(pixels, 0, pitch * new_height);
            SDL_UnlockTexture(*texture);
        }
    }
    return 0;
}

static long CalculateGCD(long num1, long num2)
{
	int r = num1 % num2;
	while (r != 0)
	{
		num1 = num2;
		num2 = r;
		r = num1 % num2;
	}

	return num2;
}


CSDLRenderer::CSDLRenderer(void)
	:m_hWndTarget(0), m_displayMode(VMR_ARMODE_LETTER_BOX), m_pRenderer(NULL), m_pbmp(NULL), m_BorderColor(RGB(0, 0, 0)), m_pRgb32Buffer(NULL)
{
    int flags = SDL_INIT_VIDEO;
    if (SDL_Init(flags)) {
        exit(1);
    }
}

CSDLRenderer::~CSDLRenderer(void)
{
    SDL_Quit();
}

HRESULT CSDLRenderer::PrepareRenderTarget(BITMAPINFOHEADER& bih, HWND hWnd, bool bFlipHorizontally)
{
    m_pBitmapInfo = bih;
    m_hWndTarget = hWnd;
    m_bFlipHorizontally = bFlipHorizontally;
    m_pitch = m_pBitmapInfo.biWidth * 4;

    DiscardResources();

    return CreateResources();
}

HRESULT	CSDLRenderer::DisplayModeChanged()
{
	return E_NOTIMPL;
}

HRESULT CSDLRenderer::GetCurrentImage(BYTE **lpDib)
{
	return E_NOTIMPL;
}

HRESULT CSDLRenderer::GetNativeVideoSize(LONG *lpWidth, LONG *lpHeight, LONG *lpARWidth, LONG *lpARHeight)
{
	*lpWidth = m_pBitmapInfo.biWidth;
	*lpHeight = abs(m_pBitmapInfo.biHeight);
	LONG nGCD = CalculateGCD(*lpWidth, *lpHeight);
	*lpARWidth = (*lpWidth) / nGCD;
	*lpARHeight = (*lpHeight) / nGCD;
	return S_OK;
}


HRESULT CSDLRenderer::SetVideoClippingWindow(HWND hwnd)
{
	m_hWndTarget = hwnd;
	return S_OK;
}

HRESULT CSDLRenderer::GetAspectRatioMode(DWORD* lpAspectRatioMode)
{
	*lpAspectRatioMode = static_cast<DWORD>(m_displayMode);
	return S_OK;
}

HRESULT CSDLRenderer::SetAspectRatioMode(DWORD AspectRatioMode)
{
	m_displayMode = static_cast<VMR_ASPECT_RATIO_MODE>(AspectRatioMode);
	return S_OK;
}

HRESULT CSDLRenderer::GetBorderColor(COLORREF *lpClr)
{
	*lpClr = m_BorderColor;
	return S_OK;
}

HRESULT CSDLRenderer::SetBorderColor(COLORREF lpClr)
{
	m_BorderColor = lpClr;
	return S_OK;
}

HRESULT CSDLRenderer::GetVideoPosition(LPRECT lpSRCRect, LPRECT lpDSTRect)
{	
	return S_OK;
}


HRESULT CSDLRenderer::GetMaxIdealVideoSize(LONG *lpWidth, LONG *lpHeight)
{
	return E_NOTIMPL;
}

HRESULT CSDLRenderer::GetMinIdealVideoSize(LONG *lpWidth, LONG *lpHeight)
{
	return E_NOTIMPL;
}

HRESULT CSDLRenderer::SetVideoPosition(const LPRECT lpSRCRect, const LPRECT lpDSTRect)
{
	if (lpSRCRect)
		m_srcRect = { lpSRCRect->left, lpSRCRect->top, lpSRCRect->right - lpSRCRect->left, lpSRCRect->bottom - lpSRCRect->top };
	else
		m_srcRect = { 0 };

	if (lpDSTRect)
		m_dstRect = { lpDSTRect->left, lpDSTRect->top, lpDSTRect->right - lpDSTRect->left, lpDSTRect->bottom - lpDSTRect->top };
	else
		m_dstRect = { 0 };

	return S_OK;
}

HRESULT CSDLRenderer::RepaintVideo(HWND hwnd, HDC  hdc)
{
	
	/*if (m_pRenderer)
		SDL_RenderPresent(m_pRenderer);
*/
	return S_OK;
}

HRESULT CSDLRenderer::CreateResources()
{
    SDL_RendererInfo info;
    HRESULT hr = S_OK;

	m_pWindow = SDL_CreateWindowFrom(m_hWndTarget);
    
	m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (m_pRenderer) {
        if (SDL_GetRendererInfo(m_pRenderer, &info))
            return E_UNEXPECTED;
    }

    if (realloc_texture(m_pRenderer, &m_pbmp, SDL_PIXELFORMAT_ARGB8888, m_pBitmapInfo.biWidth, m_pBitmapInfo.biHeight,SDL_BLENDMODE_NONE, 0) < 0) {
        return E_UNEXPECTED;
    }

    return hr;
}

void CSDLRenderer::DiscardResources()
{
    if (m_pbmp) {
        SDL_DestroyTexture(m_pbmp);
        m_pbmp = NULL;
    }

    if (m_pRenderer)
    {
        SDL_DestroyRenderer(m_pRenderer);
        m_pRenderer = NULL;
    }
}


HRESULT CSDLRenderer::DrawSample(const BYTE* pRgb32Buffer)
{
    CheckPointer(pRgb32Buffer, E_POINTER);

	SDL_SetRenderDrawColor(m_pRenderer, GetRValue(m_BorderColor), GetGValue(m_BorderColor), GetBValue(m_BorderColor), 255);
    SDL_RenderClear(m_pRenderer);



	if (m_bFlipHorizontally)
	{
		if (m_pRgb32Buffer == NULL)
			m_pRgb32Buffer = new BYTE[m_pitch*m_pBitmapInfo.biHeight];

		for (size_t i = 0; i < m_pBitmapInfo.biHeight; i++)
		{
			memcpy(m_pRgb32Buffer + i * m_pitch, pRgb32Buffer + (m_pBitmapInfo.biHeight - i - 1)*m_pitch, m_pitch);
		}

		SDL_UpdateTexture(m_pbmp, NULL, m_pRgb32Buffer, m_pitch);
	}
	else
	{
		SDL_UpdateTexture(m_pbmp, NULL, pRgb32Buffer, m_pitch);
	}
	
   
	if (m_dstRect.w != 0 && m_dstRect.h != 0)
	{
		SDL_RenderCopy(m_pRenderer, m_pbmp, NULL, &m_dstRect);
		SDL_RenderPresent(m_pRenderer);
	}

    return S_OK;
}



