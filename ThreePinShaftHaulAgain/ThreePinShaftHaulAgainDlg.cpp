
// ThreePinShaftHaulAgainDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ThreePinShaftHaulAgain.h"
#include "ThreePinShaftHaulAgainDlg.h"
#include "afxdialogex.h"
#include "_global.h"
#include "Setting.h"
#include "Windows.h"
#include <vector>
#include <list>
#include "csv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//等待推料出去气缸缩回来的变量
bool WaitCylBack = false;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CThreePinShaftHaulAgainDlg 对话框



CThreePinShaftHaulAgainDlg::CThreePinShaftHaulAgainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_THREEPINSHAFTHAULAGAIN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CThreePinShaftHaulAgainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CThreePinShaftHaulAgainDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_GET_GEAR_MODEL, &CThreePinShaftHaulAgainDlg::OnBnClickedButtonGetGearModel)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_GEAR_MODEL2, &CThreePinShaftHaulAgainDlg::OnBnClickedButtonCheckGearModel2)
	ON_BN_CLICKED(IDC_BUTTON_GET_IMAGE, &CThreePinShaftHaulAgainDlg::OnBnClickedButtonGetImage)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_PARA, &CThreePinShaftHaulAgainDlg::OnBnClickedButtonSavePara)
	ON_BN_CLICKED(IDC_BUTTON_SET, &CThreePinShaftHaulAgainDlg::OnBnClickedButtonSet)
	ON_BN_CLICKED(IDC_BUTTON_READ_IMAGE, &CThreePinShaftHaulAgainDlg::OnBnClickedButtonReadImage)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_IMAGE, &CThreePinShaftHaulAgainDlg::OnBnClickedButtonSaveImage)
END_MESSAGE_MAP()


void CThreePinShaftHaulAgainDlg::Init() {


	CStatic * pWnd = (CStatic*)GetDlgItem(IDC_STATIC_SHOW);
	_global::GetIns()->open_window(pWnd,&_global::GetIns()->disp_hand);
	AfxBeginThread(camera, this, THREAD_PRIORITY_ABOVE_NORMAL, 0, 0, NULL);

	if (0 == _global::GetIns()->Init())
	{
		UpdateHistory(L"未能找到控制卡!\r\n");
	}
	else
	{
		AfxBeginThread(io_scanner, this, THREAD_PRIORITY_ABOVE_NORMAL, 0, 0, NULL);
	}

	CString str;
	str.Format(L"%3.0f", _global::GetIns()->prj.Threshold);
	
	SetDlgItemText(IDC_EDIT_THRESHOLD, str);

	str.Format(L"%3.3f", _global::GetIns()->prj.Score_Threshold);

	SetDlgItemText(IDC_EDIT_SCORE, str);

	str.Format(L"%3.0f", _global::GetIns()->prj.Snap_Delay);

	SetDlgItemText(IDC_EDIT_SNAP_DELAY, str);

	str.Format(L"%3.3f", _global::GetIns()->prj.Fb_Score_Threshold);

	SetDlgItemText(IDC_EDIT_SCORE_FB, str);
	
	str.Format(L"%4.0f", _global::GetIns()->prj.Cyl_Out_Delay);

	SetDlgItemText(IDC_EDIT_CYL_OUT_DELAY, str);

	str.Format(L"%4.0f", _global::GetIns()->prj.Cyl_Back_Delay);

	SetDlgItemText(IDC_EDIT_CYL_BACK_DELAY, str);

#ifdef VERSION1
	str.Format(L"%4.0f", _global::GetIns()->prj.Cyl_Push_To_Check_Delay);

	SetDlgItemText(IDC_EDIT_SNAP_DELAY_FOR_PUSH_CYL, str);
#else
	
#endif


}
// CThreePinShaftHaulAgainDlg 消息处理程序

BOOL CThreePinShaftHaulAgainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

