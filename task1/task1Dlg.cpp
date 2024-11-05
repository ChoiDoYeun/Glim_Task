// task1Dlg.cpp: 구현 파일
//

////* Nessary include *////

#include "pch.h"
#include "framework.h"
#include "task1.h"
#include "task1Dlg.h"
#include "afxdialogex.h"
#include <cstdlib>  // 랜덤 함수 사용
#include <ctime>    // 랜덤 시드 초기화
#include <opencv2/opencv.hpp>  // OpenCV 헤더 추가
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <Windows.h>  // Windows API 사용
#include <string>     // std::string 사용을 위해 추가
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////* 변수 설정 *////

// 검정 이미지 파일 (256*256) 경로 
// ※ 오류시 : 이미지 경로 확인 필 ※
 const CString BASE_IMAGE_PATH = _T("./base_img.bmp");

// 상수 정의 (사용자 수정 가능 옵션)
const int IMAGE_WIDTH = 256;	// gui상 표시될 이미지의 너비 (256을 권장)
const int IMAGE_HEIGHT = 256;	// gui상 표시될 이미지의 높이 (256을 권장)

const int MIN_RADIUS = 20;	// Draw버튼으로 생성될 원의 최소 반지름
const int MAX_RADIUS = 100;	// Draw버튼으로 생성될 원의 최대 반지름

int randomRadius = 0;	// 랜덤한 반지름을 저장하는 변수 // Draw에서 생성되어 저장후 Action에서 쓰임

const int STEP_COUNT = 20;	// 높일수록 부드러운 원이동
const int SLEEP_DURATION_MS = 100;	// 낮출수록 빠른 원이동

const int SIZE_OF_X = 10;	// Open버튼으로 생성될 X의 크기 // ex) 10일경우 10*10의 X가 생성됨



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

// Ctask1Dlg 대화 상자

Ctask1Dlg::Ctask1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TASK1_DIALOG, pParent)
	, m_x1(0)
	, m_y1(0)
	, m_x2(0)
	, m_y2(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ctask1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE, m_ImageCtrl); // Picture Control과 연결
	DDX_Text(pDX, IDC_EDIT_X1, m_x1);
	DDV_MinMaxInt(pDX, m_x1, 0, IMAGE_WIDTH);
	DDX_Text(pDX, IDC_EDIT_Y1, m_y1);
	DDV_MinMaxInt(pDX, m_y1, 0, IMAGE_HEIGHT);
	DDX_Text(pDX, IDC_EDIT_X2, m_x2);
	DDV_MinMaxInt(pDX, m_x2, 0, IMAGE_WIDTH);
	DDX_Text(pDX, IDC_EDIT_Y2, m_y2);
	DDV_MinMaxInt(pDX, m_y2, 0, IMAGE_HEIGHT);
}

BEGIN_MESSAGE_MAP(Ctask1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_DRAW, &Ctask1Dlg::OnBnClickedBtnDraw)
	ON_BN_CLICKED(IDC_BTN_ACTION, &Ctask1Dlg::OnBnClickedBtnAction)
	ON_BN_CLICKED(IDC_BTN_OPEN, &Ctask1Dlg::OnBnClickedBtnOpen)
END_MESSAGE_MAP()



// Ctask1Dlg 메시지 처리기

BOOL Ctask1Dlg::OnInitDialog()
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

	// 랜덤 시드 초기화
	srand(static_cast<unsigned int>(time(NULL)));

	// currentX, currentY 초기화 (원의 시작 위치)
	currentX = m_x1;
	currentY = m_y1;

	return TRUE;
}

