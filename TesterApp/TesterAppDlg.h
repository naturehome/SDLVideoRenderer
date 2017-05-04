
// TesterAppDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "dshow.h"
#include "atlbase.h"
#include "../SDLVideoRenderer/IVideoRenderer.h"

// CTesterAppDlg dialog
class CTesterAppDlg : public CDialogEx
{
// Construction
public:
	CTesterAppDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTERAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CComPtr<IGraphBuilder> m_graph;
	CComPtr<IFilterGraph2> m_filterGraph2;
	CComPtr<IMediaControl> m_mediaCtrl;
	CComPtr<IQualProp>	   m_quality;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton1();
	CStatic m_videoWnd;
};
