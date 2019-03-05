
// ShowRobotDataDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ShowRobotData.h"
#include "ShowRobotDataDlg.h"
#include "afxdialogex.h"
#include <queue>
#include <fstream>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
#define UM_DRAWROBOTDATA    WM_USER+100    //自定义一个消息
UINT server_thd(LPVOID p);   //申明线程函数
CString IP;  //定义为全局变量
SOCKET listen_sock;
SOCKET sock;
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CShowRobotDataDlg 对话框



CShowRobotDataDlg::CShowRobotDataDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CShowRobotDataDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShowRobotDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ChartCtrl1, m_ChartCtrl1);
	DDX_Control(pDX, IDC_ChartCtrl2, m_ChartCtrl2);
	DDX_Control(pDX, IDC_ChartCtrl3, m_ChartCtrl3);
	DDX_Control(pDX, IDC_ChartCtrl4, m_ChartCtrl4);
	DDX_Control(pDX, IDC_LIST1_SHOWMESSAGE, m_recvMessage);
	DDX_Control(pDX, IDC_EDIT2_TCPPort, m_TCPPort);
	DDX_Control(pDX, IDC_IPADDRESS1, m_TCPIPaddr);
}

BEGIN_MESSAGE_MAP(CShowRobotDataDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_STARTSERVER, &CShowRobotDataDlg::OnBnClickedStartserver)
	ON_LBN_SELCHANGE(IDC_LIST1_SHOWMESSAGE, &CShowRobotDataDlg::OnLbnSelchangeList1Showmessage)
	ON_MESSAGE(UM_DRAWROBOTDATA, &CShowRobotDataDlg::OnDrawRobotData)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CShowRobotDataDlg 消息处理程序

BOOL CShowRobotDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	CChartAxis *pAxis = NULL;
	pAxis = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-120, 120);

	pAxis = m_ChartCtrl2.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl2.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-120, 120);

	pAxis = m_ChartCtrl3.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl3.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-120, 120);

	pAxis = m_ChartCtrl4.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl4.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-360, 360);
	///////创建标题
	TChartString str1;

	str1 = _T("#1的角度，速度，力矩");
	m_ChartCtrl1.GetTitle()->AddString(str1);
	str1 = _T("计数点数");
	CChartAxisLabel *pLabel = m_ChartCtrl1.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("位置deg、速度deg/s、力矩N·m");
	pLabel = m_ChartCtrl1.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	str1 = _T("#2的角度，速度，力矩");
	m_ChartCtrl2.GetTitle()->AddString(str1);
	str1 = _T("计数点数");
	pLabel = m_ChartCtrl2.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("位置deg、速度deg/s、力矩N·m");
	pLabel = m_ChartCtrl2.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	str1 = _T("#3的角度，速度，力矩");
	m_ChartCtrl3.GetTitle()->AddString(str1);
	str1 = _T("计数点数");
	pLabel = m_ChartCtrl3.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("位置deg、速度deg/s、力矩N·m");
	pLabel = m_ChartCtrl3.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	str1 = _T("#4的角度，速度，力矩");
	m_ChartCtrl4.GetTitle()->AddString(str1);
	str1 = _T("计数点数");
	pLabel = m_ChartCtrl4.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("位置deg、速度deg/s、力矩N·m");
	pLabel = m_ChartCtrl4.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	for (int i = 0; i < 3; i++)
	{
		m_pLineSerie1[i] = m_ChartCtrl1.CreateLineSerie();
		m_pLineSerie2[i] = m_ChartCtrl2.CreateLineSerie();
		m_pLineSerie3[i] = m_ChartCtrl3.CreateLineSerie();
		m_pLineSerie4[i] = m_ChartCtrl4.CreateLineSerie();
	}
	/////////////////////////////////////////////////
	SetTimer(1, 150, NULL);  //设置定时器，定时周期为100ms


	fileopenflag = false;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CShowRobotDataDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CShowRobotDataDlg::OnPaint()
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
HCURSOR CShowRobotDataDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CString2Char(CString str, char ch[])//此函数就是字符转换函数的实现代码
{
	int i;
	char *tmpch;
	int wLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);//得到Char的长度
	tmpch = new char[wLen + 1];                                             //分配变量的地址大小
	WideCharToMultiByte(CP_ACP, 0, str, -1, tmpch, wLen, NULL, NULL);       //将CString转换成char*


	for (i = 0; tmpch[i] != '\0'; i++) ch[i] = tmpch[i];
	ch[i] = '\0';
}

