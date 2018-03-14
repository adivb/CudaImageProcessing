//////////////////////////////////////////////////////////////////////
//																	//
// RecogEngine.cpp: implementation of the CProcessEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ProcessEngine.h"
#include <time.h>

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/



#define		PI			3.14159265358979f


#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

inline int GetRectWidth( RECT& x)					{ 	return (x.right-x.left+1) /*/ 2 * 2*/; }
inline int GetRectHeight(RECT& x)					{ 	return x.bottom-x.top+1; }
inline int CalcIntersectRectHeight(RECT& x, RECT& y) {	return (min(x.bottom, y.bottom)-max(x.top,  y.top));}
inline int CalcIntersectRectWidth (RECT& x, RECT& y)	{	return (min(x.right,  y.right) -max(x.left, y.left));}
inline int round(float a)							{	return (a>0.0f) ? (int)(a+0.5f) : (int)(a-0.5f);}
inline int round(int a, int b)						{	return (a> 0)	? (a+b/2)		: (a-b/2);}
template<typename T>
inline T limit(const T* value)
{
	return ( ((*value) > 255) ? 255 : (((*value) < 0) ? 0 : (*value)) );
}

inline  int  Round( double value )
{
	return (int)(value + (value >= 0 ? 0.5 : -0.5));
}
RECT GetIntersectRect(RECT& rt1,RECT& rt2)
{
    RECT rt;
    rt.left = max(rt1.left,rt2.left);
    rt.right = min(rt1.right,rt2.right);
    rt.top = max(rt1.top,rt2.top);
    rt.bottom = min(rt1.bottom,rt2.bottom);
    if(rt.left > rt.right)
    {
        rt.left = rt.right;
    }
    if(rt.top > rt.bottom)
    {
        rt.top = rt.bottom;
    }
    return rt;
}
BOOL IsRectEmpty(RECT& rt)
{
    if(rt.right <= rt.left) return TRUE;
    if(rt.bottom <= rt.top) return TRUE;
    return false;
}


void GetStatisticValue(int *buf,int n,  int* Min,int* Max,float* Ave,float* Dev)
{
    if(n <=0) return;
    if(n==1){
        *Ave = buf[0];*Min = buf[0];*Max = buf[0];*Dev = 0; return;
    }
    int i;
    int d =0,dd=0;
	int imax=buf[0];
	int imin=buf[0];
	for(i=1;i<n;++i){
		if(buf[i]>imax)imax=buf[i];
		if(buf[i]<imin)imin=buf[i];
	}
    *Max = imax;
    *Min = imin;
    for(i=0;i<n;++i)
	{
		d += buf[i];
		dd += buf[i]*buf[i];
	}
    *Ave = (float)d / n;
    float b = (float)(n*dd-d*d)/(n*(n-1));
    *Dev = sqrt(b);
}

BOOL SaveBmpFile(LPCTSTR lpszPathName, BYTE* pDib)
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

BOOL SaveImgToFile(LPCTSTR lpszPathName, BYTE* pImg, int w, int h,int nBitCount/*=1,8,24*/)
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
				lpBits[nInCnt+1] = pImg[nOutCnt+1];
				lpBits[nInCnt+2] = pImg[nOutCnt+2];
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

BOOL IsIncludeRect(RECT& x, RECT& y)
{
	if(x.left >= y.left && x.right <= y.right && x.top >= y.top && x.bottom <= y.bottom)	return TRUE;
	else																					return FALSE;
}

