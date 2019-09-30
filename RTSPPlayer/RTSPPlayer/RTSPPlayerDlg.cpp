
// RTSPPlayerDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "RTSPPlayer.h"
#include "RTSPPlayerDlg.h"
#include "afxdialogex.h"

#include <time.h>

#pragma warning(disable:4996)

#define SLEEP_500	500
#define SLEEP_100	100

#define GREEN			RGB(16, 56, 0)
#define GREEN_SELECTED	RGB(26, 104, 0)
#define YELLOW			RGB(239, 223, 0)
#define RED				RGB(109, 14, 0)


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum {
	NONE = 0,
	FRONT_CAM = 1,
	LEFT_CAM = 2,
	RIGHT_CAM = 3,
	REER_CAM = 4,
};


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRTSPPlayerDlg 대화 상자



CRTSPPlayerDlg::CRTSPPlayerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RTSPPLAYER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	av_register_all();
	avcodec_register_all();
	avdevice_register_all();
	avformat_network_init();

}

void CRTSPPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCREEN, m_Screen);
	DDX_Control(pDX, IDC_BT_FRONT, m_bt_front);
}

BEGIN_MESSAGE_MAP(CRTSPPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_BT_FRONT, &CRTSPPlayerDlg::OnCustomdrawBtFront)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BT_FRONT, &CRTSPPlayerDlg::OnBnClickedBtFront)
	ON_BN_CLICKED(IDC_BT_LEFT, &CRTSPPlayerDlg::OnBnClickedBtLeft)
	ON_BN_CLICKED(IDC_BT_RIGHT, &CRTSPPlayerDlg::OnBnClickedBtRight)
	ON_BN_CLICKED(IDC_BT_REER, &CRTSPPlayerDlg::OnBnClickedBtReer)
	ON_BN_CLICKED(IDC_BT_RECORD, &CRTSPPlayerDlg::OnBnClickedBtRecord)
END_MESSAGE_MAP()

CString CRTSPPlayerDlg::GetExecutedPath()
{
	CString strResult;
	CString strPath;

	if (GetModuleFileName(nullptr, strPath.GetBuffer(_MAX_PATH + 1), MAX_PATH) != FALSE)
	{
		strPath.ReleaseBuffer();

		strResult = strPath.Left(strPath.ReverseFind('\\') + 1);
	}
	else
	{
	}

	return strResult;
}

// CRTSPPlayerDlg 메시지 처리기

