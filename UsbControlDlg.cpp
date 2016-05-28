
// UsbControlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UsbControl.h"
#include "UsbControlDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int g_width;
int g_height;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

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

};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CUsbControlDlg 对话框




CUsbControlDlg::CUsbControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUsbControlDlg::IDD, pParent)
	, m_iRdoDriver(0)
	, m_iProcType(0)
	, m_iChangeType(0)
	, m_sEdit_Width(_T(""))
	, m_sEdit_Height(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pFileRbf=NULL;
	m_bUsbOpen=FALSE;
	m_bSendData=FALSE;
	memset(m_byData,0,sizeof(BYTE)*64);
	m_sUsbOrder.pData=m_byData;
	m_pVideoDlg=NULL;
	m_pDataProcess=NULL;
	m_pDataCapture=NULL;
	m_hDisplayDC=NULL;
	m_hThread=NULL;
	m_pReadBuff=new char[ReadDataBytes];
	memset(m_pReadBuff,0,sizeof(char)*ReadDataBytes);
	m_bCapture=FALSE;
	m_bCloseWnd=FALSE;
	m_lBytePerSecond=0;
	m_CyDriver=NEW_DRIVER;
	m_pBrush=NULL;
	m_pVideoDataFile=NULL;
	m_bReview=FALSE;
	m_bSave=FALSE;
	m_Init = FALSE;
}

CUsbControlDlg::~CUsbControlDlg()
{
	if(m_pReadBuff!=NULL)
	{
		delete[] m_pReadBuff;
		m_pReadBuff=NULL;
	}
}

void CUsbControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_NEWDRIVER, m_iRdoDriver);
	DDX_Radio(pDX, IDC_RADIO_NORMAL, m_iProcType);
	DDX_Radio(pDX, IDC_RADIO_NORMAL_CHANGE, m_iChangeType);
	DDX_Control(pDX, IDC_EDIT1, m_Edit_Width);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_Height);
	DDX_Text(pDX, IDC_EDIT1, m_sEdit_Width);
	DDX_Text(pDX, IDC_EDIT2, m_sEdit_Height);
}

BEGIN_MESSAGE_MAP(CUsbControlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_OPENUSB, &CUsbControlDlg::OnBnClickedBtnOpenusb)
	ON_BN_CLICKED(IDC_BTN_OPENFILE, &CUsbControlDlg::OnBnClickedBtnOpenfile)
	ON_WM_TIMER()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_VIDEOCAPTURE, &CUsbControlDlg::OnBnClickedBtnVideocapture)
	ON_BN_CLICKED(IDC_BTN_STOPCAPTURE, &CUsbControlDlg::OnBnClickedBtnStopcapture)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_NEWDRIVER, &CUsbControlDlg::OnBnClickedRadioDriver)
	ON_BN_CLICKED(IDC_RADIO_OLDDRIVER, &CUsbControlDlg::OnBnClickedRadioDriver)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_NORMAL, &CUsbControlDlg::OnBnClickedRadioProcType)
	ON_BN_CLICKED(IDC_RADIO_XMIRROR, &CUsbControlDlg::OnBnClickedRadioProcType)
	ON_BN_CLICKED(IDC_RADIO_YMIRROR, &CUsbControlDlg::OnBnClickedRadioProcType)
	ON_BN_CLICKED(IDC_RADIO_XYMIRROR, &CUsbControlDlg::OnBnClickedRadioProcType)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CUsbControlDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_REVIEW, &CUsbControlDlg::OnBnClickedBtnReview)
	ON_BN_CLICKED(IDC_RADIO_NORMAL_CHANGE, &CUsbControlDlg::OnBnClickedRadioChangeType)
	ON_BN_CLICKED(IDC_RADIO_RG_CHANGE, &CUsbControlDlg::OnBnClickedRadioChangeType)
	ON_BN_CLICKED(IDC_RADIO_RB_CHANGE, &CUsbControlDlg::OnBnClickedRadioChangeType)
	ON_BN_CLICKED(IDC_RADIO_GB_CHANGE, &CUsbControlDlg::OnBnClickedRadioChangeType)
	ON_BN_CLICKED(IDC_BTN_CREATEBMP, &CUsbControlDlg::OnBnClickedBtnCreatebmp)
	ON_EN_CHANGE(IDC_EDIT1, &CUsbControlDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &CUsbControlDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON2, &CUsbControlDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_PipeReset, &CUsbControlDlg::OnBnClickedPipereset)
	ON_BN_CLICKED(IDC_I2CRead, &CUsbControlDlg::OnBnClickedI2cread)
