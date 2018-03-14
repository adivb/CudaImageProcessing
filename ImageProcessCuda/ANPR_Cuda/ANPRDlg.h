
// ANPRDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "use_opencv.h"
#include "..\Engine\ProcessEngine.h"
#include "afxcmn.h"
using namespace std;
using namespace cv;


// CANPRDlg dialog
class CANPRDlg : public CDialogEx
{
// Construction
public:
	CANPRDlg(CWnd* pParent = NULL);	// standard constructor

	int m_imgWidth, m_imgHeight;

	float m_zoomRate;

	BYTE*  m_ImgData;
	BITMAPINFO*               m_bmi;
	BITMAPINFOHEADER*         m_bmih;
	unsigned int              m_buffer[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
		
	CString m_fileName;
	CString consTm;
	int m_nFileIndex;
	UINT m_nTotalProcTime;
	BOOL m_bRunFlag, m_bRecFlag;
	vector<CString> m_vecFileArray;

	Mat m_image;
	Mat m_undoImg;
	Mat m_grayImg;

	CProcessEngine m_engine;

	BOOL IsTimeLimitValid();	
	void Init();
	void InitControl();
	BOOL ReadMatImage(CString& fileName);
	BOOL ReadMatImageNew(CString& fileName);
	BOOL SaveBmpFile(LPCTSTR lpszPathName, BYTE* pDib);
	BOOL SaveImgToFile(LPCTSTR lpszPathName, BYTE* pImg, int w, int h, int nBitCount/*=1,8,24*/);
	BOOL SaveMatImage(CString& fileName, Mat& img);
	void CollectFiles(CString strDir, vector<CString> &vecArray);
	void ListSelImageView(int curSel);
	void CudaProcessImage(BOOL bUpdate);
	void CudaProcessImage(CString strImageFile);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANPR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_ImageFolderName;
	CStatic m_ImaveView;
	UINT m_sampleWidth;
	UINT m_sampleHeight;
	CListCtrl m_ctrlImageList;
	afx_msg void OnBnClickedImageBrowse();
	afx_msg void OnBnClickedClose();
	afx_msg void OnClickedSampling();
	afx_msg void OnBnClickedConvertBmp2jpg();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedRecStart();
	afx_msg void OnItemchangedListctrlImage(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedInverseColor();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedProcess();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnThreshold();
	afx_msg void OnBnClickedBtnFilter();
};