#if 1
	HANDLE hObject = ::CreateMutexW(NULL, FALSE, L"TPS00.9.12");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hObject);
		AfxMessageBox(L"程序已经运行，请勿多开。如果前一个看不见，请打开任务管理器，将进程掐掉");
		exit(0);
	}
#endif

	Init();
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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




	SetWindowText(L"浙江爱易特智能技术");



	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CThreePinShaftHaulAgainDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CThreePinShaftHaulAgainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CThreePinShaftHaulAgainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CThreePinShaftHaulAgainDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	_global::GetIns()->close_camera();
	_global::GetIns()->board_close();
	system("taskkill  /f /im ThreePinShaftHaulAgain.exe");
	CDialogEx::OnClose();
}


void CThreePinShaftHaulAgainDlg::OnBnClickedButtonGetGearModel()
{
	// TODO: 在此添加控件通知处理程序代码
	_global::GetIns()->CreateGearModel(_global::GetIns()->ho_Image,_global::GetIns()->disp_hand, _global::GetIns()->prj.Threshold);
}


void CThreePinShaftHaulAgainDlg::OnBnClickedButtonCheckGearModel2()
{
	// TODO: 在此添加控件通知处理程序代码

//	static csv Csv;
//	Csv.WriteLine_Ex("safdsafdsaf");
	using namespace HalconCpp;
	try
	{
		_global::GetIns()->GearCheck(_global::GetIns()->ho_Image, _global::GetIns()->prj.Score_Threshold, _global::GetIns()->disp_hand, _global::GetIns()->prj.Threshold);
	}
	catch (HalconCpp::HException ErrorInfo)
	{

	}
}


void CThreePinShaftHaulAgainDlg::OnBnClickedButtonGetImage()
{
	// TODO: 在此添加控件通知处理程序代码
	int res = _global::GetIns()->grab(_global::GetIns()->ho_Image);
	_global::GetIns()->disp_obj(_global::GetIns()->ho_Image, _global::GetIns()->disp_hand);
	CString info;
	info.Format(L"抓取结果%d\r\n",res);
	UpdateHistory(info);
}


void CThreePinShaftHaulAgainDlg::OnBnClickedButtonSavePara()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	GetDlgItemText(IDC_EDIT_THRESHOLD, str);

	_global::GetIns()->prj.Threshold = static_cast<double>(_ttof(str));

	GetDlgItemText(IDC_EDIT_SCORE, str);

	_global::GetIns()->prj.Score_Threshold = static_cast<double>(_ttof(str));

	GetDlgItemText(IDC_EDIT_SCORE_FB, str);

	_global::GetIns()->prj.Fb_Score_Threshold = static_cast<double>(_ttof(str));

	
	GetDlgItemText(IDC_EDIT_SNAP_DELAY, str);

	_global::GetIns()->prj.Snap_Delay = static_cast<double>(_ttof(str));

	GetDlgItemText(IDC_EDIT_CYL_OUT_DELAY, str);

	_global::GetIns()->prj.Cyl_Out_Delay = static_cast<double>(_ttof(str));

	GetDlgItemText(IDC_EDIT_CYL_BACK_DELAY, str);

	_global::GetIns()->prj.Cyl_Back_Delay = static_cast<double>(_ttof(str));


	GetDlgItemText(IDC_EDIT_SNAP_DELAY_FOR_PUSH_CYL, str);

	_global::GetIns()->prj.Cyl_Push_To_Check_Delay = static_cast<double>(_ttof(str));
	
	_global::GetIns()->prj.SaveParaFile(-1);
}

//单孔面积 == 堵孔
//孔径尺寸 == 直径  0.3 = 0.45
//总面积 0.35 * 19 -- 0.45 * 19

