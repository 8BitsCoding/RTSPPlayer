
// RTSPPlayerDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "RTSPPlayer.h"
#include "RTSPPlayerDlg.h"
#include "afxdialogex.h"

#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
}

void CRTSPPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCREEN, m_Screen);
}

BEGIN_MESSAGE_MAP(CRTSPPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PLAY, &CRTSPPlayerDlg::OnBnClickedPlay)
	ON_BN_CLICKED(IDOK, &CRTSPPlayerDlg::OnBnClickedOk)
END_MESSAGE_MAP()


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

	m_strURL = L"rtsp://192.168.10.10/channel1";

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
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

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
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CRTSPPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRTSPPlayerDlg::OnBnClickedPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	pThreadSDLPlay = AfxBeginThread((AFX_THREADPROC)SDLPlayThread, (LPVOID)this);
}

UINT CRTSPPlayerDlg::SDLPlayThread(LPVOID args)
{
	if (NULL != args)
	{
		CRTSPPlayerDlg *pThis = (CRTSPPlayerDlg*)args;

		return pThis->SDLPlayProc(args);
	}
}

int CRTSPPlayerDlg::SDLPlayProc(LPVOID args)
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

	AVFormatContext* pFormatCtx = NULL;
	pFormatCtx = avformat_alloc_context();

	// TCP, timeout 옵션지정
	AVDictionary* opts = 0;
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);
	av_dict_set(&opts, "stimeout", "1000*1000", 0);

	// RTSP Stream Open
	if (avformat_open_input(&pFormatCtx, strFilePath, NULL, &opts) != 0)
	{
		return -1;
	}

	pFormatCtx->interrupt_callback.opaque = pFormatCtx;
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		return -1;		// Couldn't find stream information
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
		return -1;
	}

	// 비디오 코덱을 얻는다.
	AVCodecContext * pCodexCtx = pFormatCtx->streams[videoStream]->codec;

	// 코덱에 대하여 디코더가 있는지 확인
	AVCodec* pCodec = avcodec_find_decoder(pCodexCtx->codec_id);
	if (pCodec == NULL)
	{
		//fprintf(sterr, "")
		printf("UnSupport Codec");
		return -1;
	}

	// 코덱을 열고
	AVDictionary* optionsDict = NULL;
	if (avcodec_open2(pCodexCtx, pCodec, &optionsDict) < 0)
	{
		return -1;
	}

	// Source color format
	AVPixelFormat src_fix_fmt = pCodexCtx->pix_fmt;

	// 
	AVPixelFormat dst_fix_fmt = AV_PIX_FMT_YUV420P;

	AVFrame* pFrame = av_frame_alloc();
	AVFrame* pFrameVUV = av_frame_alloc();

	if (pFrameVUV == NULL)
	{
		return -1;
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
		exit(1);
	}

	SDL_Window* sdlWindow = SDL_CreateWindowFrom(dlg->m_Screen.GetSafeHwnd());

	if (!sdlWindow)
	{
		exit(1);
	}

	SDL_Renderer* sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

	SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STATIC, pCodexCtx->width, pCodexCtx->height);

	if (!sdlTexture)
	{
		return -1;
	}

	SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);

	AVPacket packet;
	SDL_Event event;

	// Stream read
	while (av_read_frame(pFormatCtx, &packet)>=0)
	{
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

	SDL_DestroyRenderer(sdlRenderer);
	SDL_DestroyTexture(sdlTexture);
	SDL_Quit();

	sws_freeContext(sws_clx);
	dlg->m_Screen.ShowWindow(SW_SHOWNORMAL);

	av_free(pFrame);
	av_free(pFrameVUV);

	avcodec_close(pCodexCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}

void CRTSPPlayerDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}
