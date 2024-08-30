#include "file.h"
#include<string.h>

void compress(const string FileName);
void read(const string FileName);

int main(int argc, char* argv[])
{
	if (argc != 3) {
		cout << "ָ���������" << endl;
		cout << "ʾ����ImageZip -compress lena.tiff" << endl;
		cout << "      ImageZip -read lena.dat" << endl;
		return -1;
	}
	if (strcmp(argv[1], "-compress") == 0) {
		compress(argv[2]); //ѹ���ļ�
	}
	else if (strcmp(argv[1], "-read") == 0) {
		read(argv[2]); //��ȡ�ļ�
	}
	else {
		cout << "ָ���������" << endl;
		return -1;
	}
	return 0;
}

void compress(const string FileName)
{
	ImgData imgdata;//ͼ������
	PicReader imread;//��ͼ����

	imread.readPic(FileName.c_str());//��ȡ��ӦͼƬ
	imread.getData(imgdata.data, imgdata.x, imgdata.y);//��������
	imread.showPic(imgdata.data, imgdata.x, imgdata.y);//��ʾͼƬ

	//ȷ��ѹ��ͼƬ���ļ���
	int dotp = int(FileName.find_last_of("."));//�ҵ����һ�����λ��
	string CompressName = FileName.substr(0, dotp) + ".dat";
	
	WriteCompress wc(CompressName);
	wc.WriteInt((imgdata.x / BN) * BN);//д��ͼƬ�ߴ�
	wc.WriteInt((imgdata.y / BN) * BN);//д��ͼƬ�ߴ�

	//Ĭ��x��yΪN�ı���
	//������ȡ8*8�Ŀ�
	EnDCT endct;
	for (unsigned int i = 0; i < imgdata.y / BN; i++) {
		for (unsigned int j = 0; j < imgdata.x / BN; j++) {
			endct.GetEn(imgdata, i, j);//��������
			wc.W_ScanBlock(endct.Q[0]);
			wc.W_ScanBlock(endct.Q[1]);
			wc.W_ScanBlock(endct.Q[2]);
		}
	}
	cout << "ԭͼƬ " << FileName << " �ѱ��ɹ�ѹ��Ϊ " << CompressName << endl;
}

void read(const string FileName)//��ȡѹ���ļ�����ʾͼƬ
{
	ImgData imgdata;//ͼ������
	ReadCompress rc(FileName);//��ȡͼ��ѹ���ļ�����
	//��ȡx,y
	imgdata.x = rc.ReadUINT();
	imgdata.y = rc.ReadUINT();
	imgdata.data = new(nothrow) BYTE[imgdata.x * imgdata.y * 4];//��̬�ڴ����룬�����������ͷ�
	if (imgdata.data == NULL)
		return;

	DeDCT dedct;//����DCT����ָ�ͼƬ���ݵĹ���
	//Ĭ��x��yΪN�ı���
	//������ȡ8*8�Ŀ�
	for (unsigned int i = 0; i < imgdata.y / BN; i++) {
		for (unsigned int j = 0; j < imgdata.x / BN; j++) {
			rc.R_ScanBlock(dedct.Q[0]);//����YUV��
			rc.R_ScanBlock(dedct.Q[1]);
			rc.R_ScanBlock(dedct.Q[2]);
			dedct.GetDe(imgdata, i, j);//�ѿ���뵽ͼƬ���ݵĶ�Ӧλ��
		}
	}
	PicReader imread;//��ͼ����
	cout << "�ɹ���ȡѹ��ͼƬ" << FileName << endl;
	imread.showPic(imgdata.data, imgdata.x, imgdata.y);//��ʾͼƬ
}