BOOL CRTSPPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	CString ciniPath;
	ciniPath.Format(_T("%sScreen.ini"),GetExecutedPath());
	TCHAR buf[1024] = { 0, };
	int i_x, i_y, i_width, i_height;
	GetPrivateProfileString(_T("SCREEN"), _T("X"),_T(""), buf, 1024, ciniPath);
	i_x = _wtoi(buf);
	GetPrivateProfileString(_T("SCREEN"), _T("Y"), _T(""), buf, 1024, ciniPath);
	i_y = _wtoi(buf);
	GetPrivateProfileString(_T("SCREEN"), _T("width"), _T(""), buf, 1024, ciniPath);
	i_width = _wtoi(buf);
	GetPrivateProfileString(_T("SCREEN"), _T("height"), _T(""), buf, 1024, ciniPath);
	i_height = _wtoi(buf);

	m_Screen.MoveWindow(100, 0, i_width, i_height);
	SetWindowPos(NULL, i_x, i_y, i_width, i_height, SWP_NOZORDER);

	// Button reSize
	int bt_x = 100;
	int bt_y = 50;
	int bt_y_change = 0;
	int bt_y_iterval=5;


	CWnd *pWnd = GetDlgItem(IDC_BT_FRONT);
	pWnd->SetWindowPos(NULL, 0, 0, bt_x, bt_y, SWP_NOZORDER);

	bt_y_change += bt_y + bt_y_iterval;

	pWnd = GetDlgItem(IDC_BT_LEFT);
	pWnd->SetWindowPos(NULL, 0, bt_y_change, bt_x, bt_y, SWP_NOZORDER);

	bt_y_change += bt_y + bt_y_iterval;

	pWnd = GetDlgItem(IDC_BT_RIGHT);
	pWnd->SetWindowPos(NULL, 0, bt_y_change, bt_x, bt_y, SWP_NOZORDER);

	bt_y_change += bt_y + bt_y_iterval;

	pWnd = GetDlgItem(IDC_BT_REER);
	pWnd->SetWindowPos(NULL, 0, bt_y_change, bt_x, bt_y, SWP_NOZORDER);

	bt_y_change += bt_y + bt_y_iterval;

	pWnd = GetDlgItem(IDC_BT_RECORD);
	pWnd->SetWindowPos(NULL, 0, bt_y_change, bt_x, bt_y, SWP_NOZORDER);

	// Button reSize - end


	GetPrivateProfileString(_T("SCREEN"), _T("IP_FRONT"), _T(""), buf, 1024, ciniPath);
	m_strURL = buf;//L"rtsp://192.168.1.100:554/profile1/media.smp";
	
	bThread = true;
	b_record = false;
	playcamnum = FRONT_CAM;

	OnBnClickedBtFront();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRTSPPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CRTSPPlayerDlg::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	if (IsIconic())
	{
		

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		int m_resizeX, m_resizeY;
		m_strBGImagPath.Format(_T("%sBackGround.png"), GetExecutedPath());
		m_BGImg.Destroy();
		m_BGImg.Load(m_strBGImagPath);

		m_resizeX = m_BGImg.GetWidth();
		m_resizeY = m_BGImg.GetHeight();

		if (m_BGImg == NULL)
		{
			AfxMessageBox(_T("Cannot find image file"));
		}

		CString ciniPath;
		ciniPath.Format(_T("%sScreen.ini"), GetExecutedPath());
		TCHAR buf[1024] = { 0, };
		int i_width, i_height;
		GetPrivateProfileString(_T("SCREEN"), _T("width"), _T(""), buf, 1024, ciniPath);
		i_width = _wtoi(buf);
		GetPrivateProfileString(_T("SCREEN"), _T("height"), _T(""), buf, 1024, ciniPath);
		i_height = _wtoi(buf);

		::SetStretchBltMode(dc.m_hDC, COLORONCOLOR);
		m_BGImg.StretchBlt(dc.m_hDC, 0, 0, i_width, i_height, SRCCOPY);

		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CRTSPPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CRTSPPlayerDlg::SDLPlayThread(LPVOID args)
{
	if (NULL != args)
	{
		CRTSPPlayerDlg *pThis = (CRTSPPlayerDlg*)args;

		return pThis->SDLPlayProc(args);
	}
}

void CRTSPPlayerDlg::SDLPlayProc(LPVOID args)
{
	CRTSPPlayerDlg* dlg = (CRTSPPlayerDlg*)args;

	// 유니코드 to 멀티바이트
	int iLength = m_strURL.GetLength();
	int iBytes = WideCharToMultiByte(CP_ACP, 0, m_strURL, iLength, NULL, 0, NULL, NULL);
	char* strFilePath = new char[iBytes + 1];
	memset(strFilePath, 0, iLength + 1);
	WideCharToMultiByte(CP_OEMCP,0, m_strURL, iLength, strFilePath, iBytes, NULL, NULL);

	av_register_all();
	avformat_network_init();

	

	while (dlg->bThread)
	{
		AVFormatContext* pFormatCtx = NULL;
		pFormatCtx = avformat_alloc_context();

		// TCP, timeout 옵션지정
		AVDictionary* opts = 0;
		av_dict_set(&opts, "rtsp_transport", "tcp", 0);
		av_dict_set(&opts, "stimeout", "1000*1000", 0);

		// RTSP Stream Open
		if (avformat_open_input(&pFormatCtx, strFilePath, NULL, &opts) != 0)
		{
			Sleep(SLEEP_500);
			continue;
		}

		av_dict_free(&opts);
				
		pFormatCtx->interrupt_callback.opaque = pFormatCtx;
		if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
		{
			Sleep(SLEEP_500);
			continue;		// Couldn't find stream information
		}

		av_dump_format(pFormatCtx, 0, strFilePath, 0);
		int videoStream = -1;
		for (int i = 0; i < pFormatCtx->nb_streams; i++)
		{
			if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				videoStream = i;
				break;
			}
		}

		if (videoStream == -1)
		{
			Sleep(SLEEP_100);
			continue;
		}

		// 비디오 코덱을 얻는다.
		AVCodecContext * pCodexCtx = pFormatCtx->streams[videoStream]->codec;

		// 코덱에 대하여 디코더가 있는지 확인
		AVCodec* pCodec = avcodec_find_decoder(pCodexCtx->codec_id);
		if (pCodec == NULL)
		{
			printf("UnSupport Codec");
			Sleep(SLEEP_100);
			continue;
		}

		// 코덱을 열고
		AVDictionary* optionsDict = NULL;
		if (avcodec_open2(pCodexCtx, pCodec, &optionsDict) < 0)
		{
			Sleep(SLEEP_100);
			continue;
		}

		av_dict_free(&optionsDict);

		// Source color format
		AVPixelFormat src_fix_fmt = pCodexCtx->pix_fmt;

		// 
		AVPixelFormat dst_fix_fmt = AV_PIX_FMT_YUV420P;

		AVFrame* pFrame = av_frame_alloc();
		AVFrame* pFrameVUV = av_frame_alloc();

		if (pFrameVUV == NULL)
		{
			Sleep(SLEEP_100);
			continue;
		}

		if (pCodexCtx->width <= 0)
		{
			Sleep(SLEEP_100);
			continue;
		}

		struct SwsContext* sws_clx = sws_getContext(
			pCodexCtx->width,
			pCodexCtx->height,
			pCodexCtx->pix_fmt,
			pCodexCtx->width,
			pCodexCtx->height,
			dst_fix_fmt,
			SWS_BILINEAR,
			NULL,
			NULL,
			NULL);

		int numBytes = avpicture_get_size(dst_fix_fmt, pCodexCtx->width, pCodexCtx->height);
		uint8_t* buffer = (uint8_t*)av_malloc(numBytes + sizeof(uint8_t));

		avpicture_fill((AVPicture*)pFrameVUV, buffer, dst_fix_fmt, pCodexCtx->width, pCodexCtx->height);

		SDL_Rect sdlRect;
		sdlRect.x = 0;
		sdlRect.y = 0;
		sdlRect.w = pCodexCtx->width;
		sdlRect.h = pCodexCtx->height;

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
		{
			Sleep(SLEEP_100);
			continue;
		}

		SDL_Window* sdlWindow = SDL_CreateWindowFrom(dlg->m_Screen.GetSafeHwnd());

		if (!sdlWindow)
		{
			Sleep(SLEEP_100);
			continue;
		}

		SDL_Renderer* sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

		SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STATIC, pCodexCtx->width, pCodexCtx->height);

		if (!sdlTexture)
		{
			Sleep(SLEEP_100);
			continue;
		}

		SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);

		AVPacket packet;
		SDL_Event event;

		// Stream read
		while (dlg->bThread)
		{
			if (av_read_frame(pFormatCtx, &packet) < 0)
			{
				Sleep(SLEEP_100);
				break;
			}

			if (packet.stream_index == videoStream)
			{
				int frameFinished;
				avcodec_decode_video2(pCodexCtx, pFrame, &frameFinished, &packet);

				if (frameFinished)
				{
					sws_scale(sws_clx, (uint8_t const* const *)pFrame->data, pFrame->linesize, 0, pCodexCtx->height, pFrameVUV->data, pFrameVUV->linesize);


					SDL_UpdateTexture(sdlTexture, &sdlRect, pFrameVUV->data[0], pFrameVUV->linesize[0]);
					SDL_RenderClear(sdlRenderer);
					SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
					SDL_RenderPresent(sdlRenderer);
				}
			}
			av_free_packet(&packet);
			SDL_PollEvent(&event);
		}

		SDL_DestroyWindow(sdlWindow);
		SDL_DestroyRenderer(sdlRenderer);
		SDL_DestroyTexture(sdlTexture);
		SDL_Quit();

		sws_freeContext(sws_clx);
		dlg->m_Screen.ShowWindow(SW_SHOWNORMAL);

		av_free(pFrame);
		av_free(pFrameVUV);

		avcodec_close(pCodexCtx);
		avformat_close_input(&pFormatCtx);

		av_free(buffer);
		
	}

}

void CRTSPPlayerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();


	b_record = false;
	bThread = false;

	Sleep(1000);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CRTSPPlayerDlg::OnCustomdrawBtFront(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CRTSPPlayerDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (nIDCtl == IDC_BT_FRONT || IDC_BT_LEFT || IDC_BT_RIGHT || IDC_BT_REER || IDC_BT_RECORD)
	{
		CDC dc;
		CRect rect;
		
		dc.Attach(lpDrawItemStruct->hDC);
		rect = lpDrawItemStruct->rcItem;

		dc.Draw3dRect(rect, RGB(255, 0, 0), RGB(0, 255, 0));		
		dc.FillSolidRect(&rect, GREEN);

	
		UINT state = lpDrawItemStruct->itemState;

		if ((state & ODS_SELECTED))
		{
			//dc.DrawEdge(&rect, EDGE_SUNKEN, BF_RECT);
			dc.Draw3dRect(rect, YELLOW, YELLOW);
		}
		else if (state & ODS_DISABLED)
		{
			dc.Draw3dRect(rect, RED, RED);
		}
		else
		{
			//dc.DrawEdge(&rect, EDGE_RAISED, BF_RECT);
			dc.Draw3dRect(rect, GREEN, GREEN);
		}

		dc.SetBkColor(GREEN);
		dc.SetTextColor(YELLOW);

		TCHAR buffer[MAX_PATH];           //To store the Caption of the button.
		ZeroMemory(buffer, MAX_PATH);     //Intializing the buffer to zero
		::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH); //Get the Caption of Button Window 

		dc.DrawText(buffer, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//Redraw the  Caption of Button Window 

		dc.Detach();  // Detach the Button DC
	}
	else
	{
		CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}


void CRTSPPlayerDlg::OnBnClickedBtFront()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	killThreadFn();

	CString ciniPath;
	ciniPath.Format(_T("%sScreen.ini"), GetExecutedPath());
	TCHAR buf[1024] = { 0, };
	GetPrivateProfileString(_T("SCREEN"), _T("IP_FRONT"), _T(""), buf, 1024, ciniPath);
	m_strURL = buf;

	pThreadSDLPlay = AfxBeginThread((AFX_THREADPROC)SDLPlayThread, (LPVOID)this);

	playcamnum = FRONT_CAM;

	if (pThreadSDLPlay)
	{
		bThread = true;

		CButton *pBT = (CButton*)GetDlgItem(IDC_BT_FRONT);
		BOOL breturn = pBT->EnableWindow(false);

		if (!breturn)
		{
			pBT = (CButton*)GetDlgItem(IDC_BT_LEFT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_RIGHT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_REER);
			pBT->EnableWindow(true);
		}
	}
}


