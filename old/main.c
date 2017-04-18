/// <summary>  
/// 程序功能：c语言实现纯高斯模糊
/// 系统Ubuntu 15.10，GCC开发环境,编程语言C,最新整理时间 whd 2016.9.2。  
/// 参考代码：https://github.com/Duwaz/Gaussian_Blur
/// <remarks> 1: 能处理24位BMP格式图像。</remarks>    
/// <remarks> 2: 主程序无参数,默认处理工程目录下的input.bmp,处理后的结果为output.bmp。</remarks>    
/// <remarks> 3: 处理效果由高斯模糊半径决定:GaussBlur(bmp_t*, double)函数的第二个参数</remarks> 
/// </summary> 
#include<stdio.h>  
#include<stdlib.h>
#include<malloc.h>
#include <math.h>
#define SQRT_2PI 2.506628274631
//自定义数据类型
typedef unsigned long       DWORD; //四个字节
typedef int                 BOOL;
typedef unsigned char       BYTE; //一个字节
typedef unsigned short      WORD; //一个字节
//位图信息头结构体定义
typedef struct tagBITMAPINFOHEADER
{
   WORD   bfType;  //两个字节
   DWORD  bfileSize;
   DWORD  bfReserved;
   DWORD  bOffBits;
   DWORD  biSize;   //4个字节
   long   biWidth;  //4个字节
   long   biHeight;
   WORD   biPlanes;
   WORD   biBitCount;
   DWORD  biCompression;
   DWORD  biSizeImage;
   long   biXPelsPerMeter;
   long   biYPelsPerMeter;
   DWORD biClrUsed;
   DWORD biClrImportant;
} __attribute__((packed))BITMAPINFOHEADER,*PBITMAPINFOHEADER;  //字节对齐
typedef struct
{
	BITMAPINFOHEADER header;  //信息头
	char *data;//图像数据
} bmp_t;
int win_size(double);  
double Gauss(double, double);
void bmp_free(bmp_t *);
bmp_t *bmp_open(FILE *);
int bmp_write(bmp_t*, FILE*);
bmp_t *GaussBlur(bmp_t*, double);
int main(void)  
{  
        char *InputName, *OutputName;  //输入输出图像文件名变量
	FILE *InputFile, *OutputFile;  //输入输出图像文件
	bmp_t *bmp = NULL, *blur = NULL;
        InputName = "input.bmp"; OutputName = "output.bmp";
        if(!(InputFile=fopen(InputName, "r")))  //图像文件打开函数,打不开返回,提示找不到文件
	{
		printf("File not found\n");
		return 1;
	}
        bmp = bmp_open(InputFile);   //图像文件读取操作,前面必须有打开的操作才能读取文件数据
	fclose(InputFile);//关闭文件
        OutputFile = fopen(OutputName, "wb"); //打开并创建输出文件
        if (OutputFile == NULL) 
        {
		printf("Can't open %s\n", OutputName);
		return 1;
	}
        blur = GaussBlur(bmp, 4.0); //模糊处理并返回处理后数据
        bmp_write(blur, OutputFile);//将处理后数据存放到新建的文件中 
        fclose(OutputFile); //关闭文件
        bmp_free(bmp);//释放存放读取数据的内存
        free(blur);//释放处理后图像数据的内存
        return 0;
}  
/// <summary>  
/// 函数功能:给定一个BMP图像文件,将其中的数据读取出来,并返回图像数据  
/// 函数返回:图像的数据(信息头和数据)
/// 知识点:如何读取一个图像文件的数据 
/// </summary>  
/// <param name="f">图像文件存放位置地址。</param>  
bmp_t *bmp_open(FILE *f) 
{
	bmp_t *bmp;
	bmp = (bmp_t *)malloc(sizeof(bmp_t));
	bmp->data = NULL;

	if (fread(&(bmp->header), sizeof(BITMAPINFOHEADER), 1, f)) {
		bmp->data = (char*)malloc(bmp->header.biSizeImage);
		if (fread(bmp->data, bmp->header.biSizeImage, 1, f))
                 printf("图像读取成功\n");
                 printf("Width: %ld\n", bmp->header.biWidth);
                 printf("Height: %ld\n", bmp->header.biHeight);
                 printf("BitCount: %d\n\n", (int)bmp->header.biBitCount);
			return bmp;
	}
	fprintf(stderr, "Error reading file");
	bmp_free(bmp);
	return NULL;
}
/// <summary>  
/// 函数功能:释放存放图像数据的内存  
/// 函数返回:无
/// 知识点:释放图像数据内存 
/// </summary>  
/// <param name="bmp">图像数据变量。</param>  
void bmp_free(bmp_t *bmp)
{
	if (bmp == NULL) return;
	if (bmp->data != NULL) free(bmp->data);
	free(bmp);
}
/// <summary>  
/// 函数功能:将图像数据写入到图像文件中  
/// 函数返回:写入数据的个数
/// 知识点:将数据写入文件中(将数值从内存写入文件中)
/// </summary>  
/// <param name="bmp">图像数据变量。</param>  
/// <param name="out">文件流。</param>  
int bmp_write(bmp_t *bmp, FILE *out) {
	return fwrite(&(bmp->header), sizeof(BITMAPINFOHEADER), 1, out)
		&& fwrite(bmp->data, bmp->header.biSizeImage, 1, out);
}
/// <summary>  
/// 函数功能:模糊窗的大小(根据高斯半径和模糊半径满足3sigma原则) 
/// 函数返回:模糊窗的大小,即长或宽(长=宽)
/// 知识点:高斯模糊的3*sigma原则
/// </summary>  
/// <param name="sigma">高斯核函数的参数sigma</param>  
int win_size(double sigma)
{
	return (1 + (((int)ceil(3 * sigma)) * 2)); 
}
/// <summary>  
/// 函数功能:单像素点计算高斯系数 
/// 函数返回:高斯系数
/// 知识点:高斯系数计算公式
/// </summary>  
/// <param name="sigma">高斯核函数的参数sigma</param>  
/// <param name="x">当前像素距离模糊窗中心的距离</param>  
double Gauss(double sigma, double x)
{
	return exp(-(x * x) / (2.0 * sigma * sigma)) / (sigma * SQRT_2PI);
}
/// <summary>  
/// 函数功能:计算高斯模糊窗下的每个像素对应的权值,计算一半就够了,因为是权值是对称的(一维高斯)
/// 函数返回:模糊窗下每个像素的权值
/// 知识点:模糊窗下的各点的高斯系数(一维数组)
/// </summary>  
/// <param name="sigma">高斯核函数的参数sigma</param>  
/// <param name="win_size">模糊窗的大小</param>  
double* GaussAlgorithm(int win_size, double sigma)
{
	int wincenter, x;
	double *kern, sum = 0.0;
	wincenter = win_size / 2;
	kern = (double*)calloc(win_size, sizeof(double));

	for (x = 0; x < wincenter + 1; x++)
	{
		kern[wincenter - x] = kern[wincenter + x] = Gauss(sigma, x);
		sum += kern[wincenter - x] + ((x != 0) ? kern[wincenter + x] : 0.0);
	}
	for (x = 0; x < win_size; x++)
		kern[x] /= sum;

	return kern;
}
/// <summary>  
/// 函数功能:高斯模糊实现函数
/// 函数返回:模糊后的图像
/// 知识点:模糊窗下的各点的高斯系数(数组)
/// </summary>  
/// <param name="src">待模糊的图像</param>  
/// <param name="sigma">模糊半径(高斯核函数参数)</param>  
/// <remarks> rgb三通道分别处理</remarks> 
bmp_t *GaussBlur(bmp_t *src, double sigma) 
{
	int	row, col, col_r, col_g, col_b, winsize, halfsize, k, count, rows, count1, count2, count3;
	int width, height;
	double  row_g, row_b, row_r, col_all;
	unsigned char  r_r, r_b, r_g, c_all;
	char *tmp;
	double *algorithm;

	count=0;
	width = 3*src->header.biWidth; height = src->header.biHeight;

	if ((width % 4) != 0) width += (4 - (width % 4)); 
	bmp_t *blur;
	blur = (bmp_t*)malloc(sizeof(bmp_t));
	blur->header = src->header;
	blur->header.biWidth = src->header.biWidth;
	blur->header.biHeight = src->header.biHeight;
	blur->header.biSizeImage = width * blur->header.biHeight;
	blur->data = (char*)malloc(blur->header.biSizeImage);

	winsize = win_size(sigma);
	algorithm = GaussAlgorithm(winsize, sigma); 
	winsize *= 3; 
	halfsize = winsize / 2;

	tmp = (char*)calloc(width * height, sizeof(char)); 
	
	for (row = 0; row < height; row++)
	{
		col_r = 0;
		col_g = 1;
		col_b = 2;
		for (rows = 0; rows < width; rows += 3)
		{
			row_r = row_g = row_b = 0.0;
			count1 = count2 = count3 = 0;
		
			for (k = 1; k < winsize; k += 3)
			{
				if ((k + col_r - halfsize >= 0) && (k + col_r - halfsize < width))
				{
					r_r = *(src->data + row * width + col_r + k - halfsize);
					row_r += (int)(r_r)* algorithm[count1];
					count1++;
				}
				if ((k + col_g - halfsize >= 0) && (k + col_g - halfsize < width))
				{
					r_g = *(src->data + row * width + col_g + k - halfsize);
					row_g += (int)(r_g)* algorithm[count2];
					count2++;
				}

				if ((k + col_b - halfsize >= 0) && (k + col_b - halfsize < width))
				{
					r_b = *(src->data + row * width + col_b + k - halfsize);
					row_b += (int)(r_b)* algorithm[count3];
					count3++;
				}
			}

			*(tmp + row * width + col_r) = (unsigned char)(ceil(row_r));
			*(tmp + row * width + col_g) = (unsigned char)(ceil(row_g));
			*(tmp + row * width + col_b) = (unsigned char)(ceil(row_b));
			col_r += 3;
			col_g += 3;
			col_b += 3;
		}
	}
	
	winsize /= 3;
	halfsize = winsize / 2;
	for (col = 0; col < width; col++)
		for (row = 0; row < height; row++)
		{
		col_all = 0.0;
		for (k = 0; k < winsize; k++)
			if ((k + row - halfsize >= 0) && (k + row - halfsize < height))
			{
			c_all = *(tmp + (row + k - halfsize) * width + col);
			col_all += ((int)c_all) * algorithm[k];
			}
		*(blur->data + row * width + col) = (unsigned char)(ceil(col_all));
		}
	
	free(tmp);
	free(algorithm);

	return blur;
}
