#include "file.h"

//************************************  д��ͼ��ѹ���ļ�  ******************************************

//���캯��
WriteCompress::WriteCompress(const string FileName)
{
	fout.open(FileName, ios::out | ios::binary);
	if (!fout.is_open())
		cerr << "�ļ�д��ʧ��" << endl;
}
//����
WriteCompress::~WriteCompress()
{
	fout.close();
}

void WriteCompress::WriteInt(int wi)
{
	for (int i = 0; i < 4; i++) {
		char ch = 0;//��д����ַ�
		for (int j = 0; j < 8; j++) {
			ch = ch << 1;
			if (wi & 0x80000000) //�����λΪ1
				ch = ch | 1;
			wi = wi << 1;
		}
		fout.put(ch);
	}
}

//��һ��char[BN][BN]д���ļ�
void WriteCompress::WriteBlock(char wc, int &zerocount)
{
	if (wc != 0) { //��ֵ��0
		if (zerocount != 0) {
			//�ҵ�ǰ�м���
			fout.put(0);//0��־
			//fout.put(zerocount);//д��0�ĸ���
			WriteInt(zerocount);//д��0�ĸ���
			zerocount = 0;//����
		}
		fout.put(wc);//���µ�ǰֵ
	}
	else //��Ϊ0���ݲ�д�룬0����+1
		zerocount++;
}

//��һ��char[BN][BN]д���ļ�
void WriteCompress::W_ScanBlock(char block[BN][BN])
{
	int zerocount = 0;//��¼�м���0

	//Ϊ�˽����½ǵ�0������������ȡZ��ɨ�����
	int i = 0;//������
	int j = 0;//������
	bool mode = 0;//0��1����ǰ��ͬ��ɨ�跽��
	while (true) {
		WriteBlock(block[j][i], zerocount);//��¼

		if (i == BN - 1 && j == BN - 1) { //�������һ���㣬�˳�
			if (block[j][i] == 0) { //��Ϊ0��д��0������
				fout.put(0);//0��־
				//fout.put(zerocount);//д��0�ĸ���
				WriteInt(zerocount);//д��0�ĸ���
			}
			break;
		}

		if (mode == 0) {//���ڴ����µ�����ɨ��
			if (j == 0) {//�������Ϸ�һ��
				i++;//����һλ
				mode = 1;//�ı��ƶ�����
			}
			else if (i == BN - 1){//�������ҷ�һ��
				j++;//����һλ
				mode = 1;//�ı��ƶ�����
			}
			else {//���ڱ߽磬�������ƶ�
				i++;
				j--;
			}
		}
		else {//���ڴ����ϵ�����ɨ��
			if (j == BN - 1) {//�������·�һ��
				i++;//����һλ
				mode = 0;//�ı��ƶ�����
			}
			else if (i == 0) {//��������һ��
				j++;//����һλ
				mode = 0;//�ı��ƶ�����
			}
			else {//���ڱ߽磬�������ƶ�
				i--;
				j++;
			}
		}
	}
}

//************************************  ��ȡͼ��ѹ���ļ�  ******************************************

//���캯��
ReadCompress::ReadCompress(const string FileName)
{
	fin.open(FileName, ios::in | ios::binary);
	if (!fin.is_open())
		cerr << "�ļ���ȡʧ��" << endl;
}
//����
ReadCompress::~ReadCompress()
{
	fin.close();
}

//���ļ��ж���UINT
UINT ReadCompress::ReadUINT()
{
	int ri = 0;
	char ch_uint[4];//�ĸ�char��Ϊһ��int
	fin.read(ch_uint, 4);//��ȡ4���ֽ�
	for (int i = 0; i < 4; i++) {
		unsigned int t = unsigned char(ch_uint[i]);
		ri = ri | (t << ((3 - i) * 8));
	}
	return ri;
}

void ReadCompress::ReadBlock(char* rb, int &zerocount)
{
	//char* rb: block�������λ��

	if (zerocount != 0) {//������0��Ҫд��
		*rb = 0;
		zerocount--;
	}
	else {//����Ҫ����0
		char rc = fin.get();//�������ֵ


		if (rc == 0) { //Ϊ0
			//��������0�ĸ���
			zerocount = ReadUINT();
			//��д��һ��0
			*rb = 0;
			zerocount--;
		}
		else //��0ֵ��ֱ��д��
			*rb = rc;
	}
}

void ReadCompress::R_ScanBlock(char block[BN][BN])
{
	int zerocount = 0;//��¼�м���0

	//Ϊ�˽����½ǵ�0������������ȡZ��ɨ�����
	int i = 0;//������
	int j = 0;//������
	bool mode = 0;//0��1����ǰ��ͬ��ɨ�跽��

	while (true) {
		ReadBlock(block[j] + i, zerocount);//�ѵ�ǰֵ����block

		if (i == BN - 1 && j == BN - 1) //�������һ���㣬�˳�
			break;

		if (mode == 0) {//���ڴ����µ�����ɨ��
			if (j == 0) {//�������Ϸ�һ��
				i++;//����һλ
				mode = 1;//�ı��ƶ�����
			}
			else if (i == BN - 1) {//�������ҷ�һ��
				j++;//����һλ
				mode = 1;//�ı��ƶ�����
			}
			else {//���ڱ߽磬�������ƶ�
				i++;
				j--;
			}
		}
		else {//���ڴ����ϵ�����ɨ��
			if (j == BN - 1) {//�������·�һ��
				i++;//����һλ
				mode = 0;//�ı��ƶ�����
			}
			else if (i == 0) {//��������һ��
				j++;//����һλ
				mode = 0;//�ı��ƶ�����
			}
			else {//���ڱ߽磬�������ƶ�
				i--;
				j++;
			}
		}
	}
}