void CThreePinShaftHaulAgainDlg::OnBnClickedButtonSaveImage()
{
	try {
		// TODO: 在此添加控件通知处理程序代码
		SYSTEMTIME st;

		CString strDate, strTime;

		GetLocalTime(&st);

		strDate.Format(L"%4d-%2d-%2d", st.wYear, st.wMonth, st.wDay);

		strTime.Format(L"%2d-%2d-%2d", st.wHour, st.wMinute, st.wSecond);

		CString file_name = strDate + L"-" + strTime;

		HalconCpp::WriteImage(_global::GetIns()->ho_Image, "bmp", NULL, (char*)(LPCSTR)(CStringA)file_name);
	}
	catch (...)
	{
		return;
	}
}



void CThreePinShaftHaulAgainDlg::OnBnClickedButtonSet()
{
	// TODO: 在此添加控件通知处理程序代码
	Setting setDlg;
	setDlg.DoModal();
}

UINT CThreePinShaftHaulAgainDlg::camera(LPVOID lp)
{
	//CAmera
	CThreePinShaftHaulAgainDlg *pDlg = (CThreePinShaftHaulAgainDlg*)lp;
	if (0!=_global::GetIns()->HKInitCamera()) {
		pDlg->UpdateHistory(L"相机打开失败\r\n");
	}
	else
	{
		pDlg->UpdateHistory(L"相机打开成功\r\n");
	}
	return 0;
}


void CThreePinShaftHaulAgainDlg::OnBnClickedButtonReadImage()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDlg(TRUE, (LPCTSTR)"BMP", L"", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"All Files (*.*)|*.*|位图文件 (*.bmp)|*.bmp||");	//打开读取文件对话框
	if (fileDlg.DoModal() != IDOK) return;
	try {
		const char* imgPath = (char*)(LPCSTR)CStringA(fileDlg.GetPathName());
		using namespace HalconCpp;
		HalconCpp::ReadImage(&_global::GetIns()->ho_Image, imgPath);
		DispObj(_global::GetIns()->ho_Image, _global::GetIns()->disp_hand);
		_global::GetIns()->disp_obj(_global::GetIns()->ho_Image, _global::GetIns()->disp_hand);
	}
	catch (...)
	{
		UpdateHistory(L"读取图片失败\r\n");
	}
}


