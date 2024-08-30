#pragma once
#include<fstream>
#include "dct.h"

//写文件的类
class WriteCompress {
public:
	//string FileName;//文件名
	ofstream fout;

	void WriteInt(int wi);//把一个int写入文件
	void WriteBlock(char wc, int& zerocount);//把块写入文件
	void W_ScanBlock(char block[BN][BN]);//扫描BN*BN的块，准备写

	WriteCompress(const string FileName);
	~WriteCompress();
};

//读文件的类
class ReadCompress {
public:
	//string FileName;//文件名
	ifstream fin;

	UINT ReadUINT();//从文件中读入一个unsigned int
	void ReadBlock(char* rb, int& zerocount);//读取块文件的数值到对应矩阵
	void R_ScanBlock(char block[BN][BN]);//扫描BN*BN的块，准备读

	ReadCompress(const string FileName);
	~ReadCompress();
};