void CRTSPPlayerDlg::OnBnClickedBtLeft()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	killThreadFn();

	CString ciniPath;
	ciniPath.Format(_T("%sScreen.ini"), GetExecutedPath());
	TCHAR buf[1024] = { 0, };
	GetPrivateProfileString(_T("SCREEN"), _T("IP_LEFT"), _T(""), buf, 1024, ciniPath);
	m_strURL = buf;

	pThreadSDLPlay = AfxBeginThread((AFX_THREADPROC)SDLPlayThread, (LPVOID)this);

	playcamnum = LEFT_CAM;

	if (pThreadSDLPlay)
	{
		bThread = true;

		CButton *pBT = (CButton*)GetDlgItem(IDC_BT_LEFT);
		BOOL breturn = pBT->EnableWindow(false);

		if (!breturn)
		{
			pBT = (CButton*)GetDlgItem(IDC_BT_FRONT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_RIGHT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_REER);
			pBT->EnableWindow(true);
		}
	}
}


void CRTSPPlayerDlg::OnBnClickedBtRight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	killThreadFn();

	CString ciniPath;
	ciniPath.Format(_T("%sScreen.ini"), GetExecutedPath());
	TCHAR buf[1024] = { 0, };
	GetPrivateProfileString(_T("SCREEN"), _T("IP_RIGHT"), _T(""), buf, 1024, ciniPath);
	m_strURL = buf;

	pThreadSDLPlay = AfxBeginThread((AFX_THREADPROC)SDLPlayThread, (LPVOID)this);

	playcamnum = RIGHT_CAM;

	if (pThreadSDLPlay)
	{
		bThread = true;

		CButton *pBT = (CButton*)GetDlgItem(IDC_BT_RIGHT);
		BOOL breturn = pBT->EnableWindow(false);

		if (!breturn)
		{
			pBT = (CButton*)GetDlgItem(IDC_BT_LEFT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_FRONT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_REER);
			pBT->EnableWindow(true);
		}
	}
}