END_MESSAGE_MAP()


// CUsbControlDlg 消息处理程序

BOOL CUsbControlDlg::OnInitDialog()
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//------------------------------------
	CRect cRect,wRect,mRect;
	GetDesktopWindow()->GetWindowRect(wRect);
	GetWindowRect(cRect);
	mRect.right=wRect.right-50;
	mRect.bottom=wRect.bottom-50;
	mRect.left=mRect.right-cRect.Width();
	mRect.top=mRect.bottom-cRect.Height();
	MoveWindow(mRect);
	//------------------------------------
	m_pFileRbf=new CFile();
	m_pVideoDlg=new CVideoDlg();
	m_pDataProcess=new CDataProcess();
	m_pDataCapture=new CDataCapture();
	m_pVideoDlg->Create(IDD_DLG_VIDEO,this);
	m_pVideoDlg->ShowWindow(FALSE);
	m_hDisplayDC=m_pVideoDlg->GetDisplayDC()->m_hDC;

	SetTimer(1,1000,NULL);

	CyUsb_Init(m_CyDriver);
	m_iRdoDriver=(int)m_CyDriver;

	m_pBrush=new CBrush[2];
	m_pBrush[0].CreateSolidBrush(RGB(99,208,242));
	m_pBrush[1].CreateSolidBrush(RGB(174,238,250));
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUsbControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();//加载控制台窗口模型
		int temp1 = 1;
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUsbControlDlg::OnPaint()
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
HCURSOR CUsbControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUsbControlDlg::OnBnClickedBtnOpenusb()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_bSendData)
	{
		AfxMessageBox(L"数据发送中，请稍等！");
		return;
	}
	if(m_bUsbOpen)
	{
		CloseUsb();
	}
	m_bUsbOpen=OpenUsb()==0?TRUE:FALSE;
	if(!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_TEXT,L"USB设备打开失败！");
		return;
	}
	SetDlgItemText(IDC_STATIC_TEXT,L"USB设备打开成功！");
}

void CUsbControlDlg::OnBnClickedBtnOpenfile()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fDlg(true);
	fDlg.m_ofn.lpstrFilter=L"RBF Files(*.rbf)\0*.rbf\0\0";
	if(fDlg.DoModal()==IDOK)
	{
		m_strRbfFileName=fDlg.GetPathName();
		BOOL bFileOpen=FALSE;
		bFileOpen=OpenRbfFile();
		if(bFileOpen&&m_bUsbOpen)
		{
			m_sUsbOrder.ReqCode=0xC0;
			m_sUsbOrder.DataBytes=1;
			m_sUsbOrder.Direction=1;
			for(int i=0;i<50;++i)	
			{
				if(SendOrder(&m_sUsbOrder)==0)
				{
					switch(m_byData[0])
					{
					case 0x00:
						Sleep(1);
						break;
					case 0xC0:
						SetDlgItemText(IDC_STATIC_TEXT,L"硬件初始化成功,开始发送配置数据！");
						SendData();
						CloseRbfFile();
						return;
					default:
						break;
					}
				}
			}
			CloseRbfFile();
			SetDlgItemText(IDC_STATIC_TEXT,L"硬件初始化失败,请重新初始化！");
		}
	}
}

BOOL CUsbControlDlg::CloseRbfFile()
{
	if(m_pFileRbf!=NULL)
	{
		if(m_pFileRbf->m_hFile!=CFile::hFileNull)
		{
			m_pFileRbf->Close();
		}
		delete m_pFileRbf;
		m_pFileRbf=NULL;
	}
	return TRUE;
}

BOOL CUsbControlDlg::OpenRbfFile()
{
	CloseRbfFile();
	m_pFileRbf=new CFile;
	return m_pFileRbf->Open(m_strRbfFileName,CFile::modeRead|CFile::shareDenyNone);
}

