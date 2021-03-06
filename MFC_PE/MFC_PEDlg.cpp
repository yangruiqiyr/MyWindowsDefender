
// MFC_PEDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC_PE.h"
#include "MFC_PEDlg.h"
#include "afxdialogex.h"
#include "DiaTask.h"
#include "DiaCPU.h"
#include"DiaFile.h"
#include"DiaServerAndRegister.h"
#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include <fstream>
#include <TlHelp32.h>
#include"DiaVirus.h"
#include"SoftMgr.h"
#include"Boot.h"
using namespace std;

#define WM_MYKEY WM_USER+1
#ifdef _DEBUG
#define new DEBUG_NEW
#endif



BOOL    AddPrivilege(HANDLE hProcess, const TCHAR* pszPrivilegeName)
{
	// 进程的特权使用LUID值来表示, 因此, 需要先获取传入的权限名对应的LUID值


	// 0. 获取特权对应的LUID值
	LUID privilegeLuid;
	if (!LookupPrivilegeValue(NULL, pszPrivilegeName, &privilegeLuid))
		return FALSE;


	// 1. 获取本进程令牌
	HANDLE hToken = NULL;
	// 打开令牌时, 需要加上TOKEN_ADJUST_PRIVILEGES 权限(这个权限用于修改令牌特权)
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		printf("错误码:%x\n", GetLastError());
		return 0;
	}

	// 2. 使用令牌特权修改函数将SeDebug的LUID特权值添加到本进程令牌
	TOKEN_PRIVILEGES tokenPrivieges; // 新的特权

									 // 使用特权的LUID来初始化结构体.
	tokenPrivieges.PrivilegeCount = 1; // 特权个数
	tokenPrivieges.Privileges[0].Luid = privilegeLuid; // 将特权LUID保存到数组中
	tokenPrivieges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;// 将属性值设为启用(有禁用,移除等其他状态)



																   // 调用函数添加特权
	return AdjustTokenPrivileges(hToken,              // 要添加特权的令牌
		FALSE,               // TRUE是移除特权, FALSE是添加特权
		&tokenPrivieges,     // 要添加的特权数组
		sizeof(tokenPrivieges),// 整个特权数组的大小
		NULL,                // 旧的特权数组
		NULL                  // 旧的特权数组的长度
	);
}


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
public:
	afx_msg void OnBnClickedLoadScan();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &CAboutDlg::OnBnClickedLoadScan)
END_MESSAGE_MAP()


// CMFC_PEDlg 对话框



CMFC_PEDlg::CMFC_PEDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFC_PE_DIALOG, pParent)
	, m_PEload(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC_PEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MFCEDITBROWSE1, m_PEload);
}

BEGIN_MESSAGE_MAP(CMFC_PEDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	//ON_BN_CLICKED(IDC_BUTTON1, &CMFC_PEDlg::OnBnClickedConfirm)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFC_PEDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CMFC_PEDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFC_PEDlg::OnBnClickedMCPU)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFC_PEDlg::OnBnClickedFile)
	ON_BN_CLICKED(IDC_BUTTON5, &CMFC_PEDlg::OnBnClickedRegister)
	ON_BN_CLICKED(IDC_BUTTON8, &CMFC_PEDlg::OnBnClickedVirus)
	ON_BN_CLICKED(IDC_BUTTON9, &CMFC_PEDlg::OnBnClickedUninstall)
	ON_BN_CLICKED(IDC_BUTTON10, &CMFC_PEDlg::OnBnClickedStart)
END_MESSAGE_MAP()


// CMFC_PEDlg 消息处理程序

BOOL CMFC_PEDlg::OnInitDialog()
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

	// TODO: 在此添加额外的初始化代码
	//初始化白名单
	WhiteSave();

	//初始化时遍历进程，将进程存储到
	//注册热键
	RegisterHotKey(m_hWnd, WM_MYKEY, MOD_CONTROL | MOD_ALT, 'H');

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFC_PEDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFC_PEDlg::OnPaint()
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
HCURSOR CMFC_PEDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFC_PEDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnDropFiles(hDropInfo);
	int DropCount = DragQueryFile(hDropInfo, -1, NULL, 0);//取得被拖动文件的数目
	for (int i = 0;i < DropCount;i++) {
		WCHAR wcStr[MAX_PATH];
		DragQueryFile(hDropInfo, i, wcStr, MAX_PATH);//获得拖拽的第i个文件的文件名
		//需要做的事情
		m_PEload = wcStr;
		CPEeditor EPeditor1(m_pNt);
		EPeditor1.m_PEload = m_PEload;
		EPeditor1.DoModal();
		DragFinish(hDropInfo); //拖拽结束后，释放内存
	}
}


//void CMFC_PEDlg::OnBnClickedConfirm()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	UpdateData(TRUE);
//	CPEeditor EPeditor1;
//	EPeditor1.m_PEload = m_PEload;
//	EPeditor1.DoModal();
//}


