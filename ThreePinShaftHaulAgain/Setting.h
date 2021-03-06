#pragma once


// Setting 对话框

class Setting : public CDialogEx
{
	DECLARE_DYNAMIC(Setting)

public:
	Setting(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Setting();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SET_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg
		void Init();
	void UpdateUI();
	void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonOutCyl();
	afx_msg void OnBnClickedButtonOutMotor();
};
