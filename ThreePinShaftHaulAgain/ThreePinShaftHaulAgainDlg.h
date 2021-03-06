
// ThreePinShaftHaulAgainDlg.h: 头文件
//

#pragma once


// CThreePinShaftHaulAgainDlg 对话框
class CThreePinShaftHaulAgainDlg : public CDialogEx
{
// 构造
public:
	CThreePinShaftHaulAgainDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_THREEPINSHAFTHAULAGAIN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	void Init();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	bool isBacking = false;
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonGetGearModel();
	afx_msg void OnBnClickedButtonCheckGearModel2();
	afx_msg void OnBnClickedButtonGetImage();
	afx_msg void OnBnClickedButtonSavePara();
	afx_msg void OnBnClickedButtonSet();
	static UINT camera(LPVOID lp);
	static UINT Push_BackCyl(LPVOID lp);
	static UINT io_scanner(LPVOID lp);
	static UINT BackCyl(LPVOID lp);
	afx_msg void OnBnClickedButtonReadImage();
	void UpdateHistory(CString str);
	afx_msg void OnBnClickedButtonSaveImage();
};
