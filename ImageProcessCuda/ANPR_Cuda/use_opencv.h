/////////////////////////////////////////////////////////////////////
// use_opencv.h

#pragma once

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2\imgcodecs\imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/core/core_c.h"
//#include "opencv2/highgui/highgui_c.h"
//#include "opencv2/imgproc/imgproc_c.h"
//#include "opencv2/video/video.hpp"
//#include "opencv2/nonfree/nonfree.hpp"
//#include "opencv2/videostab/videostab.hpp"
//#include "opencv2/features2d/features2d.hpp"
//#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/flann/miniflann.hpp"
//#include "opencv2/photo/photo.hpp"
//#include "opencv2/calib3d/calib3d.hpp"
//#include "opencv2/ml/ml.hpp"
//#include "opencv2/contrib/contrib.hpp"
//#include "opencv2/ts/ts.hpp"
//#include "opencv2/stitching/stitcher.hpp"
//#include "opencv2/legacy/legacy.hpp"
//#include "opencv2/video/background_segm.hpp"

#ifdef _DEBUG
	#pragma comment(lib,"opencv_core330d.lib")
	#pragma comment(lib,"opencv_highgui330d.lib")
//	#pragma comment(lib,"opencv_imgproc330d.lib")	
//	#pragma comment(lib,"opencv_imgcodecs330d.lib")
//	#pragma comment(lib,"opencv_video330d.lib")
//	#pragma comment(lib,"opencv_nonfree330d.lib")
//	#pragma comment(lib,"opencv_videostab330d.lib")
//	#pragma comment(lib,"opencv_features2d330d.lib")
//	#pragma comment(lib,"opencv_objdetect330d.lib")
//	#pragma comment(lib,"opencv_flann330d.lib")
//	#pragma comment(lib,"opencv_photo330d.lib")
//	#pragma comment(lib,"opencv_calib3d330d.lib")
//	#pragma comment(lib,"opencv_ml330d.lib")
//	#pragma comment(lib,"opencv_contrib330d.lib")
//	#pragma comment(lib,"opencv_ts330d.lib")
//	#pragma comment(lib,"opencv_stitching330d.lib")
//	#pragma comment(lib,"opencv_legacy330d.lib")
#else
	#pragma comment(lib,"opencv_core330.lib")
	#pragma comment(lib,"opencv_highgui330.lib")
	#pragma comment(lib,"opencv_imgproc330.lib")
	#pragma comment(lib,"opencv_imgcodecs330.lib")
//	#pragma comment(lib,"opencv_video330.lib")
//	#pragma comment(lib,"opencv_nonfree330.lib")
//	#pragma comment(lib,"opencv_videostab330.lib")
//	#pragma comment(lib,"opencv_features2d330.lib")
//	#pragma comment(lib,"opencv_objdetect330.lib")
//	#pragma comment(lib,"opencv_flann330.lib")
//	#pragma comment(lib,"opencv_photo330.lib")
//	#pragma comment(lib,"opencv_calib3d330.lib")
//	#pragma comment(lib,"opencv_ml330.lib")
//	#pragma comment(lib,"opencv_contrib330.lib")
//	#pragma comment(lib,"opencv_ts330.lib")
//	#pragma comment(lib,"opencv_stitching330.lib")
//	#pragma comment(lib,"opencv_legacy330.lib")
#endif

template<class T>
class TypedMat
{
	T** m_pData;
	int m_nChannels;
	int m_nRows, m_nCols;

public:
	TypedMat():m_pData(NULL),m_nChannels(1),m_nRows(0),m_nCols(0){}
	~TypedMat(){if(m_pData) delete [] m_pData;}

	// OpenCV Mat 연동 (메모리 공유)
	void Attach(const cv::Mat& m);
	void Attach(const IplImage& m);
	TypedMat(const cv::Mat& m):m_pData(NULL),m_nChannels(1),m_nRows(0),m_nCols(0) { Attach(m);}
	TypedMat(const IplImage& m):m_pData(NULL),m_nChannels(1),m_nRows(0),m_nCols(0) { Attach(m);}
	const TypedMat & operator =(const cv::Mat& m){ Attach(m); return *this;}
	const TypedMat & operator =(const IplImage& m){ Attach(m); return *this;}

	// 행(row) 반환
	T* GetPtr(int r)
	{ assert(r>=0 && r<m_nRows); return m_pData[r];}

	// 연산자 중첩 (원소접근) -- 2D
	T * operator [](int r)
	{ assert(r>=0 && r<m_nRows); return m_pData[r];}

	const T * operator [](int r) const
	{ assert(r>=0 && r<m_nRows); return m_pData[r];}

	// 연산자 중첩 (원소접근) -- 3D
	T & operator ()(int r, int c, int k)
	{ assert(r>=0 && r<m_nRows && c>=0 && c<m_nCols && k>=0 && k<m_nChannels); return m_pData[r][c*m_nChannels+k];}

	const T operator ()(int r, int c, int k) const
	{ assert(r>=0 && r<m_nRows && c>=0 && c<m_nCols && k>=0 && k<m_nChannels); return m_pData[r][c*m_nChannels+k];}
};

template<class T>
void TypedMat<T>::Attach(const cv::Mat& m)
{
	assert(sizeof(T)==m.elemSize1());

	m_nChannels = m.channels();
	m_nRows = m.rows;
	m_nCols = m.cols;
	
	if(m_pData) delete [] m_pData;
	m_pData = new T * [m_nRows];
	for(int r=0; r<m_nRows; r++)
	{
		m_pData[r] = (T *)(m.data + r*m.step);
	}
}

template<class T>
void TypedMat<T>::Attach(const IplImage& m)
{
	assert(sizeof(T) == m.widthStep/(m.width*m.nChannels));

	m_nChannels = m.nChannels;
	m_nRows = m.height;
	m_nCols = m.width;
	
	if(m_pData) delete [] m_pData;
	m_pData = new T * [m_nRows];
	for(int r=0; r<m_nRows; r++)
	{
		m_pData[r] = (T *)(m.imageData + r*m.widthStep);
	}
}