std::ofstream oFile;
void CShowRobotDataDlg::OnBnClickedStartserver()
{
	//在函数之前添加保存Excel的文件   
	oFile.open("Robotdata.csv",std::ios::out|std::ios::trunc);  //输出模式，如果文件已经存在就删除这个文件
	oFile << "现在的角度[0]" << "," << "现在的速度[0]" << "," << "现在的力矩[0]" << "," << "下个时刻的角度[0]" << "," << "下个时刻的速度[0]" << ",";
	oFile << "现在的角度[1]" << "," << "现在的速度[1]" << "," << "现在的力矩[1]" << "," << "下个时刻的角度[1]" << "," << "下个时刻的速度[1]" << ",";
	oFile << "现在的角度[2]" << "," << "现在的速度[2]" << "," << "现在的力矩[2]" << "," << "下个时刻的角度[2]" << "," << "下个时刻的速度[2]" << ",";
	oFile << "现在的角度[3]" << "," << "现在的速度[3]" << "," << "现在的力矩[3]" << "," << "下个时刻的角度[3]" << "," << "下个时刻的速度[3]";
	oFile << std::endl;
	fileopenflag = true;
 	// TODO:  在此添加控件通知处理程序代码
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);   //加载套接字库2.0版本
	if (err!=0)
	{
		update(_T("加载套接字库2.0失败"));
	}
	send_edit = (CEdit *)GetDlgItem(IDC_EDIT1);
	send_edit->SetFocus();
	char name[128];
	hostent* pHost;
	gethostname(name, 128);//获得主机名 
	pHost = gethostbyname(name);//获得主机结构 
	IP = inet_ntoa(*(struct in_addr*)pHost->h_addr);    //获取本机的IP地址
	DWORD dwIP;
	char ch_ip1[20];
	CString2Char(IP, ch_ip1);//注意！这里调用了字符格式转换函数，此函数功能：CString类型转换为Char类型，实现代码在后面添加
	dwIP = inet_addr(ch_ip1);
	unsigned char *pIP = (unsigned char*)&dwIP;
	m_TCPIPaddr.SetAddress(*(pIP), *(pIP + 1), *(pIP + 2), *(pIP+3));
	
	update(_T("本服务器IP地址：") + IP);

	/////////////////////下面是画图程序的初始化
	for (int i = 0; i < 3; i++)
	{
		ZeroMemory(&m_HightSpeedChartArray1[i], sizeof(double)*m_c_arrayLength);
		ZeroMemory(&m_HightSpeedChartArray2[i], sizeof(double)*m_c_arrayLength);
		ZeroMemory(&m_HightSpeedChartArray3[i], sizeof(double)*m_c_arrayLength);
		ZeroMemory(&m_HightSpeedChartArray4[i], sizeof(double)*m_c_arrayLength);
	}

	for (int i = 0; i < 3; i++)
	{
		m_pLineSerie1[i]->SetSeriesOrdering(poNoOrdering);
		m_pLineSerie2[i]->SetSeriesOrdering(poNoOrdering);
		m_pLineSerie3[i]->SetSeriesOrdering(poNoOrdering);
		m_pLineSerie4[i]->SetSeriesOrdering(poNoOrdering);
	}
	for (size_t i = 0; i<m_c_arrayLength; ++i)
	{
		m_X[i] = i;   //X轴的参数初始化
	}
	m_count = m_c_arrayLength;
	for (int i = 0; i < 3; i++)
	{
		m_pLineSerie1[i]->ClearSerie();
		m_pLineSerie2[i]->ClearSerie();
		m_pLineSerie3[i]->ClearSerie();
		m_pLineSerie4[i]->ClearSerie();
	}
	TChartStringStream strs1, strs2, strs3;
	strs1 << _T("角度值");
		m_pLineSerie1[0]->SetName(strs1.str());//SetName的作用将在后面讲到
		m_pLineSerie2[0]->SetName(strs1.str());
		m_pLineSerie3[0]->SetName(strs1.str());
		m_pLineSerie4[0]->SetName(strs1.str());
	strs2 << _T("速度值");
		m_pLineSerie1[1]->SetName(strs2.str());//SetName的作用将在后面讲到
		m_pLineSerie2[1]->SetName(strs2.str());
		m_pLineSerie3[1]->SetName(strs2.str());
		m_pLineSerie4[1]->SetName(strs2.str());
	strs3 << _T("力矩");
		m_pLineSerie1[2]->SetName(strs3.str());//SetName的作用将在后面讲到
		m_pLineSerie2[2]->SetName(strs3.str());
		m_pLineSerie3[2]->SetName(strs3.str());
		m_pLineSerie4[2]->SetName(strs3.str());
    m_ChartCtrl1.GetLegend()->SetVisible(true);
	m_ChartCtrl2.GetLegend()->SetVisible(true);
	m_ChartCtrl3.GetLegend()->SetVisible(true);
	m_ChartCtrl4.GetLegend()->SetVisible(true);
	///////////////////画图程序的初始化结束
	AfxBeginThread(server_thd, (LPVOID)GetSafeHwnd(), THREAD_PRIORITY_HIGHEST);//创建线程
}