void CRTSPPlayerDlg::OnBnClickedBtReer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	killThreadFn();

	CString ciniPath;
	ciniPath.Format(_T("%sScreen.ini"), GetExecutedPath());
	TCHAR buf[1024] = { 0, };
	GetPrivateProfileString(_T("SCREEN"), _T("IP_REER"), _T(""), buf, 1024, ciniPath);
	m_strURL = buf;

	pThreadSDLPlay = AfxBeginThread((AFX_THREADPROC)SDLPlayThread, (LPVOID)this);

	playcamnum = REER_CAM;

	if (pThreadSDLPlay)
	{
		bThread = true;

		CButton *pBT = (CButton*)GetDlgItem(IDC_BT_REER);
		BOOL breturn = pBT->EnableWindow(false);

		if (!breturn)
		{
			pBT = (CButton*)GetDlgItem(IDC_BT_LEFT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_RIGHT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_FRONT);
			pBT->EnableWindow(true);
		}
	}
}


void CRTSPPlayerDlg::OnBnClickedBtRecord()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pBT = (CButton*)GetDlgItem(IDC_BT_RECORD);
	

	if (!b_record)
	{
		b_record = true;

		pThreadRecord = AfxBeginThread((AFX_THREADPROC)RecordThread, (LPVOID)this);

		pBT = (CButton*)GetDlgItem(IDC_BT_FRONT);
		pBT->EnableWindow(false);

		pBT = (CButton*)GetDlgItem(IDC_BT_LEFT);
		pBT->EnableWindow(false);

		pBT = (CButton*)GetDlgItem(IDC_BT_RIGHT);
		pBT->EnableWindow(false);

		pBT = (CButton*)GetDlgItem(IDC_BT_REER);
		pBT->EnableWindow(false);
	}
	else
	{
		b_record = false;


		if (playcamnum == FRONT_CAM)
		{
			pBT = (CButton*)GetDlgItem(IDC_BT_FRONT);
			pBT->EnableWindow(false);

			pBT = (CButton*)GetDlgItem(IDC_BT_LEFT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_RIGHT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_REER);
			pBT->EnableWindow(true);
		}
		else if (playcamnum == LEFT_CAM)
		{
			pBT = (CButton*)GetDlgItem(IDC_BT_FRONT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_LEFT);
			pBT->EnableWindow(false);

			pBT = (CButton*)GetDlgItem(IDC_BT_RIGHT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_REER);
			pBT->EnableWindow(true);
		}
		else if (playcamnum == RIGHT_CAM)
		{
			pBT = (CButton*)GetDlgItem(IDC_BT_FRONT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_LEFT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_RIGHT);
			pBT->EnableWindow(false);

			pBT = (CButton*)GetDlgItem(IDC_BT_REER);
			pBT->EnableWindow(true);
		}
		else if (playcamnum == REER_CAM)
		{
			pBT = (CButton*)GetDlgItem(IDC_BT_FRONT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_LEFT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_RIGHT);
			pBT->EnableWindow(true);

			pBT = (CButton*)GetDlgItem(IDC_BT_REER);
			pBT->EnableWindow(false);
		}
		else
		{

		}
	}
}