void CUsbControlDlg::SendData()
{
	try
	{
		m_bSendData=TRUE;
		BOOL bSend=TRUE;
		do 
		{
			m_sUsbOrder.DataBytes=m_pFileRbf->Read(m_sUsbOrder.pData,64);
			if(m_sUsbOrder.DataBytes>0)
			{
				m_sUsbOrder.ReqCode=0xC1;
				m_sUsbOrder.Direction=0;
				SendOrder(&m_sUsbOrder);
				SetDlgItemText(IDC_STATIC_TEXT,L"FPGA配置数据发送中.......");
			}
			else
			{
				m_sUsbOrder.ReqCode=0xC2;
				m_sUsbOrder.DataBytes=1;
				m_sUsbOrder.Direction=1;
				SetDlgItemText(IDC_STATIC_TEXT,L"FPGA配置数据发送结束.......");

				SendOrder(&m_sUsbOrder);
				switch(m_byData[0])
				{
				case 0xC2:
					SetDlgItemText(IDC_STATIC_TEXT,L"FPGA配置成功！");
					break;
				case 0x00:
				default:
					SetDlgItemText(IDC_STATIC_TEXT,L"FPGA配置失败,请重新配置！");
					break;
				}
				bSend=FALSE;
			}
		} while(bSend);
		m_bSendData=FALSE;
	}
	catch (CException* e)
	{
		if(CloseUsb()==0)
		{
			m_bUsbOpen=FALSE;
			SetDlgItemText(IDC_STATIC_TEXT,L"发生异常，请重新配置！");
		}
		else
		{
			SetDlgItemText(IDC_STATIC_TEXT,L"异常无法修复，请重新启动程序！");
		}
	}
}

void CUsbControlDlg::OnBnClickedBtnVideocapture()
{
	// TODO: 在此添加控件通知处理程序代码
	m_pVideoDlg->ShowWindow(TRUE);
	m_bCapture=TRUE;
	if (!m_Init)
	{
		m_pDataProcess->Open(m_hDisplayDC);
		m_pDataCapture->Open(m_pDataProcess);	
		m_hThread=(HANDLE)_beginthreadex(NULL,0,ThreadProcess,this,0,NULL);
		m_Init = TRUE;
	}
}

unsigned int __stdcall CUsbControlDlg::ThreadProcess( void* pParam )
{
	CUsbControlDlg* pThis=(CUsbControlDlg*)pParam;
	pThis->ThreadProcessFunction();
	return 0;
}

void CUsbControlDlg::ThreadProcessFunction()
{
	long lRet=0;
	int failcnt=0;
	while(TRUE)
	{
		if(m_bCloseWnd)
			break;
		if(m_bCapture)
		{
			lRet=ReadDataBytes;
			ReadData(m_pReadBuff,lRet);
			if(lRet>0)
			{
				m_lBytePerSecond+=lRet;
				m_pDataCapture->Input(m_pReadBuff,lRet);
			}
			if(m_bSave)
			{
				if(m_pVideoDataFile!=NULL&&m_pVideoDataFile->m_hFile!=CFile::hFileNull)
				{
					m_pVideoDataFile->Write(m_pReadBuff,lRet);
				}
			}
		}
		else if(m_bReview)
		{
			if(m_pVideoDataFile!=NULL&&m_pVideoDataFile->m_hFile!=CFile::hFileNull)
			{
				lRet=ReadDataBytes;
				lRet=m_pVideoDataFile->Read(m_pReadBuff,lRet);
				if(lRet>0)
				{
					m_lBytePerSecond+=lRet;
					m_pDataCapture->Input(m_pReadBuff,lRet);
				}
				else
				{
					m_bReview=FALSE;
					m_bSave=FALSE;
					AfxMessageBox(L"数据读取完毕！");
				}
				Sleep(18);//使回放时的fps和采集时的fps近似保持一致
			}
		}
		else//lRet<0;
		{
			/*failcnt++;
			Sleep(5);
			if(failcnt==2)OnBnClickedButton2();*/
		}
	}
}

void CUsbControlDlg::OnBnClickedBtnStopcapture()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bCapture=FALSE;
	m_pVideoDlg->ShowWindow(FALSE);
	m_bSave=FALSE;
	CloseDataFile();
}

void CUsbControlDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
	KillTimer(1);
	m_bCapture=FALSE;
	m_bCloseWnd=TRUE;
	m_pDataProcess->Close();
	Sleep(100);
	CloseRbfFile();
	CloseDataFile();
	if(m_pDataCapture!=NULL)
	{
		m_pDataCapture->Close();
		delete m_pDataCapture;
		m_pDataCapture=NULL;
	}

	if(m_pDataProcess!=NULL)
	{
		m_pDataProcess->Close();
		delete m_pDataProcess;
		m_pDataProcess=NULL;
	}

	if(m_pVideoDlg!=NULL)
	{
		delete m_pVideoDlg;
		m_pVideoDlg=NULL;
	}
	CloseUsb();
	CyUsb_Destroy();
	if(m_pBrush!=NULL)
	{
		for(int i=0;i<2;++i)
		{
			if(m_pBrush[i].m_hObject!=NULL)
			{
				m_pBrush[i].DeleteObject();
			}
		}
		delete[] m_pBrush;
		m_pBrush=NULL;
	}
}

void CUsbControlDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int iFrame=0;
	CString str;
	switch(nIDEvent)
	{
	case 1:
		{
			if(m_pDataProcess!=NULL)
			{
				m_pDataProcess->GetFrameCount(iFrame);
				str.Format(L"%d Fps     %0.4f MBs",iFrame,float(m_lBytePerSecond)/1024.0/1024.0);
				m_lBytePerSecond=0;
				if(m_pVideoDlg!=NULL)
				{
					m_pVideoDlg->SetWindowText(str);
				}
			}
		}
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CUsbControlDlg::OnBnClickedRadioDriver()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_bCapture||m_bSendData)
	{
		UpdateData(FALSE);
		AfxMessageBox(L"请先停止采集或者配置后再选择驱动类型！");
		return;
	}
	UpdateData(TRUE);
	m_CyDriver=(CYUSB_DRIVER)m_iRdoDriver;
	CyUsb_Init(m_CyDriver);
}

void CUsbControlDlg::OnBnClickedRadioProcType()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pDataProcess!=NULL)
	{
		UpdateData(TRUE);
		m_pDataProcess->SetProcType(DataProcessType(m_iProcType));
	}	
}

void CUsbControlDlg::OnBnClickedRadioChangeType()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pDataProcess!=NULL)
	{
		UpdateData(TRUE);
		m_pDataProcess->SetChangeType(RgbChangeType(m_iChangeType));
	}	
}

HBRUSH CUsbControlDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  在此更改 DC 的任何特性
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	int ID=pWnd->GetDlgCtrlID();
	if(ID==IDC_STATIC_TEXT)
	{
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
	}
	switch(nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_BTN:
		return m_pBrush[0];
	case CTLCOLOR_STATIC:
		return m_pBrush[0];
	default:
		return hbr;
	}
}

void CUsbControlDlg::OnBnClickedBtnSave()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!m_bCapture)
	{
		AfxMessageBox(L"请先进行视频采集！");
		return;
	}
	m_bSave=TRUE;
	m_bReview=FALSE;

	CFileDialog fDlg(FALSE);
	fDlg.m_ofn.lpstrFilter=L"VideoData Files(*.vdata)\0*.vdata\0\0";
	fDlg.m_ofn.lpstrDefExt=L".vdata";
	if(fDlg.DoModal()==IDOK)
	{
		m_strDataFileName=fDlg.GetPathName();
		OpenDataFile();
	}
}

void CUsbControlDlg::OnBnClickedBtnReview()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_bCapture)
		m_bCapture=FALSE;
	m_pVideoDlg->ShowWindow(TRUE);
	m_bReview=TRUE;
	m_bSave=FALSE;

	CFileDialog fDlg(TRUE);
	fDlg.m_ofn.lpstrFilter=L"VideoData Files(*.vdata)\0*.vdata\0\0";
	if(fDlg.DoModal()==IDOK)
	{
		m_strDataFileName=fDlg.GetPathName();
		OpenDataFile();
	}
	if (!m_Init)
	{
		m_pDataProcess->Open(m_hDisplayDC);
		m_pDataCapture->Open(m_pDataProcess);	
		m_hThread=(HANDLE)_beginthreadex(NULL,0,ThreadProcess,this,0,NULL);
		m_Init = TRUE;
	}
}

BOOL CUsbControlDlg::OpenDataFile()
{
	CloseDataFile();
	m_pVideoDataFile=new CFile();
	BOOL bRet=FALSE;
	if(m_bSave&&!m_bReview)
	{
		 bRet=m_pVideoDataFile->Open(m_strDataFileName,CFile::modeCreate|CFile::modeWrite);
	}
	else if(!m_bSave&&m_bReview)
	{
		 bRet=m_pVideoDataFile->Open(m_strDataFileName,CFile::modeRead);
	}
	return bRet;
}