void CShowRobotDataDlg::OnLbnSelchangeList1Showmessage()
{
	// TODO:  在此添加控件通知处理程序代码
}

//在list接受框中打印信息
void CShowRobotDataDlg::update(CString s)
{
	m_recvMessage.AddString(s);
}

/// \brief 左移数组  
/// \param ptr 数组指针  
/// \param data 新数值  
/// 
void LeftMoveArray(double* ptr, size_t length, double data)
{
	for (size_t i = 1; i<length; ++i)
	{
		ptr[i - 1] = ptr[i];
	}
	ptr[length - 1] = data;
}
/////左移数组，左移size位
void LeftMoveArrayWithQueue(double* ptr, size_t length, RobotData DealQueueData[],int size,int WhichData)
{
	for (size_t i = 0; i<(length-size); ++i)
	{
		ptr[i] = ptr[size+i];
	}
	switch (WhichData)
	{
	case 11:   //第一根轴的角度信息
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsNow[0];
		}
		break;
	case 12:   //第一根轴的速度信息
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsVelNow[0];
		}
		break;
	case 13:   //第一根轴的力矩信息
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsTorque[0];
		}
		break;
	case 21:   //第二根轴的角度信息
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsNow[1];
		}
		break;
	case 22:   //第二根轴的速度信息
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsVelNow[1];
		}
		break;
	case 23:   //第二根轴的力矩信息
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsTorque[1];
		}
		break;
	case 31:   //第三根轴的角度信息
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsNow[2];
		}
		break;
	case 32:   //第三根轴的角度信息
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsVelNow[2];
		}
		break;
	case 33:   //第三根轴的力矩信息
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsTorque[2];
		}
		break;
	case 41:   //第四根轴的角度信息
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsNow[3];
		}
		break;
	case 42:   //第四根轴的角度信息
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsVelNow[3];
		}
		break;
	case 43:   //第四根轴的力矩信息
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsTorque[3];
		}
		break;
	}


}

/////X轴数组的偏移
void LeftMoveArrayXWithQueue(double* ptr, size_t length, unsigned int& data, int size)
{
	for (size_t i = 0; i<(length-size); ++i)
	{
		ptr[i] = ptr[size+i];
	}
	for (int i = 0; i < size; i++)
	{
		ptr[length - size + i] = ++data;
	}
}