void CThreePinShaftHaulAgainDlg::UpdateHistory(CString str)
{
	static int record_counter = 0;
	CString msg;
	msg.Format(L"%d: ", record_counter++);
	((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->SetSel(GetDlgItem(IDC_EDIT_HISTORY)->GetWindowTextLength(), GetDlgItem(IDC_EDIT_HISTORY)->GetWindowTextLength());//EDIT1是edit控件，首先要在属性里multiline设置为true,并把Horizen Scroll 和veritcal Scroll设置为true
	if (!(record_counter % 100))
		((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->SetWindowText(msg + str);
	else
		((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->ReplaceSel(msg + str);
}


UINT CThreePinShaftHaulAgainDlg::Push_BackCyl(LPVOID lp)
{
	CThreePinShaftHaulAgainDlg *pDlg = (CThreePinShaftHaulAgainDlg*)lp;
	Sleep(50);
	for (int i = 0; i < 6; i++) _global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_OUTSIDE, _global::ON);
	Sleep(600);
	for (int i = 0; i < 6; i++) _global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_OUTSIDE, _global::OFF);

	WaitCylBack = false;
	return 0;
}

UINT CThreePinShaftHaulAgainDlg::io_scanner(LPVOID lp)
{
	CThreePinShaftHaulAgainDlg *pDlg = (CThreePinShaftHaulAgainDlg*)lp;

	csv CsvRec;

	DWORD in_start = _global::GetIns()->ReadInPutBit(_global::GetIns()->prj.IN_START);
	DWORD old_in_start = in_start;

	DWORD in_pause = _global::GetIns()->ReadInPutBit(_global::GetIns()->prj.IN_PAUSE);
	DWORD old_in_pause = in_pause;

	DWORD in_stop = _global::GetIns()->ReadInPutBit(_global::GetIns()->prj.IN_STOP);
	DWORD old_in_stop = in_stop;

	DWORD in_sensor_in = _global::GetIns()->ReadInPutBit(_global::GetIns()->prj.IN_SENSOR_START);
	DWORD old_in_sensor_in = in_sensor_in;

	DWORD in_sensor_end = _global::GetIns()->ReadInPutBit(_global::GetIns()->prj.IN_SENSOR_END);
	DWORD old_in_sensor_end = in_sensor_end;
	
	enum {
		//没检查的，OK的，NG的，预备检查动作正在执行中
		UNKNOWN, OK, NG,CHECKING,IMAGECHECKING,IMAGECHECKOVER
	};

	struct Info {
	public:
		Info(int _Type, clock_t _Clock) {
			Type = _Type;
			Clock = _Clock;
			isCheckOver = false;
		}
		int Type = UNKNOWN;
		//该齿检查完毕？
		bool isCheckOver = false;
		//上升沿感应器感应到的延时的时间记录
		clock_t Clock = 0;
		//气缸推出去检测的记录
		bool isRecordPush_Cyl_Delay = false;
		clock_t Clock_For_Push_Cyl_Delay = 0;

	};
	//参数一包含了，UNKNOWN//没有检测， OK//合格 //NG不合格三种状态
	std::list<Info> info_list;

	clock_t tick_cyl = clock(),last_clock = tick_cyl;

	bool isRun = false;
	//正在检测中，上升沿无法触发
	bool isChecking = false;

	

	for (;;) {

		clock_t clk = clock();

		in_start = _global::GetIns()->ReadInPutBit(_global::GetIns()->prj.IN_START);
	

		in_pause = _global::GetIns()->ReadInPutBit(_global::GetIns()->prj.IN_PAUSE);
		

		in_stop = _global::GetIns()->ReadInPutBit(_global::GetIns()->prj.IN_STOP);
	

		in_sensor_in = _global::GetIns()->ReadInPutBit(_global::GetIns()->prj.IN_SENSOR_START);

		in_sensor_end = _global::GetIns()->ReadInPutBit(_global::GetIns()->prj.IN_SENSOR_END);
		//状态机,启动条件为，没有按下暂停键，没有按下停止键，并且启动键为上升沿
		//三个变量，八种状态，实际上只需要四种
		if ((in_start && (in_start != old_in_start) &&
			!in_pause &&
			in_stop
			)
			)
		{
			

			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_STOP, _global::OFF);
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_PAUSE, _global::OFF);
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_START, _global::ON);
			_global::GetIns()->ConveyorCW();
#ifdef VERSION1
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_CHECK, _global::OFF);
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_OUTSIDE, _global::OFF);

			if (!isRun && !in_sensor_in)
				info_list.push_back(Info(UNKNOWN, clk));
			

#endif
			isRun = true;
			tick_cyl = clk;

		}
		else if (!in_stop)
		{
			isRun = false;
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_STOP, _global::ON);
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_PAUSE, _global::OFF);
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_START, _global::OFF);
			_global::GetIns()->ConveyorSTOP();
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL, _global::OFF);
#ifdef VERSION1
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_CHECK, _global::OFF);
			Sleep(500);
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_OUTSIDE, _global::OFF);
#endif
			tick_cyl = clk;
			isChecking = false;
			info_list.clear();
		}
		else if (in_pause)
		{
			isRun = false;
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_STOP, _global::OFF);
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_PAUSE, _global::ON);
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_START, _global::OFF);
			_global::GetIns()->ConveyorSTOP();
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL, _global::OFF);
#ifdef VERSION1
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_CHECK, _global::OFF);
			Sleep(500);
			_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_OUTSIDE, _global::OFF);