void Ctask1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Ctask1Dlg::OnPaint()
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
		// CPaintDC 대신 Picture Control에 DC를 직접 얻어 이미지를 그림
		CRect rect;
		m_ImageCtrl.GetClientRect(&rect);

		// Picture Control의 DC를 얻음
		CPaintDC dc(&m_ImageCtrl);

		// 이미지 로드
		CImage image;
		HRESULT hResult = image.Load(BASE_IMAGE_PATH);  // CString 타입 사용

		if (SUCCEEDED(hResult))
		{
			// 이미지 크기를 조정하여 그리기
			CRect targetRect(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
			image.StretchBlt(dc.m_hDC, targetRect, SRCCOPY);
		}

		// 부모 클래스의 OnPaint 호출
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.

HCURSOR Ctask1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// OpenCV 이미지를 CImage로 변환하는 함수
void MatToCImage(cv::Mat& mat, CImage& img)
{
	if (mat.empty())
		return;

	int bpp = mat.channels() * 8;  // 비트 수
	img.Create(mat.cols, mat.rows, bpp);

	for (int y = 0; y < mat.rows; ++y)
	{
		for (int x = 0; x < mat.cols; ++x)
		{
			COLORREF color = RGB(mat.at<cv::Vec3b>(y, x)[2], mat.at<cv::Vec3b>(y, x)[1], mat.at<cv::Vec3b>(y, x)[0]);  // BGR -> RGB로 변환
			img.SetPixel(x, y, color);
		}
	}
}

// Draw 버튼 이벤트 처리기
	// 기능 : 사용자가 입력한 x1, y1에 랜덤한 반지름 크기만큼의 원을 그림

void Ctask1Dlg::OnBnClickedBtnDraw()
{
	// x1, y1 좌표를 업데이트
	UpdateData(TRUE); // 사용자가 입력한 x1, y1 값을 가져옴

	// OpenCV를 사용하여 이미지에 원을 그림
	CT2CA asciiPath(BASE_IMAGE_PATH);
	cv::Mat img = cv::imread(std::string(asciiPath));  // CString을 std::string으로 변환하여 전달

	if (!img.empty())
	{
		cv::resize(img, img, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT));  // 이미지를 지정된 크기로 리사이즈

		// 랜덤한 반지름 생성
		randomRadius = rand() % (MAX_RADIUS - MIN_RADIUS + 1) + MIN_RADIUS;  // 랜덤 반지름을 저장

		// 흰색 원을 (m_x1, m_y1) 중심으로 그리기
		cv::circle(img, cv::Point(m_x1, m_y1), randomRadius, cv::Scalar(255, 255, 255), -1);  // 채워진 흰색원

		// 업데이트된 이미지를 Picture Control에 표시
		CClientDC dc(&m_ImageCtrl);
		CImage cimage;
		MatToCImage(img, cimage);
		cimage.Draw(dc.m_hDC, 0, 0);
	}

	// 다이얼로그에 변경된 값을 반영
	UpdateData(FALSE);
}

// Action 버튼 이벤트 처리기
	// 기능 : x1,y1으로 생성된 원을 x1,y1 부터 x2, y2까지 이동, 이동과정 이미지 저장

void Ctask1Dlg::OnBnClickedBtnAction()
{
	// x1, y1, x2, y2 좌표를 업데이트
	UpdateData(TRUE);

	// 실행 파일의 경로를 구하고, image 폴더를 생성
	TCHAR szFilePath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);  // 실행 파일의 경로를 얻음
	CString strFilePath(szFilePath);
	int pos = strFilePath.ReverseFind('\\');
	strFilePath = strFilePath.Left(pos + 1);  // 실행 파일이 있는 폴더 경로

	CString imageFolderPath = strFilePath + _T("image\\");  // image 폴더 경로 설정
	CreateDirectory(imageFolderPath, NULL);  // 폴더가 없으면 생성

	// 이미지 로드
	CT2CA asciiPath(BASE_IMAGE_PATH);
	cv::Mat img = cv::imread(std::string(asciiPath));  // CString을 std::string으로 변환하여 전달

	if (img.empty())
	{
		AfxMessageBox(_T("이미지를 불러올 수 없습니다."));
		return;
	}

	// 이미지 리사이즈
	cv::resize(img, img, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT));

	// 원 이동 간격 설정
	double deltaX = static_cast<double>(m_x2 - currentX) / STEP_COUNT; // x축 이동 간격
	double deltaY = static_cast<double>(m_y2 - currentY) / STEP_COUNT; // y축 이동 간격

	for (int i = 0; i <= STEP_COUNT; ++i)
	{
		// 원 그리기 (randomRadius 사용)
		cv::Mat imgCopy = img.clone();  // 원본 이미지를 복사
		cv::circle(imgCopy, cv::Point(static_cast<int>(currentX), static_cast<int>(currentY)), randomRadius, cv::Scalar(255, 255, 255), -1); // 저장된 반지름 사용

		// 이미지 업데이트 및 Picture Control에 그리기
		CClientDC dc(&m_ImageCtrl);
		CImage cimage;
		MatToCImage(imgCopy, cimage);
		cimage.Draw(dc.m_hDC, 0, 0);

		// 현재 위치 업데이트
		currentX += deltaX;
		currentY += deltaY;

		// 이미지 파일 저장
		CString fileName;
		fileName.Format(_T("image_step_%d.bmp"), i);  // 저장될 파일 이름 설정
		CString fullFilePath = imageFolderPath + fileName;

		// CString을 std::string으로 변환 (CStringA 사용)
		CStringA ansiFullFilePath(fullFilePath);
		std::string savePath(ansiFullFilePath);

		// OpenCV를 사용하여 이미지 저장
		cv::imwrite(savePath, imgCopy);  // 이미지를 파일로 저장

		Sleep(SLEEP_DURATION_MS);  // 지정된 시간만큼 대기
	}

	// 다이얼로그에 변경된 값을 반영 (GUI의 m_x1, m_y1은 그대로 유지)
	UpdateData(FALSE);
}

