#pragma once
#define BN 8
#define PI 3.1415926
#include "PicReader.h"
#include<iostream>
using namespace std;

//ͼ������
class ImgData {
public:
	BYTE* data; //RGBA
	UINT x;//��
	UINT y;//��
	ImgData() {
		data = nullptr;
	}
	~ImgData() {//�����ͷ�
		delete[] data;
		data = nullptr;
	}
};

//һ��8*8��RGB��
class RGBblock {
public:
	BYTE R[BN][BN];
	BYTE G[BN][BN];
	BYTE B[BN][BN];
};
//һ��8*8��YUV��
class YUVblock {
public:
	double Y[BN][BN];
	double U[BN][BN];
	double V[BN][BN];
};

//��ɢ����DCT�任������
class EnDCT {
public:
	RGBblock RGBblock;
	YUVblock YUVblock;

	double A[BN][BN];
	double At[BN][BN];

	double F[3][BN][BN];//F = A * f * At
	char Q[3][BN][BN]; //F��������

	void GetRGB(BYTE* BlockData);//��һ�������ж���RGB
	void RGBtoYUV();//RGBתΪYUV
	void GetAAt();//��ȡת��������ת��
	void GetF();//F = A * f * At
	void GetQ(); //F��������
	void GetEn(ImgData& imgdata, int i, int j);//�����µ����ݣ����±���

	EnDCT() {
		GetAAt();//��ȡת������
	}
};


//����
//��ɢ����DCT�任������
class DeDCT {
public:
	RGBblock RGBblock;
	YUVblock YUVblock;

	double A[BN][BN];
	double At[BN][BN];

	double F[3][BN][BN];//F = A * f * At
	char Q[3][BN][BN]; //F��������
	
	
	void GetAAt();//��ȡת����������ת��
	void GetF(); //Q����������
	void GetYUV();//f = A * F * At
	
	void YUVtoRGB();//YUVתΪRGB
	void GetBlock(BYTE* BlockData);//��RGB����뵽һ������
	void GetDe(ImgData& imgdata, int i, int j);//�����µ����ݣ����±���

	DeDCT() {
		GetAAt();//��ȡת������
	}
	
};