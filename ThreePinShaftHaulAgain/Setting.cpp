// Setting.cpp: 实现文件
//

#include "stdafx.h"
#include "ThreePinShaftHaulAgain.h"
#include "Setting.h"
#include "afxdialogex.h"
#include "_global.h"

// Setting 对话框

IMPLEMENT_DYNAMIC(Setting, CDialogEx)

Setting::Setting(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SET_DIALOG, pParent)
{

}

Setting::~Setting()
{
}

void Setting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Setting, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_OUT_CYL, &Setting::OnBnClickedButtonOutCyl)
	ON_BN_CLICKED(IDC_BUTTON_OUT_MOTOR, &Setting::OnBnClickedButtonOutMotor)
END_MESSAGE_MAP()


// Setting 消息处理程序

void Setting::Init()
{
	SetDlgItemText(IDC_BUTTON_OUT_MOTOR, L"启动电机");
	_global::GetIns()->ConveyorSTOP();





	if (_global::GetIns()->ReadOutPutBit(_global::GetIns()->prj.OUT_CYL))
	{
		SetDlgItemText(IDC_BUTTON_OUT_CYL, L"气缸伸出");
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_OUT_CYL, L"气缸收缩");
	}





	SetTimer(1, 50, NULL);
}

void Setting::UpdateUI()
{

}

void Setting::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (1 == nIDEvent)
	{
		Sleep(1);
	}
	CDialogEx::OnTimer(nIDEvent);
}


BOOL Setting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	Init();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void Setting::OnBnClickedButtonOutCyl()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!_global::GetIns()->ReadOutPutBit(_global::GetIns()->prj.OUT_CYL))
	{
		SetDlgItemText(IDC_BUTTON_OUT_CYL, L"气缸伸出");
		_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL, _global::OFF);
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_OUT_CYL, L"气缸收缩");
		_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL, _global::ON);
	}
}


void Setting::OnBnClickedButtonOutMotor()
{
	// TODO: 在此添加控件通知处理程序代码
	if (_global::STOP == _global::GetIns()->GetConveyorState())
	{
		SetDlgItemText(IDC_BUTTON_OUT_MOTOR, L"停止电机");
		_global::GetIns()->ConveyorCW();
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_OUT_MOTOR, L"启动电机");
		_global::GetIns()->ConveyorSTOP();
	}
}
