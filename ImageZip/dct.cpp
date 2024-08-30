#include "dct.h"
#include <math.h>


/****************************     RGB与YUV转化    *******************************/
//带入公式
void EnDCT::RGBtoYUV()
{
    for (int i = 0; i < BN; i++) {
        for (int j = 0; j < BN; j++) {
            YUVblock.Y[i][j] =  0.299 * RGBblock.R[i][j] + 0.587 * RGBblock.G[i][j] + 0.114 * RGBblock.B[i][j] - 128;
            YUVblock.U[i][j] = -0.169 * RGBblock.R[i][j] - 0.331 * RGBblock.G[i][j] + 0.500 * RGBblock.B[i][j] + 128;
            YUVblock.V[i][j] =  0.500 * RGBblock.R[i][j] - 0.419 * RGBblock.G[i][j] - 0.081 * RGBblock.B[i][j] + 128;
        }
    }
}
void DeDCT::YUVtoRGB()
{
    for (int i = 0; i < BN; i++) {
        for (int j = 0; j < BN; j++) {
            //消去偏移量
            YUVblock.Y[i][j] += 128;
            YUVblock.U[i][j] -= 128;
            YUVblock.V[i][j] -= 128;
            //计算   
            double t1 = round(YUVblock.Y[i][j] + 1.4075 * YUVblock.V[i][j]);
            if (t1 >= 255)
                RGBblock.R[i][j] = 255;
            else if (t1 <= 0)
                RGBblock.R[i][j] = 0;
            else
                RGBblock.R[i][j] = BYTE(t1);

            double t2 = round(YUVblock.Y[i][j] - 0.3455 * YUVblock.U[i][j] - 0.7169 * YUVblock.V[i][j]);
            if (t2 >= 255)
                RGBblock.G[i][j] = 255;
            else if (t2 <= 0)
                RGBblock.G[i][j] = 0;
            else
                RGBblock.G[i][j] = BYTE(t2);

            double t3 = round(YUVblock.Y[i][j] + 1.779 *  YUVblock.U[i][j]);
            if (t3 >= 255)
                RGBblock.B[i][j] = 255;
            else if (t3 <= 0)
                RGBblock.B[i][j] = 0;
            else
                RGBblock.B[i][j] = BYTE(t3);
        }
    }
}
/***********************************************************************************************************/

void EnDCT::GetRGB(BYTE* BlockData)
{
    BYTE* dp = BlockData;  //读DataStart的指针
    BYTE* rp = RGBblock.R[0];//写R的指针
    BYTE* gp = RGBblock.G[0];//写G的指针
    BYTE* bp = RGBblock.B[0];//写B的指针
    for (int i = 0; i < BN * BN; i++) {//读取8*8个RGB
        *rp++ = *dp++; //R
        *gp++ = *dp++; //G
        *bp++ = *dp++; //B
        dp++;//A
    }
}

//根据公式计算转换矩阵及其转置
void EnDCT::GetAAt()
{
    //求A
    for (int i = 0; i < BN; i++) {
        for (int j = 0; j < BN; j++) {
            double c = i == 0 ? sqrt(1.0 / BN) : sqrt(2.0 / BN);
            A[i][j] = c * cos((j + 0.5) * PI * i / BN);
        }
    }
    //求At，转置
    for (int i = 0; i < BN; i++)
        for (int j = 0; j < BN; j++)
            At[i][j] = A[j][i];
}

/*
测试数据
double mY[8][8] =
{
    {-76,-73,-67,-62,-58,-67,-64,-55},
    {-65,-69,-73,-38,-19,-43,-59,-56},
    {-66,-69,-60,-15, 16,-24,-62,-55},
    {-65,-70,-57, -6, 26,-22,-58,-59},
    {-61,-67,-60,-24, -2,-40,-60,-58},
    {-49,-63,-68,-58,-51,-60,-70,-53},
    {-43,-57,-64,-69,-73,-67,-63,-45},
    {-41,-49,-59,-60,-63,-52,-50,-34}
};
*/

//矩阵乘法运算 A0 = A1 * A2 * A3
void MatrixMult(double A0[BN][BN], double A1[BN][BN], double A2[BN][BN], double A3[BN][BN])
{
    double t[BN][BN]; //A1*A2
    for (int i = 0; i < BN; i++) {
        for (int j = 0; j < BN; j++) {
            t[i][j] = 0;
            for (int k = 0; k < BN; k++) //累加求和
                t[i][j] += A1[i][k] * A2[k][j];
        }
    }
    for (int i = 0; i < BN; i++) {
        for (int j = 0; j < BN; j++) {
            A0[i][j] = 0;
            for (int k = 0; k < BN; k++) //累加求和
                A0[i][j] += t[i][k] * A3[k][j];
        }
    }
}

//F = A * f * At
void EnDCT::GetF()
{
    MatrixMult(F[0], A, YUVblock.Y, At);
    MatrixMult(F[1], A, YUVblock.U, At);
    MatrixMult(F[2], A, YUVblock.V, At);
}