RECT GetRect(int x0, int y0, int x1, int y1)
{	RECT ret;
	ret.left	= max(0, x0);	ret.right	= max(ret.left, x1);
	ret.top		= max(0, y0);	ret.bottom	= max(ret.top,  y1);
	return ret;
}
BOOL IsOverRects(RECT& r1, RECT& r2)
{
	if(r1.left > r2.right  || r2.left > r1.right)	return TRUE;
	else											return FALSE;
}
RECT GetUnionRect(RECT& rt1,RECT& rt2)
{
    RECT rt;
    if(IsRectEmpty(rt1)) rt = rt2;
    else if(IsRectEmpty(rt2)) rt = rt1;
    else
    {
        rt.left = min(rt1.left,rt2.left);
        rt.right = max(rt1.right,rt2.right);
        rt.top = min(rt1.top,rt2.top);
        rt.bottom = max(rt1.bottom,rt2.bottom);
    }
    
    return rt;
}
RECT GetIncRect(RECT rt, int x0, int y0, int x1, int y1)
{
	RECT ret = GetRect(rt.left+x0, rt.top+y0, rt.right+x1, rt.bottom+y1);
	rt.left	= max(0, rt.left);	rt.right	= max(rt.left, rt.right);
	rt.top	= max(0, rt.top);	rt.bottom	= max(rt.top,  rt.bottom);
	return ret;
}
POINT GetCenterPoint(RECT& rt)
{
    POINT x;
    x.x = (rt.right + rt.left)/2;
    x.y = (rt.top + rt.bottom) / 2;
    return x;
}
RECT GetMultiRect(RECT rt, int x0, int y0, int x1, int y1)
{
	RECT ret = GetRect(rt.left*x0, rt.top*y0, rt.right*x1, rt.bottom*y1);
	rt.left	= max(0, rt.left);	rt.right	= max(rt.left, rt.right);
	rt.top	= max(0, rt.top);	rt.bottom	= max(rt.top,  rt.bottom);
	return ret;
}
RECT GetReSkewRect(RECT rt, float offy,int ww)
{
	RECT ret = rt;
	if(offy==0.0f) return ret; 
	int l = rt.left;
	int r = rt.right;
	int offyL = (int)((float)(offy*l/ww) - offy/2);
	int offyR = (int)((float)(offy*r/ww) - offy/2);
	int t = rt.top;
	ret.top = min(t+offyL,t+offyR);
	int b = rt.bottom;
	ret.bottom = max(b+offyL,b+offyR);
	return ret;
}
RECT BoundRect(RECT& rt, int nWidth, int nHeight)
{
	RECT ret = rt;
	int t;
	ret.left	= max(0, min(nWidth-1,	ret.left));		ret.right	= max(0, min(nWidth-1,	ret.right));
	ret.top		= max(0, min(nHeight-1,	ret.top));		ret.bottom	= max(0, min(nHeight-1,	ret.bottom));

	if(ret.left > ret.right) {t = ret.left; ret.left = ret.right; ret.right = t;}
	if(ret.top > ret.bottom) {t = ret.top; ret.top = ret.bottom; ret.bottom = t;}
	return ret;
}

inline BOOL IsSameRect(RECT& rt0, RECT& rt1)
{
	if(rt0.left == rt1.left && rt0.top == rt1.top && rt0.right == rt1.right && rt0.bottom == rt1.bottom) return TRUE;
	else return false;
}

#define imRef_Gray(buff, y, x, width)			buff[(y) * (width) + (x)]

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessEngine::CProcessEngine(void)
{
	m_pbRGBorg = NULL;
	m_pbRGB = NULL;
	m_pbGray = NULL;
	m_pbGrayOrg = NULL;
	m_pbTemp = NULL;

	AllocMem(IMAGE_WIDTH, IMAGE_HEIGHT, 24);
}

BOOL CProcessEngine::AllocMem(int nWidth, int nHeight, int nBitCount)
{
	if (nBitCount != 8 && nBitCount != 24) return false;

	int  nW, nH;
	nW = nWidth;
	nH = nHeight;

	m_pbTemp = (BYTE*)malloc(nW*nH);
	m_pbGrayOrg = (BYTE*)malloc(nW * nH);

	return TRUE;
}

CProcessEngine::~CProcessEngine(void)
{
	FreeMem();
}


BOOL CProcessEngine::initMem(BYTE* pbImage, int* nWidth, int* nHeight, int nBitCount)
{
	if (nBitCount != 8 && nBitCount != 24) return false;

	int  nW, nH;

	nW = *nWidth;
	nH = *nHeight;
	if (nBitCount == 24)
	{
		m_pbRGB = pbImage;
		m_pbGray = m_pbGrayOrg;
	}
	else if (nBitCount == 8)
	{
		m_pbRGB = NULL;
		m_pbGray = pbImage;
	}

	*nWidth = m_nWidth = nW;
	*nHeight = m_nHeight = nH;

	return TRUE;
}



void CProcessEngine::FreeMem()
{
	if(m_pbTemp)	free(m_pbTemp);		m_pbTemp	= NULL;
	if (m_pbGrayOrg) free(m_pbGrayOrg); m_pbGrayOrg = NULL;

}

