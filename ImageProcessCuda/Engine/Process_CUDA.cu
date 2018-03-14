
#include "Process_CUDA.h"

#ifdef USE_CUDA
#include <assert.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <helper_cuda.h>
#include <cuda_runtime.h>
#include "device_launch_parameters.h"
#include "device_functions.h"

#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))


#define GetRectWidth(x) (x.right - x.left + 1)
#define GetRectHeight(x) (x.bottom - x.top + 1)
#define round(a) ((a>0.0f) ? (int)(a + 0.5f) : (int)(a - 0.5f))
#define round2(a, b) ((a> 0) ? (a + b / 2) : (a - b / 2))

void cudaProcess::AllocCudaMem()
{
	if (m_width == 0 || m_height == 0)
		return;

	int mem_size = m_width*m_height;
	cudaMallocManaged((void**)&m_dpbGray, mem_size);
	cudaMallocManaged((void**)&m_dpnEdge, mem_size*sizeof(int));
	cudaMallocManaged((void**)&m_dpbEdgeSpec, mem_size);
	cudaMallocManaged((void**)&m_dpbTemp, mem_size);
}
void cudaProcess::FreeCudaMem()
{
	cudaFree(m_dpbGray);
	cudaFree(m_dpnEdge);
	cudaFree(m_dpbEdgeSpec);
	cudaFree(m_dpbTemp);
}


__global__ void
    DetectEdgeKernel(BYTE* pbGray, BYTE* pbEdge, int nWidth, int nHeight)
{

	int i = blockDim.x * blockIdx.x + threadIdx.x;
	int idy = i / nWidth;
	int idx = i - idy * nWidth;
	int pos = idy*nWidth + idx;
	int subSum, localSum;
	
	if (idx < 0 || idx >= nWidth || idy < 0 || idy >= nHeight)
		return;
	if (idx <= 3 || idy <= 3 || idy >= nHeight - 3 || idx >= nWidth - 3)
	{
		pbEdge[pos] = 128;//white
		return;
	}
	subSum = localSum = 0;
	for (int y = idy - 3; y <= idy + 3; y++)
		for (int x = idx - 3; x <= idx + 3; x++)
			subSum += pbGray[y*nWidth + x];
	localSum = pbGray[pos] + pbGray[pos - 1] + pbGray[pos + 1] + pbGray[pos-nWidth] + pbGray[pos+nWidth];
	subSum -= localSum;
	subSum /= 44;
	localSum /= 5;
	if (subSum - localSum > 3)
		pbEdge[pos] = 0;
	else if (subSum - localSum < -3)
		pbEdge[pos] = 255;
	else
		pbEdge[pos] = 128;
}

void cudaProcess::setGrayData(BYTE* pbGray)
{
	int mem_size = m_width*m_height;
	cudaMemcpy(m_dpbGray, pbGray, mem_size, cudaMemcpyHostToDevice);

	int threadsPerBlock = 256;
	int blocksPerGrid = (mem_size + threadsPerBlock - 1) / threadsPerBlock;
	DetectEdgeKernel << < blocksPerGrid, threadsPerBlock >> >(m_dpbGray, m_dpbEdgeSpec, m_width, m_height);
}

void cudaProcess::getEdgeSpec(BYTE* pbEdgeSpec)
{
	int mem_size = m_width*m_height;
	//int threadsPerBlock = 256;
	//int blocksPerGrid = (mem_size + threadsPerBlock - 1) / threadsPerBlock;
	//DetectEdgeKernel <<< blocksPerGrid, threadsPerBlock >>>(m_dpbGray, m_dpbEdgeSpec, m_width, m_height);
	cudaMemcpy(pbEdgeSpec, m_dpbEdgeSpec, mem_size, cudaMemcpyDeviceToHost);

}

#endif