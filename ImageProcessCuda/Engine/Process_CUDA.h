#pragma once

#include "type_define.h"

#ifdef USE_CUDA

class cudaProcess
{
	int m_width, m_height;
	BYTE* m_dpbGray;
	int*  m_dpnEdge;
	BYTE* m_dpbEdgeSpec;
	BYTE* m_dpbTemp;
	//	STROKE* m_dpStroke;
public:
	cudaProcess() { m_width = m_height = 0; }
	cudaProcess(int wid, int hei) {
		m_width = wid; m_height = hei;
	}
	void setImageSize(int wid, int hei) {
		m_width = wid; m_height = hei;
	}
	void AllocCudaMem();
	void FreeCudaMem();
	void setGrayData(BYTE* pbGray);
	void getEdgeSpec(BYTE* pbEdgeSpec);
	//	void setStroke(STROKE *pStroke, int nStrokeNum);
};

#endif