void CRTSPPlayerDlg::RecordThread(LPVOID args)
{
	if (NULL != args)
	{
		CRTSPPlayerDlg *pThis = (CRTSPPlayerDlg*)args;

		while (pThis->b_record)
		{
			int value = 0;
			AVDictionary* options = NULL;
			AVFormatContext *pAVFormatContext = NULL;

			pAVFormatContext = avformat_alloc_context();//Allocate an AVFormatContext.

			// 유니코드 to 멀티바이트
			/*
			CString ciniPath;
			ciniPath.Format(_T("%sScreen.ini"), pThis->GetExecutedPath());
			TCHAR buf[1024] = { 0, };
			GetPrivateProfileString(_T("SCREEN"), _T("IP_REER"), _T(""), buf, 1024, ciniPath);
			
			CString m_strURL = buf;
			*/
			int iLength = pThis->m_strURL.GetLength();
			int iBytes = WideCharToMultiByte(CP_ACP, 0, pThis->m_strURL, iLength, NULL, 0, NULL, NULL);
			char* strFilePath = new char[iBytes + 1];
			memset(strFilePath, 0, iLength + 1);
			
			WideCharToMultiByte(CP_OEMCP, 0, pThis->m_strURL, iLength, strFilePath, iBytes, NULL, NULL);

			/* set option */
			value = av_dict_set(&options, "rtsp_transport", "tcp", 0);
			value = av_dict_set(&options, "stimeout", "1000*1000", 0);
			value = av_dict_set(&options, "framerate", "30", 0);

			if (value < 0)
			{
				//AfxMessageBox(_T("error in setting dictionary value"));
				continue;
			}
			value = av_dict_set(&options, "preset", "medium", 0);
			if (value < 0)
			{
				//AfxMessageBox(_T("error in setting preset values"));
				continue;
			}

			// RTSP Stream Open
			value = avformat_open_input(&pAVFormatContext, strFilePath, NULL, &options);
			if (value != 0)
			{
				//AfxMessageBox(_T("RTSP open Error"));
				continue;
			}

			value = avformat_find_stream_info(pAVFormatContext, NULL);
			if (value < 0)
			{
				//AfxMessageBox(_T("unable to find the stream information"));
				continue;
			}

			int VideoStreamIndx = -1;

			/* find the first video stream index . Also there is an API available to do the below operations */
			for (int i = 0; i < pAVFormatContext->nb_streams; i++) // find video stream posistion/index.
			{
				if (pAVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
				{
					VideoStreamIndx = i;
					break;
				}
			}

			if (VideoStreamIndx == -1)
			{
				//AfxMessageBox(_T("unable to find the video stream index. (-1)"));
				continue;
			}
			AVCodecContext *pAVCodecContext;
			AVCodec *pAVCodec;

			pAVCodecContext = pAVFormatContext->streams[VideoStreamIndx]->codec;

			pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
			if (pAVCodec == NULL)
			{
				//AfxMessageBox(_T("unable to find the decoder"));
				continue;
			}

			value = avcodec_open2(pAVCodecContext, pAVCodec, NULL);//Initialize the AVCodecContext to use the given AVCodec.
			if (value < 0)
			{
				//AfxMessageBox(_T("unable to open the av codec"));
				continue;
			}


			// init_outputfile() Func Start
			AVFormatContext *outAVFormatContext = NULL;
			value = 0;

			// Get File Name
			CString soutput_file;
			time_t current_time;
			struct tm  local_time;

			time(&current_time);
			localtime_s(&local_time, &current_time);

			int Year = local_time.tm_year +1900;
			int Month = local_time.tm_mon + 1;
			int Day = local_time.tm_mday;

			int Hour = local_time.tm_hour;
			int Min = local_time.tm_min;
			int Sec = local_time.tm_sec;

			soutput_file.Format(_T("%d.%d.%d_%d.%d.%d_Record.mp4"), Year, Month, Day, Hour, Min, Sec);
			//soutput_file.Format(_T("output.mp4"));

			const char *output_file;
			CStringA aoutput_file(soutput_file);
			output_file = aoutput_file;
	

			avformat_alloc_output_context2(&outAVFormatContext, NULL, NULL, output_file);
			if (!outAVFormatContext)
			{
				//AfxMessageBox(_T("error in allocating av format output context"));
				continue;
			}

			AVOutputFormat *output_format;

			/* Returns the output format in the list of registered output formats which best matches the provided parameters, or returns NULL if there is no match. */
			output_format = av_guess_format(NULL, output_file, NULL);
			if (!output_format)
			{
				//AfxMessageBox(_T("error in guessing the video format. try with correct format"));
				continue;
			}

			AVStream *video_st;

			video_st = avformat_new_stream(outAVFormatContext, NULL);
			if (!video_st)
			{
				//AfxMessageBox(_T("error in creating a av format new stream"));
				continue;
			}

			AVCodecContext *outAVCodecContext;
			AVCodec *outAVCodec;
			outAVCodec = NULL;

			outAVCodecContext = avcodec_alloc_context3(outAVCodec);
			if (!outAVCodecContext)
			{
				//AfxMessageBox(_T("error in allocating the codec contexts"));
				continue;
			}

			/* set property of the video file */
			outAVCodecContext = video_st->codec;
			outAVCodecContext->codec_id = AV_CODEC_ID_MPEG4;// AV_CODEC_ID_MPEG4; // AV_CODEC_ID_H264 // AV_CODEC_ID_MPEG1VIDEO
			outAVCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
			outAVCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
			outAVCodecContext->bit_rate = 4000000; // 2500000
			outAVCodecContext->width = 1920;
			outAVCodecContext->height = 1080;
			outAVCodecContext->max_b_frames = 2;
			outAVCodecContext->time_base.num = 1;
			//outAVCodecContext->time_base.den = pAVCodecContext->time_base.den;//video_st->time_base.den; 5;		// 테스트용 IPCamera의 경우 5프레, 실제 환경에 따라 설정을 이후에 변경하자
			outAVCodecContext->time_base.den = 1.0 / av_q2d(pAVCodecContext->time_base) / FFMAX(pAVCodecContext->ticks_per_frame, 1);

			outAVCodec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
			if (!outAVCodec)
			{
				//AfxMessageBox(_T("error in finding the av codecs. try again with correct codec"));
				continue;
			}

			/* Some container formats (like MP4) require global headers to be present Mark the encoder so that it behaves accordingly. */

			if (outAVFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
			{
				outAVCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
			}

			value = avcodec_open2(outAVCodecContext, outAVCodec, NULL);
			if (value < 0)
			{
				//AfxMessageBox(_T("error in opening the avcodec"));
				continue;
			}

			/* create empty video file */
			if (!(outAVFormatContext->flags & AVFMT_NOFILE))
			{
				if (avio_open2(&outAVFormatContext->pb, output_file, AVIO_FLAG_WRITE, NULL, NULL) < 0)
				{
					//AfxMessageBox(_T("error in creating the video file"));
					continue;
				}
			}

			if (!outAVFormatContext->nb_streams)
			{
				//AfxMessageBox(_T("output file dose not contain any stream"));
				continue;
			}

			/* imp: mp4 container or some advanced container file required header information*/
			value = avformat_write_header(outAVFormatContext, &options);
			if (value < 0)
			{
				//AfxMessageBox(_T("error in writing the header context"));
				continue;
			}

			int flag;
			int frameFinished;//when you decode a single packet, you still don't have information enough to have a frame [depending on the type of codec, some of them //you do], when you decode a GROUP of packets that represents a frame, then you have a picture! that's why frameFinished will let //you know you decoded enough to have a frame.

			int frame_index = 0;
			value = 0;

			AVPacket *pAVPacket;

			pAVPacket = (AVPacket *)av_malloc(sizeof(AVPacket));
			av_init_packet(pAVPacket);

			AVFrame *pAVFrame;

			pAVFrame = av_frame_alloc();
			if (!pAVFrame)
			{
				//AfxMessageBox(_T("unable to release the avframe resources"));
				continue;
			}

			AVFrame *outFrame;

			outFrame = av_frame_alloc();//Allocate an AVFrame and set its fields to default values.
			if (!outFrame)
			{
				//AfxMessageBox(_T("unable to release the avframe resources for outframe"));
				continue;
			}

			int video_outbuf_size;
			int nbytes = av_image_get_buffer_size(outAVCodecContext->pix_fmt, outAVCodecContext->width, outAVCodecContext->height, 32);
			uint8_t *video_outbuf = (uint8_t*)av_malloc(nbytes);
			if (video_outbuf == NULL)
			{
				//AfxMessageBox(_T("unable to allocate memory"));
				continue;
			}

			// Setup the data pointers and linesizes based on the specified image parameters and the provided array.
			value = av_image_fill_arrays(outFrame->data, outFrame->linesize, video_outbuf, AV_PIX_FMT_YUV420P, outAVCodecContext->width, outAVCodecContext->height, 1); // returns : the size in bytes required for src
			if (value < 0)
			{
				//AfxMessageBox(_T("error in filling image array"));
			}

			SwsContext* swsCtx_;

			// Allocate and return swsContext.
			// a pointer to an allocated context, or NULL in case of error
			// Deprecated : Use sws_getCachedContext() instead.
			swsCtx_ = sws_getContext(pAVCodecContext->width,
				pAVCodecContext->height,
				pAVCodecContext->pix_fmt,
				outAVCodecContext->width,
				outAVCodecContext->height,
				outAVCodecContext->pix_fmt,
				SWS_BICUBIC, NULL, NULL, NULL);

			AVPacket outPacket;
			int got_picture;

			while ((av_read_frame(pAVFormatContext, pAVPacket) >= 0)&&(pThis->b_record))
			{
				if (pAVPacket->stream_index == VideoStreamIndx)
				{
					value = avcodec_decode_video2(pAVCodecContext, pAVFrame, &frameFinished, pAVPacket);
					if (value < 0)
					{
						//AfxMessageBox(_T("unable to decode video"));
					}

					if (frameFinished)// Frame successfully decoded :)
					{
						sws_scale(swsCtx_, pAVFrame->data, pAVFrame->linesize, 0, pAVCodecContext->height, outFrame->data, outFrame->linesize);
						av_init_packet(&outPacket);
						outPacket.data = NULL;    // packet data will be allocated by the encoder
						outPacket.size = 0;

						avcodec_encode_video2(outAVCodecContext, &outPacket, outFrame, &got_picture);

						if (got_picture)
						{
							if (outPacket.pts != AV_NOPTS_VALUE)
								//outPacket.pts = av_rescale_q(outPacket.pts, video_st->codec->time_base, video_st->time_base);
								outPacket.pts = av_rescale_q(outPacket.pts, video_st->codec->time_base, video_st->time_base);
							if (outPacket.dts != AV_NOPTS_VALUE)
								outPacket.dts = av_rescale_q(outPacket.dts, video_st->codec->time_base, video_st->time_base);

							//printf("Write frame %3d (size= %2d)\n", j++, outPacket.size / 1000);
							if (av_write_frame(outAVFormatContext, &outPacket) != 0)
							{
								AfxMessageBox(_T("error in writing video frame"));
							}

							av_packet_unref(&outPacket);
						} // got_picture
						av_packet_unref(&outPacket);
					} // frameFinished

					// av_free_packet 없으면 메모리릭 발생 주의!
					av_free_packet(pAVPacket);
				}

			}// End of while-loop

			value = av_write_trailer(outAVFormatContext);
			if (value < 0)
			{
				//AfxMessageBox(_T("error in writing av trailer"));
				//return 0;
			}

			//THIS WAS ADDED LATER
			av_free(video_outbuf);

			//
		}

		pThis->killRecordThreadFn();
	}
}

void CRTSPPlayerDlg::killThreadFn()
{
	if (pThreadSDLPlay == NULL)
	{
		// Thread 없음
	}
	else
	{
		// Thread 삭제
		pThreadSDLPlay->SuspendThread();

		DWORD dwResult;
		::GetExitCodeThread(pThreadSDLPlay->m_hThread, &dwResult);

		delete pThreadSDLPlay;
		pThreadSDLPlay = NULL;

		bThread = false;
	}
}

void CRTSPPlayerDlg::killRecordThreadFn()
{
	if (pThreadRecord == NULL)
	{
		// Thread 없음
	}
	else
	{
		pThreadRecord->SuspendThread();

		DWORD dwResult;
		::GetExitCodeThread(pThreadRecord->m_hThread, &dwResult);

		delete pThreadRecord;
		pThreadRecord = NULL;
	}
}