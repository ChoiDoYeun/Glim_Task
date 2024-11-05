// task1Dlg.h: 헤더 파일
//

#pragma once

// Ctask1Dlg 대화 상자
class Ctask1Dlg : public CDialogEx
{
	// 생성입니다.
public:
	Ctask1Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	CStatic m_ImageCtrl;

	// 멤버 변수 추가 (현재 원의 위치를 저장할 변수)
	int currentX;
	int currentY;

	// 랜덤 반지름을 저장할 변수 추가
	int randomRadius;


	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TASK1_DIALOG };
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
	int m_x1;
	int m_y1;
	int m_x2;
	int m_y2;
	afx_msg void OnBnClickedBtnDraw();
	afx_msg void OnBnClickedBtnAction();
	afx_msg void OnBnClickedBtnOpen();
};
