
// RTSPPlayerDlg.h: 헤더 파일
//

#pragma once

// for deprecated로 선언되었습니다.
#pragma warning(disable:4996)

//FFMPEG LIBRARIES
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavcodec/avfft.h"

#include "libavdevice/avdevice.h"

#include "libavfilter/avfilter.h"
	//#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"

#include "libavformat/avformat.h"
#include "libavformat/avio.h"

// libav resample

#include "libavutil/opt.h"
#include "libavutil/common.h"
#include "libavutil/channel_layout.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
#include "libavutil/time.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libavutil/file.h"

// lib swresample

#include "libswscale/swscale.h"
}

#include <SDL.h>
#include <SDL_thread.h>

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "swresample.lib")

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
	CStatic m_Screen;

	CWinThread* pThreadSDLPlay;
	static void SDLPlayThread(LPVOID args);
	void SDLPlayProc(LPVOID args);
	bool bThread;

	CString m_strURL;
	afx_msg void OnDestroy();
	CString GetExecutedPath();

	CString m_strBGImagPath;
	CImage m_BGImg;
	afx_msg void OnCustomdrawBtFront(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_bt_front;
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnBnClickedBtFront();
	afx_msg void OnBnClickedBtLeft();
	afx_msg void OnBnClickedBtRight();
	afx_msg void OnBnClickedBtReer();
	afx_msg void OnBnClickedBtRecord();

	unsigned int playcamnum;
	bool b_record;

	CWinThread* pThreadRecord;
	static void RecordThread(LPVOID args);

	void killThreadFn();
	void killRecordThreadFn();
};
