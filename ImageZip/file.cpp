#include "file.h"

//************************************  写入图像压缩文件  ******************************************

//构造函数
WriteCompress::WriteCompress(const string FileName)
{
	fout.open(FileName, ios::out | ios::binary);
	if (!fout.is_open())
		cerr << "文件写入失败" << endl;
}
//析构
WriteCompress::~WriteCompress()
{
	fout.close();
}

void WriteCompress::WriteInt(int wi)
{
	for (int i = 0; i < 4; i++) {
		char ch = 0;//待写入的字符
		for (int j = 0; j < 8; j++) {
			ch = ch << 1;
			if (wi & 0x80000000) //若最高位为1
				ch = ch | 1;
			wi = wi << 1;
		}
		fout.put(ch);
	}
}

//把一个char[BN][BN]写入文件
void WriteCompress::WriteBlock(char wc, int &zerocount)
{
	if (wc != 0) { //该值非0
		if (zerocount != 0) {
			//且当前有计数
			fout.put(0);//0标志
			//fout.put(zerocount);//写下0的个数
			WriteInt(zerocount);//写下0的个数
			zerocount = 0;//清零
		}
		fout.put(wc);//记下当前值
	}
	else //若为0，暂不写入，0计数+1
		zerocount++;
}

//把一个char[BN][BN]写入文件
void WriteCompress::W_ScanBlock(char block[BN][BN])
{
	int zerocount = 0;//记录有几个0

	//为了将右下角的0集中起来，采取Z形扫描程序
	int i = 0;//横坐标
	int j = 0;//纵坐标
	bool mode = 0;//0、1代表当前不同的扫描方向
	while (true) {
		WriteBlock(block[j][i], zerocount);//记录

		if (i == BN - 1 && j == BN - 1) { //到达最后一个点，退出
			if (block[j][i] == 0) { //若为0，写入0的数量
				fout.put(0);//0标志
				//fout.put(zerocount);//写下0的个数
				WriteInt(zerocount);//写下0的个数
			}
			break;
		}

		if (mode == 0) {//正在从左下到右上扫描
			if (j == 0) {//到达最上方一行
				i++;//右移一位
				mode = 1;//改变移动方向
			}
			else if (i == BN - 1){//到达最右方一行
				j++;//下移一位
				mode = 1;//改变移动方向
			}
			else {//不在边界，向右上移动
				i++;
				j--;
			}
		}
		else {//正在从右上到左下扫描
			if (j == BN - 1) {//到达最下方一行
				i++;//右移一位
				mode = 0;//改变移动方向
			}
			else if (i == 0) {//到达最左方一行
				j++;//下移一位
				mode = 0;//改变移动方向
			}
			else {//不在边界，向左下移动
				i--;
				j++;
			}
		}
	}
}

//************************************  读取图像压缩文件  ******************************************

//构造函数
ReadCompress::ReadCompress(const string FileName)
{
	fin.open(FileName, ios::in | ios::binary);
	if (!fin.is_open())
		cerr << "文件读取失败" << endl;
}
//析构
ReadCompress::~ReadCompress()
{
	fin.close();
}

//从文件中读入UINT
UINT ReadCompress::ReadUINT()
{
	int ri = 0;
	char ch_uint[4];//四个char视为一个int
	fin.read(ch_uint, 4);//读取4个字节
	for (int i = 0; i < 4; i++) {
		unsigned int t = unsigned char(ch_uint[i]);
		ri = ri | (t << ((3 - i) * 8));
	}
	return ri;
}

void ReadCompress::ReadBlock(char* rb, int &zerocount)
{
	//char* rb: block待读入的位置

	if (zerocount != 0) {//仍有余0需要写入
		*rb = 0;
		zerocount--;
	}
	else {//不需要读入0
		char rc = fin.get();//待读入的值


		if (rc == 0) { //为0
			//继续读入0的个数
			zerocount = ReadUINT();
			//再写入一个0
			*rb = 0;
			zerocount--;
		}
		else //非0值，直接写入
			*rb = rc;
	}
}

void ReadCompress::R_ScanBlock(char block[BN][BN])
{
	int zerocount = 0;//记录有几个0

	//为了将右下角的0集中起来，采取Z形扫描程序
	int i = 0;//横坐标
	int j = 0;//纵坐标
	bool mode = 0;//0、1代表当前不同的扫描方向

	while (true) {
		ReadBlock(block[j] + i, zerocount);//把当前值读到block

		if (i == BN - 1 && j == BN - 1) //到达最后一个点，退出
			break;

		if (mode == 0) {//正在从左下到右上扫描
			if (j == 0) {//到达最上方一行
				i++;//右移一位
				mode = 1;//改变移动方向
			}
			else if (i == BN - 1) {//到达最右方一行
				j++;//下移一位
				mode = 1;//改变移动方向
			}
			else {//不在边界，向右上移动
				i++;
				j--;
			}
		}
		else {//正在从右上到左下扫描
			if (j == BN - 1) {//到达最下方一行
				i++;//右移一位
				mode = 0;//改变移动方向
			}
			else if (i == 0) {//到达最左方一行
				j++;//下移一位
				mode = 0;//改变移动方向
			}
			else {//不在边界，向左下移动
				i--;
				j++;
			}
		}
	}
}