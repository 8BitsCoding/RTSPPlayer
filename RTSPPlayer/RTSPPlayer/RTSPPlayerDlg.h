
// RTSPPlayerDlg.h: 헤더 파일
//

#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
};

#include <SDL.h>
#include <SDL_thread.h>

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "SDL2.lib")

#include "Windows.h"

// CRTSPPlayerDlg 대화 상자
class CRTSPPlayerDlg : public CDialogEx
{
// 생성입니다.
public:
	CRTSPPlayerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RTSPPLAYER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPlay();
	CStatic m_Screen;

	CWinThread* pThreadSDLPlay;
	static UINT SDLPlayThread(LPVOID args);
	int SDLPlayProc(LPVOID args);

	CString m_strURL;
	afx_msg void OnBnClickedOk();
};