#endif
			tick_cyl = clk;
			isChecking = false;
			info_list.clear();
		}
		

		if (isRun)
		{

			//感应器感应到物料的上升沿后，记链表记录延时时间
#ifdef VERSION1
			//过滤，如果太频繁的上升沿，就是不存在的信号,clk- last_clock 时间上过滤
			if (!in_sensor_in && (in_sensor_in != old_in_sensor_in)
				&&  in_sensor_end
				&& isChecking == false
				&& (clk- last_clock)>500)
			//改为
#else
			if (in_sensor_in && (in_sensor_in != old_in_sensor_in))
#endif
			{
				last_clock = clk;
				info_list.push_back(Info(UNKNOWN,clk));
			}

				if (!_global::GetIns()->ReadOutPutBit(_global::GetIns()->prj.OUT_CYL))
				{

					if ((clk - tick_cyl) > _global::GetIns()->prj.Cyl_Out_Delay)
					{
						_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL, _global::OFF);
						tick_cyl = clk;
					}
				}
				else
				{
					if ((clk - tick_cyl) > _global::GetIns()->prj.Cyl_Back_Delay)
					{
						_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL, _global::ON);
						tick_cyl = clk;
					}
				}



//查看状态来决定怎么执行动作
			for (std::list<Info>::iterator it = info_list.begin(); it != info_list.end();)
			{
				bool isDel = false;
				switch (it->Type)
				{
				case UNKNOWN: 
					if (((clk - it->Clock) > _global::GetIns()->prj.Snap_Delay) &&
						isChecking == false)
					{
#ifdef VERSION1
						CString str;
						str.Format(L"延时了 %d ms去推出气缸\r\n",static_cast<int>(_global::GetIns()->prj.Snap_Delay));
						pDlg->UpdateHistory(str);
						it->Type = CHECKING;
						it->Clock = clk;
						break;
#else
						HTuple hv_exception;
						try {

							//如果样品未知的状态
							HalconCpp::HObject hobj;
							_global::GetIns()->grab(hobj);
							if (false == _global::GetIns()->GearCheck(hobj, _global::GetIns()->prj.Score_Threshold, _global::GetIns()->disp_hand, _global::GetIns()->prj.Threshold)) {
								isRun = false;
								CsvRec.WriteLine_Ex("漏拉或者复拉");
								//报警
								_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_STOP, _global::ON);
								_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_PAUSE, _global::OFF);
								_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_START, _global::OFF);
								_global::GetIns()->ConveyorSTOP();
								_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL, _global::OFF);
								tick_cyl = clk;
							}
						}
						catch (HalconCpp::HException &HDevExpDefaultException)
						{
							HDevExpDefaultException.ToHTuple(&hv_exception);
							isRun = false;
							CsvRec.WriteLine_Ex("没有模板或没有图像");
							//报警
							_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_STOP, _global::OFF);
							_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_PAUSE, _global::ON);
							_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_START, _global::OFF);
							_global::GetIns()->ConveyorSTOP();
							_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL, _global::OFF);
							tick_cyl = clk;
							pDlg->UpdateHistory(L"可能没有设置过模板，或者没有图像\r\n");
						}
						//删除掉这个节点，反正已经没用了
						isDel = true;