// Open 버튼 이벤트 처리기
	// 기능 : Open으로 열린 원 이미지에서 원의 중심을 찾아 X자를 그리고, 중심값을 gui에 표시
		// 경험적 오차 : 최대 ± 5 픽셀 (원의 반지름에 따라, 평균 ± 3 픽셀)

void Ctask1Dlg::OnBnClickedBtnOpen()
{
	// 파일 탐색기 열기: BMP, JPG, PNG 파일 형식을 지원
	CFileDialog fileDlg(TRUE, _T("Image Files"), NULL, OFN_FILEMUSTEXIST,
		_T("Image Files (*.bmp; *.jpg; *.png)|*.bmp;*.jpg;*.png|All Files (*.*)|*.*||"));

	if (fileDlg.DoModal() == IDOK)
	{
		// 선택한 파일 경로 가져오기
		CString filePath = fileDlg.GetPathName();

		// CString을 std::string으로 변환
		CStringA ansiPath(filePath);
		std::string imagePath(ansiPath);

		// OpenCV를 사용하여 이미지 로드
		cv::Mat img = cv::imread(imagePath);

		if (img.empty())
		{
			AfxMessageBox(_T("이미지를 불러올 수 없습니다."));
			return;
		}

		// 이미지를 그레이스케일로 변환
		cv::Mat gray;
		cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

		// 가우시안 블러 적용
		cv::GaussianBlur(gray, gray, cv::Size(5, 5), 1, 1);

		// HoughCircles 함수를 사용하여 원 검출
		std::vector<cv::Vec3f> circles;
		cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1,
			gray.rows / 2,  // 최소 거리 설정
			254, 1,        // 캐니 에지 검출기 임계값 설정
			MIN_RADIUS, MAX_RADIUS            // 최소 및 최대 반지름
		);

		if (circles.size() > 0)
		{
			// 첫 번째 검출된 원의 중심 좌표와 반지름 가져오기
			cv::Vec3f c = circles[0];
			cv::Point center(cvRound(c[0]), cvRound(c[1]));
			int radius = cvRound(c[2]);

			// X 자 표시 그리기
			int x = center.x;
			int y = center.y;

			cv::line(img, cv::Point(x - SIZE_OF_X, y - SIZE_OF_X),
				cv::Point(x + SIZE_OF_X, y + SIZE_OF_X), cv::Scalar(0, 0, 255), 2);
			cv::line(img, cv::Point(x - SIZE_OF_X, y + SIZE_OF_X),
				cv::Point(x + SIZE_OF_X, y - SIZE_OF_X), cv::Scalar(0, 0, 255), 2);

			// 좌표값을 이미지에 텍스트로 표시
			std::string coordText = "X: " + std::to_string(x) + " Y: " + std::to_string(y);
			cv::putText(img, coordText, cv::Point(10, img.rows - 10),
				cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);

			// 검출된 원 그리기
			cv::circle(img, center, radius, cv::Scalar(0, 255, 0), 2);
		}
		else
		{
			AfxMessageBox(_T("이미지에서 원을 찾을 수 없습니다."));
		}

		// 이미지를 Picture Control에 출력
		CClientDC dc(&m_ImageCtrl);
		CImage cimage;
		MatToCImage(img, cimage);  // OpenCV Mat을 MFC CImage로 변환
		cimage.Draw(dc.m_hDC, 0, 0);  // Picture Control에 그리기
	}
}
