
// ANPRDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ANPR.h"
#include "ANPRDlg.h"
#include "afxdialogex.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
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


// CANPRDlg dialog



CANPRDlg::CANPRDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ANPR_DIALOG, pParent)
	, m_ImageFolderName(_T(""))
	, m_sampleWidth(0)
	, m_sampleHeight(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CANPRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ImageFolderName);
	DDX_Control(pDX, IDC_IMAGE_VIEW, m_ImaveView);
	DDX_Text(pDX, IDC_SAMPLE_WIDTH, m_sampleWidth);
	DDX_Text(pDX, IDC_SAMPLE_HEIGHT, m_sampleHeight);
	DDX_Control(pDX, IDC_LISTCTRL__IMAGE, m_ctrlImageList);
}

BEGIN_MESSAGE_MAP(CANPRDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_IMAGE_BROWSE, &CANPRDlg::OnBnClickedImageBrowse)
	ON_BN_CLICKED(ID_CLOSE, &CANPRDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDC_SAMPLING, &CANPRDlg::OnClickedSampling)
	ON_BN_CLICKED(IDC_CONVERT_BMP2JPG, &CANPRDlg::OnBnClickedConvertBmp2jpg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_REC_START, &CANPRDlg::OnBnClickedRecStart)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTCTRL__IMAGE, &CANPRDlg::OnItemchangedListctrlImage)
	ON_BN_CLICKED(IDC_INVERSE_COLOR, &CANPRDlg::OnBnClickedInverseColor)
	ON_BN_CLICKED(IDC_SINGLE_RECOG, &CANPRDlg::OnBnClickedProcess)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CANPRDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_THRESHOLD, &CANPRDlg::OnBnClickedBtnThreshold)
	ON_BN_CLICKED(IDC_BTN_FILTER, &CANPRDlg::OnBnClickedBtnFilter)
END_MESSAGE_MAP()


// CANPRDlg message handlers

BOOL CANPRDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	Init();
	InitControl();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CANPRDlg::Init()
{
	m_zoomRate = 1.0;
	m_nTotalProcTime = 0;
	m_fileName.Empty();
	m_ImageFolderName.Empty();
	m_nFileIndex = 0;
	m_bRunFlag = m_bRecFlag = FALSE;
	m_imgWidth = m_imgHeight = 0;
	m_image = NULL;
	m_undoImg = NULL;
	m_bmi = (BITMAPINFO*)m_buffer;
	m_bmih = &(m_bmi->bmiHeader);
	memset(m_bmih, 0, sizeof(*m_bmih));
	m_bmih->biSize = sizeof(BITMAPINFOHEADER);	

	((CButton*)GetDlgItem(IDC_SINGLE_RECOG))->EnableWindow(TRUE);
}

void CANPRDlg::InitControl()
{

	m_ctrlImageList.InsertColumn(0, _T("No"), LVCFMT_CENTER, 45);
	m_ctrlImageList.InsertColumn(1, _T("Name"), LVCFMT_CENTER, 100);
	m_ctrlImageList.InsertColumn(2, _T("Time"), LVCFMT_CENTER, 50);
	m_ctrlImageList.InsertColumn(3, _T("   Result"), LVCFMT_LEFT, 200);
	m_ctrlImageList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	((CStatic*)GetDlgItem(IDC_STATIC_IMAGE_COUNT))->SetWindowText(_T(""));
	((CStatic*)GetDlgItem(IDC_STATIC_PROC_SPEED))->SetWindowText(_T(""));
	((CStatic*)GetDlgItem(IDC_STATIC_PROC_TIME))->SetWindowText(_T(""));
	((CStatic*)GetDlgItem(IDC_STATIC_PROC_NUMBER))->SetWindowText(_T(""));
	UpdateData(FALSE);
}

void CANPRDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CANPRDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (IsIconic())
	{		
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if (!m_image.empty())
		{
			CRect rect;
			m_ImaveView.GetWindowRect(&rect);
			CClientDC dc1(&m_ImaveView);
			Mat drawMat = m_image.clone();
			m_bmih->biWidth = drawMat.cols;
			m_bmih->biHeight = -drawMat.rows;
			m_bmih->biPlanes = 1;
			m_bmih->biCompression = BI_RGB;//BI_RGB;
			m_bmih->biBitCount = 8 * drawMat.channels();

			dc1.SetStretchBltMode(COLORONCOLOR);

			if (m_imgWidth*rect.Height() > m_imgHeight*rect.Width())
				m_zoomRate = ((float)rect.Width() / m_imgWidth);
			else
				m_zoomRate = ((float)rect.Height() / m_imgHeight);

			int viewX, viewY, viewWid, viewHei;
			viewX = (rect.Width() - m_imgWidth*m_zoomRate) / 2;
			viewY = (rect.Height() - m_imgHeight*m_zoomRate) / 2;
			viewWid = m_imgWidth*m_zoomRate;
			viewHei = m_imgHeight*m_zoomRate;
			::StretchDIBits(
				dc1.GetSafeHdc(),
				//				0, 0, drawMat.cols, drawMat.rows,
				viewX, viewY, viewWid, viewHei,
				0, 0, m_imgWidth, m_imgHeight,
				drawMat.data,
				(BITMAPINFO*)m_bmi,
				DIB_RGB_COLORS,
				SRCCOPY);

			//////////////////////////////////////////////////////////////////////////
		}
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CANPRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CANPRDlg::ReadMatImage(CString& fileName)
{
	CT2CA pszConvertedAnsiString(fileName);
	string f_name(pszConvertedAnsiString);
	m_image = imread(f_name);
	if (m_image.empty())
		FALSE;
	int newCols = (int)(m_image.cols / 4) * 4;

	resize(m_image, m_image, Size(newCols, m_image.rows), 0, 0, CV_INTER_LINEAR);
	m_imgWidth = m_image.cols;
	m_imgHeight = m_image.rows;
	
	return TRUE;
}

BOOL CANPRDlg::ReadMatImageNew(CString& fileName)
{	
	
	CFile w_file;
	w_file.Open(fileName, CFile::modeRead);
	long w_sz = w_file.GetLength();
	Mat w_img(w_sz, 1, CV_8UC1);
	w_file.Read(w_img.data, w_sz);
	w_file.Close();

	m_image = imdecode(w_img, CV_LOAD_IMAGE_COLOR);

	if (m_image.empty())
		FALSE;
	int newCols = (int)(m_image.cols / 4) * 4;

	resize(m_image, m_image, Size(newCols, m_image.rows), 0, 0, CV_INTER_LINEAR);
	m_imgWidth = m_image.cols;
	m_imgHeight = m_image.rows;

	return TRUE;
}

BOOL CANPRDlg::SaveMatImage(CString& fileName, Mat& img)
{
	if (img.empty())
		return FALSE;

	CT2CA pszConvertedAnsiString(fileName);
	string f_name(pszConvertedAnsiString);

	return imwrite(f_name, img);
}

void CANPRDlg::OnBnClickedImageBrowse()
{
	// TODO: Add your control notification handler code here
	Init();

	CFolderPickerDialog dlg;
	if (dlg.DoModal() != IDOK)
	{
		return;
	}
	m_image.release();
	m_ImageFolderName = dlg.GetPathName();
	UpdateData(FALSE);
	CollectFiles(m_ImageFolderName, m_vecFileArray);
	if (m_vecFileArray.size() > 0)
	{
		ListSelImageView(0);
		m_sampleWidth = m_imgWidth;
		m_sampleHeight = m_imgHeight;
		UpdateData(FALSE);
	}
	CString strCnt;
	strCnt.Format(_T("%d"), m_vecFileArray.size());
	((CStatic*)GetDlgItem(IDC_STATIC_IMAGE_COUNT))->SetWindowText(strCnt);
	((CStatic*)GetDlgItem(IDC_STATIC_PROC_SPEED))->SetWindowText(_T(""));
	((CStatic*)GetDlgItem(IDC_STATIC_PROC_TIME))->SetWindowText(_T(""));
	((CStatic*)GetDlgItem(IDC_STATIC_PROC_NUMBER))->SetWindowText(_T(""));
}

void CANPRDlg::CollectFiles(CString strDir, vector<CString> &vecArray)
{
	vecArray.clear();
	m_ctrlImageList.DeleteAllItems();

	CString strDefaultPath;
	strDefaultPath = strDir;

	CString strIndexFind = strDefaultPath + _T("\\*.*");
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strIndexFind);

	CString strFileName, strOnlyName, strExtension;

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		strFileName = finder.GetFileName();

		if(finder.IsDots())
			continue;
		if(finder.IsDirectory())
			continue;

		strExtension = strFileName.Right(3);
		//		strOnlyName = FindOnlyName(strFileName);
		strExtension.MakeLower();

		if (strExtension == _T("jpg") || strExtension == _T("bmp") || strExtension == _T("png"))
		{
			
			CString strImageFileName = /*strDefaultPath + _T("\\") + */finder.GetFileName();
			vecArray.push_back(strImageFileName);

			CString strNo;	
			m_nFileIndex = vecArray.size()-1;
			strNo.Format(_T("%d"), m_nFileIndex+1);
			int nId = m_ctrlImageList.InsertItem(m_nFileIndex, strNo);
			m_ctrlImageList.SetItemText(nId, 1, strImageFileName);
			
		}
	}
}