void CMFC_PEDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码

	TCHAR szDir[MAX_PATH] = { 0 };
	//打开文件
	OPENFILENAME ofn = { sizeof(OPENFILENAME) };
	TCHAR szFileName[MAX_PATH] = { 0 };
	ofn.Flags = OFN_HIDEREADONLY;
	ofn.hwndOwner = NULL;
	TCHAR szFilter[] = TEXT("EXE Files(*.exe)\0*.exe\0")TEXT("DLL Files(*.dll)\0*.dll\0")TEXT("All Files(*.*)\0*.*\0\0");
	ofn.lpstrFilter = szFilter;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;
	//用来保存文件路径
	ofn.lpstrFile = szDir;
	ofn.lpstrFileTitle = szFileName;
	ofn.lpstrInitialDir = szDir;
	GetOpenFileName(&ofn);
	m_PEload = ofn.lpstrFile;
	m_pNt=getPNT(m_PEload);
	//点击确定跳出
	CPEeditor EPeditor1(m_pNt);
	EPeditor1.m_PEload = m_PEload;
	EPeditor1.DoModal();
}

PIMAGE_NT_HEADERS32 CMFC_PEDlg::getPNT(CString m_PEload)
{
	//获取文件句柄
	HANDLE hFile = CreateFile((m_PEload.GetBuffer()), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//获取文件大小
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	//创建一个堆空间存放
	CHAR *pFileBuf = new CHAR[dwFileSize];

	DWORD ReadFileSize = 0;
	//将文件读入内存
	ReadFile(hFile, pFileBuf, dwFileSize, &ReadFileSize, NULL);
	//判断DOS头
	PIMAGE_DOS_HEADER pFile = (PIMAGE_DOS_HEADER)pFileBuf;
	if (pFile->e_magic != IMAGE_DOS_SIGNATURE) {//0x5A4D
		cout << "这不是一个PE文件" << endl;
		return 0;
	}
	//判断PE头部
	DWORD dwNewPos = (DWORD)pFileBuf + ((PIMAGE_DOS_HEADER)pFileBuf)->e_lfanew;
	PIMAGE_NT_HEADERS32 pNTHeader = (PIMAGE_NT_HEADERS32)(dwNewPos);
	CloseHandle(hFile);
	return pNTHeader;

}


void CMFC_PEDlg::OnBnClickedButton1()//按下任务管理器
{
	// TODO: 在此添加控件通知处理程序代码
	CDiaTask temp1;
	temp1.DoModal();
}


void CMFC_PEDlg::OnBnClickedMCPU()
{
	// TODO: 在此添加控件通知处理程序代码
	CDiaCPU tempCpu;
	//tempCpu.m_Flag = 0;
	//tempCpu.DoModal();
	CDiaCPU *pTD = new CDiaCPU();
	pTD->Create(IDD_DIACPU); //创建一个非模态对话框  
	pTD->ShowWindow(SW_SHOWNORMAL);
}


void CMFC_PEDlg::OnBnClickedFile()
{
	// TODO: 在此添加控件通知处理程序代码
	CDiaFile tempFile;
	tempFile.DoModal();
}


void CMFC_PEDlg::OnBnClickedRegister()
{
	bool ret=AddPrivilege(GetCurrentProcess(), SE_DEBUG_NAME/*字符串形式的权限名*/);
	// TODO: 在此添加控件通知处理程序代码
	CDiaServerAndRegister tempServer;
	tempServer.DoModal();
}


void CMFC_PEDlg::OnBnClickedVirus()
{
	// TODO: 在此添加控件通知处理程序代码
	CDiaVirus tempVirus(&m_client);
	if (!m_client.ConnectServer("127.0.0.1", 1234)) {
		MessageBox(L"连接服务器失败！", L"Error!", MB_OK | MB_ICONWARNING);
		return;
	}
	tempVirus.DoModal();
}

void CMFC_PEDlg::WhiteSave()
{
	//fstream output_stream;
	//output_stream.open("D:\\temp.txt", ios::out | ios::app);
	FILE *pfile = NULL;
	if (!fopen_s(&pfile, "D:\\temp.txt", "w+"));


	HANDLE hTool32 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (INVALID_HANDLE_VALUE == hTool32)
	{
		printf("快照error!\n");
		return;
	}
	// 2. 开始遍历进程
	PROCESSENTRY32 ti = { sizeof(PROCESSENTRY32) };
	BOOL bRet = Process32First(hTool32, &ti);
	if (!bRet)
	{

		//printf("Thread32First error!\n");
		return;
	}
	int i = 0;
	do
	{
		//if (ti.th32OwnerProcessID == dwPID)
		{
			//WCHAR temp[100] = {};
			//wmemcpy_s(temp, 100, ti.szExeFile, 100);
			//output_stream<< "12346" <<endl;
			fprintf(pfile, "%S\n", ti.szExeFile);
			i++;
		}
	} while (Process32Next(hTool32, &ti));
	//output_stream.close();
	fclose(pfile);
}


void CAboutDlg::OnBnClickedLoadScan()
{
	// TODO: 在此添加控件通知处理程序代码
}


BOOL CMFC_PEDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
		if ((pMsg->message == WM_HOTKEY) && (pMsg->wParam == WM_MYKEY))
		{
			if (IsWindowVisible())
				ShowWindow(SW_HIDE);
			else
				ShowWindow(SW_SHOW);
		}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMFC_PEDlg::OnBnClickedUninstall()
{
	// TODO: 在此添加控件通知处理程序代码
	CSoftMgr tempSoft;
	tempSoft.DoModal();
}


void CMFC_PEDlg::OnBnClickedStart()
{
	// TODO: 在此添加控件通知处理程序代码
	CBoot tempBoot;
	tempBoot.DoModal();
}