//亮度分量量化表
const BYTE quant_Y[BN][BN] = {
    {16, 11, 10, 16, 24,  40,  51,  61},
    {12, 12, 14, 19, 26,  58,  60,  55},
    {14, 13, 16, 24, 40,  57,  69,  56},
    {14, 17, 22, 29, 51,  87,  80,  62},
    {18, 22, 37, 56, 68,  109, 103, 77},
    {24, 35, 55, 64, 81,  104, 113, 92},
    {49, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99}
};

//色度分量量化表
const BYTE quant_UV[BN][BN] = {
    {17, 18, 24, 47, 99, 99, 99, 99},
    {18, 21, 26, 66, 99, 99, 99, 99},
    {24, 26, 56, 99, 99, 99, 99, 99},
    {47, 66, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99}
};

//F经过量化
void EnDCT::GetQ()
{
    for (int i = 0; i < BN; i++) {
        for (int j = 0; j < BN; j++) {
            //四舍五入
            Q[0][i][j] = char(round(F[0][i][j] / quant_Y[i][j])); //Y
            Q[1][i][j] = char(round(F[1][i][j] / quant_UV[i][j]));//U
            Q[2][i][j] = char(round(F[2][i][j] / quant_UV[i][j]));//V
        }
    }
}

//获得编码
void EnDCT::GetEn(ImgData& imgdata, int i, int j)
{
    //生成BlockData
    BYTE* BlockData = new(nothrow) BYTE[BN * BN * 4];
    if (BlockData == NULL)
        return;

    BYTE* bp = BlockData;//用于写入BlockData的指针
    for (int m = 0; m < BN; m++) { //在BN行中的行数
        //定位到起始位置
        BYTE* tp = imgdata.data + ((i * BN + m) * imgdata.x + j * BN) * 4;
        for (int n = 0; n < BN; n++) { //在BN列中的列数
            *bp++ = *tp++;//R
            *bp++ = *tp++;//G
            *bp++ = *tp++;//B
            *bp++ = *tp++;//A
        }
    }

    //后续步骤
    GetRGB(BlockData);
    RGBtoYUV();//RGB转为YUV
    GetF();//F = A * f * At
    GetQ(); //F经过量化

    //释放
    delete[] BlockData;
}

/**************************************  读取压缩文件解码  *********************************************/

//根据公式计算转换矩阵及其转置
void DeDCT::GetAAt()
{
    //求A
    for (int i = 0; i < BN; i++) {
        for (int j = 0; j < BN; j++) {
            double c = i == 0 ? sqrt(1.0 / BN) : sqrt(2.0 / BN);
            A[i][j] = c * cos((j + 0.5) * PI * i / BN);
        }
    }
    //求At，转置
    for (int i = 0; i < BN; i++)
        for (int j = 0; j < BN; j++)
            At[i][j] = A[j][i];
}

//Q计算量化前的F
void DeDCT::GetF()
{
    for (int i = 0; i < BN; i++) {
        for (int j = 0; j < BN; j++) {
            //四舍五入
            F[0][i][j] = double(Q[0][i][j]) * quant_Y[i][j];  //Y
            F[1][i][j] = double(Q[1][i][j]) * quant_UV[i][j]; //U
            F[2][i][j] = double(Q[2][i][j]) * quant_UV[i][j]; //V
        }
    }
}

//f = At * F * A
void DeDCT::GetYUV()
{
    MatrixMult(YUVblock.Y, At, F[0], A);
    MatrixMult(YUVblock.U, At, F[1], A);
    MatrixMult(YUVblock.V, At, F[2], A);
}

void DeDCT::GetBlock(BYTE* BlockData)
{
    BYTE* dp = BlockData;  //读BlockData的指针
    BYTE* rp = RGBblock.R[0];//写R的指针
    BYTE* gp = RGBblock.G[0];//写G的指针
    BYTE* bp = RGBblock.B[0];//写B的指针
    for (int i = 0; i < BN * BN; i++) {//读入到BlockData数组
        *dp++ = *rp++; //R
        *dp++ = *gp++; //G
        *dp++ = *bp++; //B
        *dp++ = 255;//A
    }
}

//获得解码
void DeDCT::GetDe(ImgData& imgdata, int i, int j)
{
    //后续步骤
    GetF();//Q经过反量化
    GetYUV();//f = A * F * At
    YUVtoRGB();//RGB转为YUV

    //生成BlockData
    BYTE* BlockData = new(nothrow) BYTE[BN * BN * 4];
    if (BlockData == NULL)
        return;
    GetBlock(BlockData);//把RGB块读入到一个数组

    BYTE* bp = BlockData;//用于读入BlockData的指针
    for (int m = 0; m < BN; m++) { //在BN行中的行数
        //定位到起始位置
        BYTE* tp = imgdata.data + ((i * BN + m) * imgdata.x + j * BN) * 4;
        for (int n = 0; n < BN; n++) { //在BN列中的列数
            *tp++ = *bp++;//R
            *tp++ = *bp++;//G
            *tp++ = *bp++;//B
            *tp++ = *bp++;//A
        }
    }

    //释放
    delete[] BlockData;
}