void CANPRDlg::OnBnClickedClose()
{
	// TODO: Add your control notification handler code here

	CDialogEx::OnOK();
}



void CANPRDlg::ListSelImageView(int curSel)
{
	if (curSel < 0)
		return;
	
	m_fileName = m_vecFileArray[curSel];
	CString fileFullPath = m_ImageFolderName + _T("\\");
	fileFullPath += m_fileName;
	ReadMatImage(fileFullPath);
	Invalidate();
}


void CANPRDlg::OnClickedSampling()
{
	// TODO: Add your control notification handler code here
}


void CANPRDlg::CudaProcessImage(BOOL bUpdate)
{	
	if (m_image.empty())
		return;
	DWORD tm = GetTickCount();

	int mem_size = 3 * m_imgWidth*m_imgHeight;
	m_ImgData = (BYTE*)malloc(mem_size);
	memcpy(m_ImgData, m_image.data, mem_size);


	//m_engine.RM_Recognition(m_ImgData, m_imgWidth, m_imgHeight, 24, &m_iniSet, &m_carplateData);
	
	tm = GetTickCount() - tm;
	m_nTotalProcTime += (UINT)tm;
	CString tmtmp;
	tmtmp.Format(_T("%d ms"), tm);
	m_ctrlImageList.SetItemText(m_nFileIndex, 2, tmtmp);

	tmtmp.Format(_T("%.1f"), (float)(m_nFileIndex + 1)*1000/m_nTotalProcTime);
	((CStatic*)GetDlgItem(IDC_STATIC_PROC_SPEED))->SetWindowText(tmtmp);

	tmtmp.Format(_T("%d ms"), m_nTotalProcTime);
	((CStatic*)GetDlgItem(IDC_STATIC_PROC_TIME))->SetWindowText(tmtmp);

	tmtmp.Format(_T("%d"), m_nFileIndex + 1);
	((CStatic*)GetDlgItem(IDC_STATIC_PROC_NUMBER))->SetWindowText(tmtmp);
}

void CANPRDlg::CudaProcessImage(CString strImageFile)
{
	if(!ReadMatImageNew(strImageFile))
		return;
	DWORD tm = GetTickCount();
	int mem_size = 3 * m_imgWidth*m_imgHeight;
	m_ImgData = (BYTE*)malloc(mem_size);
	memcpy(m_ImgData, m_image.data, mem_size);


	//m_engine.RM_Recognition(m_ImgData, m_imgWidth, m_imgHeight, 24, &m_iniSet, &m_carplateData);

	free(m_ImgData);
	tm = GetTickCount() - tm;
	CString tmtmp;
	tmtmp.Format(_T("%d ms"), tm);
	m_ctrlImageList.SetItemText(m_nFileIndex, 2, tmtmp);
	Invalidate();
}

void CANPRDlg::OnBnClickedConvertBmp2jpg()
{
	// TODO: Add your control notification handler code here

	CString temp;

	temp = m_ImageFolderName;
	CString fileFullPath;

	int w_i = 0;
	for (w_i = 0; w_i < m_vecFileArray.size(); w_i++)
	{
		fileFullPath = temp + _T("\\");
		fileFullPath += m_vecFileArray[w_i];

		ReadMatImageNew(fileFullPath);
		if (m_image.empty()) continue;

		if (fileFullPath.Right(3) == _T("jpg"))
			fileFullPath = fileFullPath.Left(fileFullPath.GetLength() - 3) + _T("bmp");
		else if(fileFullPath.Right(3) == _T("bmp"))
			fileFullPath = fileFullPath.Left(fileFullPath.GetLength() - 3) + _T("jpg");
		else 
			continue;
		//SaveMatImage(fileFullPath, m_image);
		SaveImgToFile(fileFullPath, m_image.data, m_imgWidth, m_imgHeight, 8 * m_image.channels());
	}
	MessageBox(_T("process finished!"), _T("Info"), MB_OK);
}



void CANPRDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CDialogEx::OnTimer(nIDEvent);
}

BOOL CANPRDlg::IsTimeLimitValid()
{
	
	return TRUE;
}

void CANPRDlg::OnBnClickedRecStart()
{
	// TODO: Add your control notification handler code here
	if (!IsTimeLimitValid())
		return;

	if (m_vecFileArray.size() < 1)
	{
		MessageBox(_T(" Select folder! \n\n Please click a button <Browse>."), _T("! ANPR MSG"), MB_OK);
		return;
	}
	((CButton*)GetDlgItem(IDC_SINGLE_RECOG))->EnableWindow(FALSE);

	m_bRecFlag = TRUE;
	m_nFileIndex = -1;
	if (!m_bRunFlag)
	{
		m_nTotalProcTime = 0;
		m_ctrlImageList.DeleteAllItems();
		((CButton*)GetDlgItem(IDC_REC_START))->SetWindowText(_T("Stop"));
		SetTimer(0, 10, NULL);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_REC_START))->SetWindowText(_T("All Frame Recog"));
		KillTimer(0);		
	}
	m_bRunFlag = !m_bRunFlag;
}


void CANPRDlg::OnItemchangedListctrlImage(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_nFileIndex = pNMLV->iItem; //m_ctrlImageList.GetHotItem();
	if (m_nFileIndex < 0)
		return;
	ListSelImageView(m_nFileIndex);
	m_sampleWidth = m_imgWidth;
	m_sampleHeight = m_imgHeight;
	UpdateData(FALSE);
	*pResult = 0;
}




void CANPRDlg::OnBnClickedInverseColor()
{
	// TODO: Add your control notification handler code here
	if (m_image.empty())
		return;
	Mat temp = m_image.clone();
	
	int memsize = temp.rows*temp.cols * temp.channels();
	for (int i = 0; i < memsize; i++)
		temp.data[i] = 255 - temp.data[i];
	
	m_image = temp.clone();
	Invalidate();
}

BOOL CANPRDlg::SaveBmpFile(LPCTSTR lpszPathName, BYTE* pDib)
{
	LPBITMAPINFOHEADER lpBIH = (LPBITMAPINFOHEADER)pDib;
	int FileSize, ImgSize;
	int HeadSize;
	int QuadSize;
	BITMAPFILEHEADER FilehHeader;
	ImgSize = lpBIH->biSizeImage;//GetBmpSize(w,h,lpBIH->biBitCount);
	if (lpBIH->biBitCount == 1)		QuadSize = sizeof(RGBQUAD) * 2;
	else if (lpBIH->biBitCount == 2)	QuadSize = sizeof(RGBQUAD) * 4;
	else if (lpBIH->biBitCount == 4)	QuadSize = sizeof(RGBQUAD) * 16;
	else if (lpBIH->biBitCount == 8)	QuadSize = sizeof(RGBQUAD) * 256;
	else							QuadSize = 0;//24
	HeadSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + QuadSize;
	FileSize = HeadSize + ImgSize;
	int DibSize = sizeof(BITMAPINFOHEADER) + QuadSize + ImgSize;

	FilehHeader.bfType = 0x4d42;     //unsigned short    bfType;
	FilehHeader.bfSize = FileSize;   //unsigned int	     bfSize;
	FilehHeader.bfReserved1 = 0;     //unsigned short    bfReserved1;
	FilehHeader.bfReserved2 = 0;     //unsigned short    bfReserved2;
	FilehHeader.bfOffBits = HeadSize;//unsigned int      bfOffBits;

	FILE* file = _tfopen(lpszPathName, _T("wb"));
	if (file == NULL)
		return FALSE;

	fwrite(&FilehHeader, sizeof(BITMAPFILEHEADER), 1, file);
	fwrite(pDib, DibSize, 1, file);
	fclose(file);

	return TRUE;

}