std::queue<RobotData> g_QueueData;  //全局队列
HANDLE g_hMutex;  //互斥量句柄
UINT server_thd(LPVOID p)//线程要调用的函数
{
	HWND hWnd = (HWND)p;
	SOCKADDR_IN local_addr;
	SOCKADDR_IN client_addr;
	int iaddrSize = sizeof(SOCKADDR_IN);
	int res;
	CShowRobotDataDlg * dlg = (CShowRobotDataDlg *)AfxGetApp()->GetMainWnd();
	char ch_ip[20];
	CString2Char(IP, ch_ip);//注意！这里调用了字符格式转换函数，此函数功能：CString类型转换为Char类型，实现代码在后面添加
	//local_addr.sin_addr.s_addr = htonl(INADDR_ANY);//获取任意IP地址
	local_addr.sin_addr.s_addr = inet_addr(ch_ip);
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(8888);   //端口号为8888
	dlg->SetDlgItemText(IDC_EDIT2_TCPPort, _T("8888"));     //写入

	if ((listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)//创建套接字
	{
		dlg->update(_T("创建监听套接字失败"));
	}
	if (bind(listen_sock, (struct sockaddr*) &local_addr, sizeof(SOCKADDR_IN)))//绑定套接字
	{
		dlg->update(_T("绑定错误"));
	}

    listen(listen_sock, 1);     //开始监听,这里使用的是阻塞模式
	if ((sock = accept(listen_sock, (struct sockaddr *)&client_addr, &iaddrSize)) == INVALID_SOCKET)//接收套接字
	{
		dlg->update(_T("accept 失败"));
	}
	else
	{
		CString port;
		port.Format(_T("%d"), int(ntohs(client_addr.sin_port)));
		dlg->update(_T("已连接客户端：") + CString(inet_ntoa(client_addr.sin_addr)) + "  端口：" + port);
	}


	////////////接收数据
	g_hMutex = CreateMutex(NULL, FALSE, NULL);   //创建无名的互斥量，这个互斥量不被任何线程占有
	RobotData MyRobotData;
	char recbuf[sizeof(MyRobotData)];
	memset(recbuf, 0, sizeof(MyRobotData));
	while (1)
	{		
		if ((res = recv(sock, recbuf, sizeof(recbuf), 0)) == -1)    //这个rev函数也是阻塞模式
		{
			dlg->update(_T("失去客户端的连接"));
			break;
		}
		else
		{
			memset(&MyRobotData, 0, sizeof(MyRobotData));
			memcpy(&MyRobotData, recbuf, sizeof(MyRobotData));

			//////保存这个文件为excel
			oFile.precision(6); //设置精度
			//发现传输过程中有一些坏点，将这些观点剔除掉
			if (MyRobotData.JointsNow[0] <= 1000 && MyRobotData.JointsNow[0] >= -1000 && MyRobotData.JointsVelNow[0] <= 100 && MyRobotData.JointsVelNow[0] >= -100)
			{
				oFile << MyRobotData.JointsNow[0] << "," << MyRobotData.JointsVelNow[0] << "," << MyRobotData.JointsTorque[0] << "," << MyRobotData.JointsNext[0] << "," << MyRobotData.JointsVelNext[0] << ",";
				oFile << MyRobotData.JointsNow[1] << "," << MyRobotData.JointsVelNow[1] << "," << MyRobotData.JointsTorque[1] << "," << MyRobotData.JointsNext[1] << "," << MyRobotData.JointsVelNext[1] << ",";
				oFile << MyRobotData.JointsNow[2] << "," << MyRobotData.JointsVelNow[2] << "," << MyRobotData.JointsTorque[2] << "," << MyRobotData.JointsNext[2] << "," << MyRobotData.JointsVelNext[2] << ",";
				oFile << MyRobotData.JointsNow[3] << "," << MyRobotData.JointsVelNow[3] << "," << MyRobotData.JointsTorque[3] << "," << MyRobotData.JointsNext[3] << "," << MyRobotData.JointsVelNext[3];
				oFile << std::endl;
			}

			//////保存这个文件为excel结束

			//****使用队列的方式来完成任务			
			WaitForSingleObject(g_hMutex, INFINITE);    //使用互斥量来保护g_QueueData队列读取和插入分开
			if (MyRobotData.JointsNow[0] <= 1000 && MyRobotData.JointsNow[0] >= -1000 && MyRobotData.JointsVelNow[0] <= 100 && MyRobotData.JointsVelNow[0] >= -100)
			{
				g_QueueData.push(MyRobotData);
			}
			ReleaseMutex(g_hMutex);
			//****使用队列的方式来完成任务结束




			////******使用发送消息的方式来完成任务
			//::PostMessageA(hWnd,UM_DRAWROBOTDATA,(WPARAM)&MyRobotData,1);
			////******使用发送消息的方式来完成任务结束
		}


	}
	WSACleanup();
	return 0;
}

LRESULT CShowRobotDataDlg::OnDrawRobotData(WPARAM wParam, LPARAM lParam)
{
	RobotData *myRobotData;
	myRobotData = (RobotData*)wParam;
	++m_count;
	m_pLineSerie1[0]->ClearSerie();
	m_pLineSerie1[1]->ClearSerie();
	m_pLineSerie1[2]->ClearSerie();

	m_pLineSerie2[0]->ClearSerie();
	m_pLineSerie2[1]->ClearSerie();
	m_pLineSerie2[2]->ClearSerie();

	m_pLineSerie3[0]->ClearSerie();
	m_pLineSerie3[1]->ClearSerie();
	m_pLineSerie3[2]->ClearSerie();

	m_pLineSerie4[0]->ClearSerie();
	m_pLineSerie4[1]->ClearSerie();
	m_pLineSerie4[2]->ClearSerie();

	LeftMoveArray(m_HightSpeedChartArray1[0], m_c_arrayLength, myRobotData->JointsNow[0]);  //角度值
	LeftMoveArray(m_HightSpeedChartArray1[1], m_c_arrayLength, myRobotData->JointsVelNow[0]);  //速度值
	LeftMoveArray(m_HightSpeedChartArray1[2], m_c_arrayLength, myRobotData->JointsNow[0]);  //力矩

	LeftMoveArray(m_HightSpeedChartArray2[0], m_c_arrayLength, myRobotData->JointsNow[1]);  //角度值
	LeftMoveArray(m_HightSpeedChartArray2[1], m_c_arrayLength, myRobotData->JointsVelNow[1]);  //速度值
	LeftMoveArray(m_HightSpeedChartArray2[2], m_c_arrayLength, myRobotData->JointsNow[1]);  //力矩

	LeftMoveArray(m_HightSpeedChartArray3[0], m_c_arrayLength, myRobotData->JointsNow[2]);  //角度值
	LeftMoveArray(m_HightSpeedChartArray3[1], m_c_arrayLength, myRobotData->JointsVelNow[2]);  //速度值
	LeftMoveArray(m_HightSpeedChartArray3[2], m_c_arrayLength, myRobotData->JointsNow[2]);  //力矩

	LeftMoveArray(m_HightSpeedChartArray4[0], m_c_arrayLength, myRobotData->JointsNow[3]);  //角度值
	LeftMoveArray(m_HightSpeedChartArray4[1], m_c_arrayLength, myRobotData->JointsVelNow[3]);  //速度值
	LeftMoveArray(m_HightSpeedChartArray4[2], m_c_arrayLength, myRobotData->JointsNow[3]);  //力矩

	LeftMoveArray(m_X, m_c_arrayLength, m_count);

	m_pLineSerie1[0]->AddPoints(m_X, m_HightSpeedChartArray1[0], m_c_arrayLength);
	m_pLineSerie1[1]->AddPoints(m_X, m_HightSpeedChartArray1[1], m_c_arrayLength);
	m_pLineSerie1[2]->AddPoints(m_X, m_HightSpeedChartArray1[2], m_c_arrayLength);

	m_pLineSerie2[0]->AddPoints(m_X, m_HightSpeedChartArray2[0], m_c_arrayLength);
	m_pLineSerie2[1]->AddPoints(m_X, m_HightSpeedChartArray2[1], m_c_arrayLength);
	m_pLineSerie2[2]->AddPoints(m_X, m_HightSpeedChartArray2[2], m_c_arrayLength);

	m_pLineSerie3[0]->AddPoints(m_X, m_HightSpeedChartArray3[0], m_c_arrayLength);
	m_pLineSerie3[1]->AddPoints(m_X, m_HightSpeedChartArray3[1], m_c_arrayLength);
	m_pLineSerie3[2]->AddPoints(m_X, m_HightSpeedChartArray3[2], m_c_arrayLength);

	m_pLineSerie4[0]->AddPoints(m_X, m_HightSpeedChartArray4[0], m_c_arrayLength);
	m_pLineSerie4[1]->AddPoints(m_X, m_HightSpeedChartArray4[1], m_c_arrayLength);
	m_pLineSerie4[2]->AddPoints(m_X, m_HightSpeedChartArray4[2], m_c_arrayLength);

	return 1;
}

void CShowRobotDataDlg::DrawData(RobotData myrobotdata)
{
	++m_count;
	//for (int i = 0; i < 3; i++)
	//{
	//	m_pLineSerie1[i]->ClearSerie();
	//	m_pLineSerie2[i]->ClearSerie();
	//	m_pLineSerie3[i]->ClearSerie();
	//	m_pLineSerie4[i]->ClearSerie();
	//}
	m_pLineSerie1[0]->ClearSerie();
	m_pLineSerie1[1]->ClearSerie();
	m_pLineSerie1[2]->ClearSerie();

	LeftMoveArray(m_HightSpeedChartArray1[0], m_c_arrayLength, myrobotdata.JointsNow[0]);  //角度值
	LeftMoveArray(m_HightSpeedChartArray1[1], m_c_arrayLength, myrobotdata.JointsVelNow[0]);  //速度值
	LeftMoveArray(m_HightSpeedChartArray1[2], m_c_arrayLength, myrobotdata.JointsNow[0]);  //力矩

	LeftMoveArray(m_X, m_c_arrayLength, m_count);

	m_pLineSerie1[0]->AddPoints(m_X, m_HightSpeedChartArray1[0], m_c_arrayLength);
	m_pLineSerie1[1]->AddPoints(m_X, m_HightSpeedChartArray1[1], m_c_arrayLength);
	m_pLineSerie1[2]->AddPoints(m_X, m_HightSpeedChartArray1[2], m_c_arrayLength);

	//LeftMoveArray(m_HightSpeedChartArray2[0], m_c_arrayLength, myrobotdata.JointsNow[1]);  //角度值
	//LeftMoveArray(m_HightSpeedChartArray2[1], m_c_arrayLength, myrobotdata.JointsVelNow[1]);  //速度值
	//LeftMoveArray(m_HightSpeedChartArray2[2], m_c_arrayLength, myrobotdata.JointsNow[1]);  //力矩

	//LeftMoveArray(m_HightSpeedChartArray3[0], m_c_arrayLength, myrobotdata.JointsNow[2]);  //角度值
	//LeftMoveArray(m_HightSpeedChartArray3[1], m_c_arrayLength, myrobotdata.JointsVelNow[2]);  //速度值
	//LeftMoveArray(m_HightSpeedChartArray3[2], m_c_arrayLength, myrobotdata.JointsNow[2]);  //力矩

	//LeftMoveArray(m_HightSpeedChartArray4[0], m_c_arrayLength, myrobotdata.JointsNow[3]);  //角度值
	//LeftMoveArray(m_HightSpeedChartArray4[1], m_c_arrayLength, myrobotdata.JointsVelNow[3]);  //速度值
	//LeftMoveArray(m_HightSpeedChartArray4[2], m_c_arrayLength, myrobotdata.JointsNow[3]);  //力矩

	LeftMoveArray(m_X, m_c_arrayLength, m_count);

	m_pLineSerie1[0]->AddPoints(m_X, m_HightSpeedChartArray1[0], m_c_arrayLength);
	m_pLineSerie1[1]->AddPoints(m_X, m_HightSpeedChartArray1[1], m_c_arrayLength);
	m_pLineSerie1[2]->AddPoints(m_X, m_HightSpeedChartArray1[2], m_c_arrayLength);

	//m_pLineSerie2[0]->AddPoints(m_X, m_HightSpeedChartArray2[0], m_c_arrayLength);
	//m_pLineSerie2[1]->AddPoints(m_X, m_HightSpeedChartArray2[1], m_c_arrayLength);
	//m_pLineSerie2[2]->AddPoints(m_X, m_HightSpeedChartArray2[2], m_c_arrayLength);

	//m_pLineSerie3[0]->AddPoints(m_X, m_HightSpeedChartArray3[0], m_c_arrayLength);
	//m_pLineSerie3[1]->AddPoints(m_X, m_HightSpeedChartArray3[1], m_c_arrayLength);
	//m_pLineSerie3[2]->AddPoints(m_X, m_HightSpeedChartArray3[2], m_c_arrayLength);

	//m_pLineSerie4[0]->AddPoints(m_X, m_HightSpeedChartArray4[0], m_c_arrayLength);
	//m_pLineSerie4[1]->AddPoints(m_X, m_HightSpeedChartArray4[1], m_c_arrayLength);
	//m_pLineSerie4[2]->AddPoints(m_X, m_HightSpeedChartArray4[2], m_c_arrayLength);
}


RobotData DealQueueData[100];    //定义全局变量比较省时间
void CShowRobotDataDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	////****使用队列的方式来完成任务			
	WaitForSingleObject(g_hMutex, INFINITE);
	int DataSize = g_QueueData.size();   //返回队列中元素个数
	for (int i = 0; i < DataSize; i++)
	{
		DealQueueData[i] = g_QueueData.front();   //取出
		g_QueueData.pop();
	}
	ReleaseMutex(g_hMutex);
	////****使用队列的方式来完成任务结束
	//++m_count;
	m_pLineSerie1[0]->ClearSerie();
	m_pLineSerie1[1]->ClearSerie();
	m_pLineSerie1[2]->ClearSerie();
	m_pLineSerie2[0]->ClearSerie();
	m_pLineSerie2[1]->ClearSerie();
	m_pLineSerie2[2]->ClearSerie();
	m_pLineSerie3[0]->ClearSerie();
	m_pLineSerie3[1]->ClearSerie();
	m_pLineSerie3[2]->ClearSerie();

	LeftMoveArrayWithQueue(m_HightSpeedChartArray1[0], m_c_arrayLength, DealQueueData, DataSize,11);  //角度值
	LeftMoveArrayWithQueue(m_HightSpeedChartArray1[1], m_c_arrayLength, DealQueueData, DataSize,12);  //速度值
	LeftMoveArrayWithQueue(m_HightSpeedChartArray1[2], m_c_arrayLength, DealQueueData, DataSize,13);  //力矩

	LeftMoveArrayWithQueue(m_HightSpeedChartArray2[0], m_c_arrayLength, DealQueueData, DataSize, 21);  //角度值
	LeftMoveArrayWithQueue(m_HightSpeedChartArray2[1], m_c_arrayLength, DealQueueData, DataSize, 22);  //速度值
	LeftMoveArrayWithQueue(m_HightSpeedChartArray2[2], m_c_arrayLength, DealQueueData, DataSize, 23);  //力矩

	LeftMoveArrayWithQueue(m_HightSpeedChartArray3[0], m_c_arrayLength, DealQueueData, DataSize, 31);  //角度值
	LeftMoveArrayWithQueue(m_HightSpeedChartArray3[1], m_c_arrayLength, DealQueueData, DataSize, 32);  //速度值
	LeftMoveArrayWithQueue(m_HightSpeedChartArray3[2], m_c_arrayLength, DealQueueData, DataSize, 33);  //力矩

	LeftMoveArrayXWithQueue(m_X, m_c_arrayLength, m_count, DataSize);

	m_pLineSerie1[0]->AddPoints(m_X, m_HightSpeedChartArray1[0], m_c_arrayLength);
	m_pLineSerie1[1]->AddPoints(m_X, m_HightSpeedChartArray1[1], m_c_arrayLength);
	m_pLineSerie1[2]->AddPoints(m_X, m_HightSpeedChartArray1[2], m_c_arrayLength);

	m_pLineSerie2[0]->AddPoints(m_X, m_HightSpeedChartArray2[0], m_c_arrayLength);
	m_pLineSerie2[1]->AddPoints(m_X, m_HightSpeedChartArray2[1], m_c_arrayLength);
	m_pLineSerie2[2]->AddPoints(m_X, m_HightSpeedChartArray2[2], m_c_arrayLength);

	m_pLineSerie3[0]->AddPoints(m_X, m_HightSpeedChartArray3[0], m_c_arrayLength);
	m_pLineSerie3[1]->AddPoints(m_X, m_HightSpeedChartArray3[1], m_c_arrayLength);
	m_pLineSerie3[2]->AddPoints(m_X, m_HightSpeedChartArray3[2], m_c_arrayLength);


	CDialogEx::OnTimer(nIDEvent);
}


void CShowRobotDataDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (fileopenflag)
	{
		oFile.close();
	}
	CDialogEx::OnClose();
}
