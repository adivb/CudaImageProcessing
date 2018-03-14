// RecogEngine.h: interface for the CRecogEngine class.
//
//////////////////////////////////////////////////////////////////////
	
//recognition width range
#pragma  once

#include "Process_CUDA.h"

#define IMAGE_WIDTH			3000
#define IMAGE_HEIGHT		2000




class CProcessEngine
{
public:
	CProcessEngine();
	virtual ~CProcessEngine();
public:
	cudaProcess m_cudaProcess;
		
	BYTE	*m_pbRGBorg ;
	BYTE	*m_pbRGB;
	BYTE	*m_pbGray;
	BYTE    *m_pbGrayOrg;

	BYTE	*m_pbTemp;

	int		m_nWidth,	m_nHeight;
	int		m_nWidthorg,m_nHeightorg;
	
	BOOL AllocMem(int nWidth, int nHeight, int nBitCount);
	BOOL initMem(BYTE* pbImage, int* nWidth, int* nHeight, int nBitCount);
	void FreeMem();

	void test();
	void RGBToGrayNoMask(BYTE *pbRGB, BYTE *pbGray, int nWidth, int nHeight);
	void CalcEdgeHist(int *pnEdge, int nWidth, int nHeight, int *pnHistP, int *pnHistN, int nMax);
	int	 CalcEdgeThres(int *pnHist, int nLen);
	void BinaryByEdge(BYTE *pbGray, BYTE *pbEdge, BYTE *pbBin, int nWidth, int nHeight);
};