BOOL CANPRDlg::SaveImgToFile(LPCTSTR lpszPathName, BYTE* pImg, int w, int h, int nBitCount/*=1,8,24*/)
{
// #ifndef _DEBUG
// 	return FALSE;
// #endif
	int i, j;
	if (pImg == NULL) return FALSE;
	//	if (nBitCount != 8 && nBitCount != 1) return FALSE;

	int widByte = (nBitCount*w + 31) / 32 * 4;
	int ImgSize = widByte * h;
	int nColror;
	if (nBitCount == 24)
		nColror = 0;
	else if (nBitCount == 8)
		nColror = 256;
	else
		nColror = 2;
	int HeadSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*nColror;

	int DibSize = HeadSize + ImgSize;
	BYTE *pDib = (BYTE*)malloc(DibSize);

	//Create InfoHeader
	BITMAPINFOHEADER* pBIH = (BITMAPINFOHEADER*)pDib;
	pBIH->biSize = sizeof(BITMAPINFOHEADER);
	pBIH->biWidth = w;
	pBIH->biHeight = h;
	pBIH->biPlanes = 1;
	pBIH->biBitCount = (WORD)nBitCount;
	pBIH->biCompression = 0;
	pBIH->biSizeImage = ImgSize;
	pBIH->biXPelsPerMeter = 0;
	pBIH->biYPelsPerMeter = 0;
	pBIH->biClrUsed = nColror;
	pBIH->biClrImportant = 0;

	//Create Palette
	if (nColror > 0) {
		BITMAPINFO* pInfoH = (BITMAPINFO*)(LPSTR)pBIH;
		for (i = 0; i < nColror - 1; i++)
		{
			pInfoH->bmiColors[i].rgbRed = BYTE(i);
			pInfoH->bmiColors[i].rgbGreen = BYTE(i);
			pInfoH->bmiColors[i].rgbBlue = BYTE(i);
			pInfoH->bmiColors[i].rgbReserved = 0;
		}
		pInfoH->bmiColors[nColror - 1].rgbRed = 255;
		pInfoH->bmiColors[nColror - 1].rgbGreen = 255;
		pInfoH->bmiColors[nColror - 1].rgbBlue = 255;
		pInfoH->bmiColors[nColror - 1].rgbReserved = 0;
	}

	//Copy Image
	BYTE *lpBits = pDib + HeadSize;
	if (nBitCount == 24) {
		int nInCnt = 0;
		int nOutCnt = 0;
		for (i = 0; i < h; i++) {
			for (j = 0; j < w; j++) {
				nInCnt = (widByte*(h - i - 1) + j * 3);
				nOutCnt = (i*w + j) * 3;
				lpBits[nInCnt] = pImg[nOutCnt];
				lpBits[nInCnt + 1] = pImg[nOutCnt + 1];
				lpBits[nInCnt + 2] = pImg[nOutCnt + 2];
			}
		}
	}
	else if (nBitCount == 8)
	{
		for (i = 0; i < h; i++) for (j = 0; j < w; j++)
			lpBits[widByte*(h - i - 1) + j] = pImg[i*w + j];
	}
	else
	{
		BYTE reg = 0;
		int rem = w % 8;
		int i1 = 0;
		for (i = h - 1; i >= 0; i--) {
			int j1 = 0;
			for (j = 0; j < w; j++) {
				if (pImg[i*w + j] != 0)	reg |= 0;
				else					reg |= 1;
				if (((j + 1) % 8) == 0) {
					lpBits[i1*widByte + j1] = reg;
					j1++;
					reg = 0;
				}
				else { reg <<= 1; }
			}
			if (rem != 0) {
				reg <<= 8 - rem - 1;
				lpBits[i1*widByte + j1] = reg;
			}
			reg = 0;
			i1++;
		}
	}
	BOOL bRet = SaveBmpFile(lpszPathName, pDib);
	free(pDib);
	return TRUE;

}


BOOL CANPRDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case 'I':
		case 'i':
			if (::GetKeyState(VK_CONTROL) < 0)
				OnBnClickedInverseColor();
			break;
		default:;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CANPRDlg::OnBnClickedProcess()
{
	// TODO: Add your control notification handler code here
}




void CANPRDlg::OnBnClickedBtnSave()
{
	// TODO: Add your control notification handler code here
	if (m_image.empty())
		return;

	CString filename;
	CFileDialog dlg(FALSE);

	dlg.m_ofn.lpstrFilter = _T("Jpeg Files (*.jpg)\0*.jpg\0Bitmap Files (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0\0");
	dlg.m_ofn.lpstrTitle = _T("Save Image File As");

	if (dlg.DoModal() == IDOK)
	{
		filename = dlg.GetPathName(); // return full path and filename
									  //MessageBox(filename);
		SaveMatImage(filename, m_image);
	}
}


