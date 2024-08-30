#pragma once
#define BN 8
#define PI 3.1415926
#include "PicReader.h"
#include<iostream>
using namespace std;

//图像数据
class ImgData {
public:
	BYTE* data; //RGBA
	UINT x;//宽
	UINT y;//高
	ImgData() {
		data = nullptr;
	}
	~ImgData() {//析构释放
		delete[] data;
		data = nullptr;
	}
};

//一个8*8的RGB块
class RGBblock {
public:
	BYTE R[BN][BN];
	BYTE G[BN][BN];
	BYTE B[BN][BN];
};
//一个8*8的YUV块
class YUVblock {
public:
	double Y[BN][BN];
	double U[BN][BN];
	double V[BN][BN];
};

//离散余弦DCT变换，编码
class EnDCT {
public:
	RGBblock RGBblock;
	YUVblock YUVblock;

	double A[BN][BN];
	double At[BN][BN];

	double F[3][BN][BN];//F = A * f * At
	char Q[3][BN][BN]; //F经过量化

	void GetRGB(BYTE* BlockData);//从一个数组中读入RGB
	void RGBtoYUV();//RGB转为YUV
	void GetAAt();//获取转换矩阵及其转置
	void GetF();//F = A * f * At
	void GetQ(); //F经过量化
	void GetEn(ImgData& imgdata, int i, int j);//接收新的数据，重新编码

	EnDCT() {
		GetAAt();//获取转换矩阵
	}
};


//解码
//离散余弦DCT变换，解码
class DeDCT {
public:
	RGBblock RGBblock;
	YUVblock YUVblock;

	double A[BN][BN];
	double At[BN][BN];

	double F[3][BN][BN];//F = A * f * At
	char Q[3][BN][BN]; //F经过量化
	
	
	void GetAAt();//获取转换函数及其转置
	void GetF(); //Q经过反量化
	void GetYUV();//f = A * F * At
	
	void YUVtoRGB();//YUV转为RGB
	void GetBlock(BYTE* BlockData);//把RGB块读入到一个数组
	void GetDe(ImgData& imgdata, int i, int j);//接收新的数据，重新编码

	DeDCT() {
		GetAAt();//获取转换函数
	}
	
};