#endif					
						break;
					}
					break;
				case CHECKING:
				{
					isChecking = true;
					//已经在检查状态中了，意味着气缸要推出物料检查，但是，实现要检查气缸状态，免得干涉,一共四种状态，气缸1伸出中，气缸2伸出中
					if (_global::GetIns()->ReadOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_CHECK) && _global::GetIns()->ReadOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_OUTSIDE))
					{
						_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_CHECK, _global::ON);
						_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL, _global::ON);
						_global::GetIns()->ConveyorSTOP();
						//保持气缸不动
						tick_cyl = clk;
						//状态立即转化
						it->Clock = clk;
						it->Type = IMAGECHECKING;
					}

					if ((clk - it->Clock) > 5000)
					{
						//超时报警
						isRun = false;
						_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_STOP, _global::OFF);
						_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_PAUSE, _global::ON);
						_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_START, _global::OFF);
						_global::GetIns()->ConveyorSTOP();
						_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL, _global::OFF);
						_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_CHECK, _global::OFF);
						Sleep(500);
						_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_OUTSIDE, _global::OFF);
						tick_cyl = clk;
						pDlg->UpdateHistory(L"等待气缸缩回超时\r\n");
					}
				}
				break;
				case IMAGECHECKING:
				{
					isChecking = true;
					//保持气缸不动
					tick_cyl = clk;
					if ((clk - it->Clock) > _global::GetIns()->prj.Cyl_Push_To_Check_Delay)
					{
						for (int i = 0; i < 6; i++) _global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_CHECK, _global::OFF);
						HTuple hv_exception;
						try {
							int res = _global::GetIns()->grab(_global::GetIns()->ho_Image);
							_global::GetIns()->disp_obj(_global::GetIns()->ho_Image, _global::GetIns()->disp_hand);
							CString info;
							info.Format(L"检测的抓取结果%d\r\n", res);
							pDlg->UpdateHistory(info);
							if (false == _global::GetIns()->GearCheck(_global::GetIns()->ho_Image, _global::GetIns()->prj.Score_Threshold, _global::GetIns()->disp_hand, _global::GetIns()->prj.Threshold)) {
								isRun = false;
								CsvRec.WriteLine_Ex("漏拉或者复拉");
								//报警
								_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_STOP, _global::ON);
								_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_PAUSE, _global::OFF);
								_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_START, _global::OFF);
								_global::GetIns()->ConveyorSTOP();
								_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL, _global::OFF);
								_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_CHECK, _global::OFF);
								Sleep(500);
								_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_OUTSIDE, _global::OFF);
								tick_cyl = clk;
							}
							
							//状态转化
							it->Clock = clk;
							it->Type = IMAGECHECKOVER;
							WaitCylBack = true;
							AfxBeginThread(Push_BackCyl, pDlg, THREAD_PRIORITY_ABOVE_NORMAL, 0, 0, NULL);
							
						}
						catch (HalconCpp::HException &HDevExpDefaultException)
						{
							HDevExpDefaultException.ToHTuple(&hv_exception);
							isRun = false;
							CsvRec.WriteLine_Ex("没有模板或没有图像");
							//报警
							_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_STOP, _global::OFF);
							_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_PAUSE, _global::ON);
							_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_START, _global::OFF);
							_global::GetIns()->ConveyorSTOP();
							_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL, _global::OFF);
							_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_CHECK, _global::OFF);
							Sleep(500);
							_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_OUTSIDE, _global::OFF);
							tick_cyl = clk;
							pDlg->UpdateHistory(L"可能没有设置过模板，或者没有图像\r\n");
						}
					}
				}
				break;
				case IMAGECHECKOVER:
					//保持气缸不动
					tick_cyl = clk;
					//读取到结果就可以运动
					if (in_sensor_end)
					{
						isChecking = false;
						isDel = true;
						//恢复皮带运动
						_global::GetIns()->ConveyorCW();
					}
					
				/**
					if (!WaitCylBack)
					{

					}	
					*/
					break;
				default:
					break;
				}

				if (isDel)
				{
					info_list.erase(it++);
				}
				else
				{
					++it;
				}

			}



			if (!isRun)
			{
				_global::GetIns()->ConveyorSTOP();
				tick_cyl = clk;
				isChecking = false;
				info_list.clear();
			}
		}




		old_in_start = in_start;


		old_in_pause = in_pause;


		old_in_stop = in_stop;


		 old_in_sensor_in = in_sensor_in;


		Sleep(1);
	}
}

UINT CThreePinShaftHaulAgainDlg::BackCyl(LPVOID lp)
{
	_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_CHECK, _global::OFF);
	Sleep(1000);
	_global::GetIns()->WriteOutPutBit(_global::GetIns()->prj.OUT_CYL_PUSH_OBJ_TO_OUTSIDE, _global::OFF);
}