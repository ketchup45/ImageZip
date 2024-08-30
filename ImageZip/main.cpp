#include "file.h"
#include<string.h>

void compress(const string FileName);
void read(const string FileName);

int main(int argc, char* argv[])
{
	if (argc != 3) {
		cout << "指令参数有误" << endl;
		cout << "示例：ImageZip -compress lena.tiff" << endl;
		cout << "      ImageZip -read lena.dat" << endl;
		return -1;
	}
	if (strcmp(argv[1], "-compress") == 0) {
		compress(argv[2]); //压缩文件
	}
	else if (strcmp(argv[1], "-read") == 0) {
		read(argv[2]); //读取文件
	}
	else {
		cout << "指令参数有误" << endl;
		return -1;
	}
	return 0;
}

void compress(const string FileName)
{
	ImgData imgdata;//图像数据
	PicReader imread;//读图工具

	imread.readPic(FileName.c_str());//读取对应图片
	imread.getData(imgdata.data, imgdata.x, imgdata.y);//读入数据
	imread.showPic(imgdata.data, imgdata.x, imgdata.y);//显示图片

	//确定压缩图片的文件名
	int dotp = int(FileName.find_last_of("."));//找到最后一个点的位置
	string CompressName = FileName.substr(0, dotp) + ".dat";
	
	WriteCompress wc(CompressName);
	wc.WriteInt((imgdata.x / BN) * BN);//写入图片尺寸
	wc.WriteInt((imgdata.y / BN) * BN);//写入图片尺寸

	//默认x，y为N的倍数
	//遍历读取8*8的块
	EnDCT endct;
	for (unsigned int i = 0; i < imgdata.y / BN; i++) {
		for (unsigned int j = 0; j < imgdata.x / BN; j++) {
			endct.GetEn(imgdata, i, j);//读入数据
			wc.W_ScanBlock(endct.Q[0]);
			wc.W_ScanBlock(endct.Q[1]);
			wc.W_ScanBlock(endct.Q[2]);
		}
	}
	cout << "原图片 " << FileName << " 已被成功压缩为 " << CompressName << endl;
}

void read(const string FileName)//读取压缩文件，显示图片
{
	ImgData imgdata;//图像数据
	ReadCompress rc(FileName);//读取图像压缩文件工具
	//读取x,y
	imgdata.x = rc.ReadUINT();
	imgdata.y = rc.ReadUINT();
	imgdata.data = new(nothrow) BYTE[imgdata.x * imgdata.y * 4];//动态内存申请，在析构函数释放
	if (imgdata.data == NULL)
		return;

	DeDCT dedct;//根据DCT编码恢复图片数据的工具
	//默认x，y为N的倍数
	//遍历读取8*8的块
	for (unsigned int i = 0; i < imgdata.y / BN; i++) {
		for (unsigned int j = 0; j < imgdata.x / BN; j++) {
			rc.R_ScanBlock(dedct.Q[0]);//读入YUV块
			rc.R_ScanBlock(dedct.Q[1]);
			rc.R_ScanBlock(dedct.Q[2]);
			dedct.GetDe(imgdata, i, j);//把块解码到图片数据的对应位置
		}
	}
	PicReader imread;//读图工具
	cout << "成功读取压缩图片" << FileName << endl;
	imread.showPic(imgdata.data, imgdata.x, imgdata.y);//显示图片
}