void CProcessEngine::RGBToGrayNoMask(BYTE *pbRGB, BYTE *pbGray, int nWidth, int nHeight)
{
	int i, nSize = nWidth * nHeight,buf;
	for(i = 0; i < nSize; i++)
	{
		pbGray[i] = (((int)pbRGB[i*3]*117+(int)pbRGB[i*3+1]*601+(int)pbRGB[i*3+2]*306) >> 10);
	}
}

void CProcessEngine::BinaryByEdge(BYTE *pbGray, BYTE *pbEdge, BYTE *pbBin, int nWidth, int nHeight)
{ 
	RECT rtRegion = {0, 0, nWidth-1, nHeight-1};
	int nX, nY, nX2, nY2;
	int nSumH, nSumL, nNumH, nNumL, nThres;
	int nWidth2 = rtRegion.right-rtRegion.left+1;
	int nHeight2 = rtRegion.bottom-rtRegion.top+1;
	int point_pos, point_pos1;

	nSumH = 0; nSumL = 0; nNumH = 0; nNumL = 0;
	for(nY = rtRegion.top, nY2 = 0; nY <= rtRegion.bottom; nY++, nY2++)
	{
		point_pos=nY*nWidth;
		point_pos1=nY2*nWidth2;
		for(nX = rtRegion.left, nX2 = 0; nX <= rtRegion.right; nX++, nX2++)
		{
			if(!pbEdge[point_pos1+nX2])			
			{
				nSumL += pbGray[point_pos+nX];
				nNumL++;
			}
			else if(pbEdge[point_pos1+nX2] == 255)
			{
				nSumH += pbGray[point_pos+nX];
				nNumH++;
			}
		}
	}
	if(!nNumL || !nNumH)
	{
		memset(pbBin, 0, nWidth2*nHeight2);
		return;
	}
	
	nThres = (nSumH+nSumL)/(nNumH+nNumL);
	for(nY = rtRegion.top, nY2 = 0; nY <= rtRegion.bottom; nY++, nY2++)
	{
		point_pos=nY*nWidth;
		point_pos1=nY2*nWidth2;
		for(nX = rtRegion.left, nX2 = 0; nX <= rtRegion.right; nX++, nX2++)
		{
			if(pbGray[point_pos+nX] > nThres)	pbBin[point_pos1+nX2] = 255;
			else								pbBin[point_pos1+nX2] = 0;
			//if(pbEdge[point_pos1+nX2] == 255) pbBin[point_pos1+nX2] = 255;
		}
	}
}


void CProcessEngine::CalcEdgeHist(int *pnEdge, int nWidth, int nHeight, int *pnHistP, int *pnHistN, int nMax)
{
	int y, x, nEdge, point_pos;
	memset(pnHistP, 0, nMax*sizeof(int));
	memset(pnHistN, 0, nMax*sizeof(int));
	for(y = 0; y < nHeight; y++)
	{
		point_pos=y*nWidth;
		for(x = 0; x < nWidth; x++)
		{
			nEdge = pnEdge[point_pos+x];
			if(		nEdge >=  nMax)	nEdge =  nMax-1;
			else if(nEdge <= -nMax)	nEdge = -nMax+1;

			if(nEdge > 0)	pnHistP[ nEdge]++;
			else			pnHistN[-nEdge]++;
		}
	}
}

int CProcessEngine::CalcEdgeThres(int *pnHist, int nLen)
{
	int i, nMax, nSum, nNum, nMean, nStdDev;
	nMax = 0;
	for(i = 1; i < nLen; i++)
	{
		if(pnHist[nMax] < pnHist[i])	nMax = i;
	}
	nSum = 0; nNum = 0;
	for(i = nMax+1; i < nLen; i++)
	{
		nSum += pnHist[i]*i;
		nNum += pnHist[i];
	}
	if(!nNum)	return 20;
	nMean = (nSum/nNum);
	nStdDev = 0;
	for(i = nMax+1; i < nLen; i++)
		nStdDev += (i-nMean)*(i-nMean)*pnHist[i];	

	nStdDev = (int)sqrt((float)nStdDev)/nNum;
	return nMean+nStdDev;
}
