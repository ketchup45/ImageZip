#pragma once
#include<fstream>
#include "dct.h"

//д�ļ�����
class WriteCompress {
public:
	//string FileName;//�ļ���
	ofstream fout;

	void WriteInt(int wi);//��һ��intд���ļ�
	void WriteBlock(char wc, int& zerocount);//�ѿ�д���ļ�
	void W_ScanBlock(char block[BN][BN]);//ɨ��BN*BN�Ŀ飬׼��д

	WriteCompress(const string FileName);
	~WriteCompress();
};

//���ļ�����
class ReadCompress {
public:
	//string FileName;//�ļ���
	ifstream fin;

	UINT ReadUINT();//���ļ��ж���һ��unsigned int
	void ReadBlock(char* rb, int& zerocount);//��ȡ���ļ�����ֵ����Ӧ����
	void R_ScanBlock(char block[BN][BN]);//ɨ��BN*BN�Ŀ飬׼����

	ReadCompress(const string FileName);
	~ReadCompress();
};