BOOL CUsbControlDlg::CloseDataFile()
{
	if(m_pVideoDataFile!=NULL)
	{
		if(m_pVideoDataFile->m_hFile!=CFile::hFileNull)
		{
			m_pVideoDataFile->Close();
		}
		delete m_pVideoDataFile;
		m_pVideoDataFile=NULL;
	}
	return TRUE;
}

void CUsbControlDlg::OnBnClickedBtnCreatebmp()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pDataProcess!=NULL)
	{
		m_pDataProcess->CreateBmp();
	}
}


void CUsbControlDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	/*-------单个字符输入，弹出信息框*/
	/*
	CString s;
	s.GetBufferSetLength(1024);
	m_Edit_Width.GetWindowTextW(s.GetBuffer(),s.GetLength());
	MessageBox(s,_T("获取图像宽度"),MB_OK);
	s.ReleaseBuffer();
	*/	
	CString s_temp;
	UpdateData(true);
	s_temp = m_sEdit_Width.GetString();
	g_width = _tstoi(s_temp);
	s_temp.ReleaseBuffer();
}


void CUsbControlDlg::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString s_temp;
	UpdateData(true);
	s_temp = m_sEdit_Height.GetString();
	g_height = _tstoi(s_temp);
	s_temp.ReleaseBuffer();
}


void CUsbControlDlg::OnBnClickedButton2()
{
	m_sUsbOrder.DataBytes=1;
	m_sUsbOrder.ReqCode=0xa0;
	m_sUsbOrder.Direction=0;
	m_sUsbOrder.Value=0xe600;
	m_sUsbOrder.Index=0;
	m_byData[0]=1;
	SendOrder(&m_sUsbOrder);

	Sleep(1000);
	
	m_sUsbOrder.DataBytes=1;
	m_byData[0]=0;
	m_sUsbOrder.ReqCode=0xa0;
	m_sUsbOrder.Direction=0;
	m_sUsbOrder.Value=0xe600;
	m_sUsbOrder.Index=0;
	SendOrder(&m_sUsbOrder);
	
}


void CUsbControlDlg::OnBnClickedPipereset()
{
	Reset();
}


void CUsbControlDlg::OnBnClickedI2cread()
{
	if(!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_TEXT,L"请先打开USB端口");
		return;
	}
	CString strText;
	CString cstrOut[64];
	int cstrcnt;
	GetDlgItem(IDC_tbAddr)->GetWindowText(strText);
	int strlen=0;
	if(strText.GetLength()<=0) return;
	Split(strText,cstrOut,cstrcnt,L" ",63);
	unsigned int n[64];
	const char* testcharp=(char*)cstrOut[0].GetBuffer();
	wchar_t *end=NULL;
	if(cstrcnt>63)cstrcnt=63;//64 max
	for(int i=0;i<cstrcnt+1;i++)
	{
		//sscanf(testcharp,"%d",&n[i]);
		n[i]=wcstol(cstrOut[i],&end,16);
		m_byData[i]=(UINT8)n[i];
	}
	m_sUsbOrder.DataBytes=cstrcnt+1;
	m_sUsbOrder.ReqCode=0xD3;
	m_sUsbOrder.Direction=0;//out
	SendOrder(&m_sUsbOrder);
	//int li1;
	//li1=_tstoi(strText);
}
void CUsbControlDlg::Split(CString in, CString *out, int &outcnt, CString spliter,int maxn)
{

	int d_len=spliter.GetLength();
	int j=0;
	int n=0;
	int m_pos;
	while(1)
	{
		m_pos= in.Find(spliter,j);
		if(m_pos==-1 && j==0)
		{
			out[0]=in.Mid(0);
			outcnt=0;//-1
			break;
		}
		if((m_pos==-1 && j!=0)||(n==maxn))
		{
			out[n]=in.Mid(j,in.GetLength()-j);
			outcnt=n;
			break;
		}

		if(j==0)
		{
			out[n]=in.Mid(0,m_pos);
			j=m_pos+d_len;
		}
		else
		{
			out[n]=in.Mid(j,m_pos-j);
			j=m_pos+d_len;
		}
		n++;
	}
}