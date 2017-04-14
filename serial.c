#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

char* str = "test.bmp";
char* outstr = "testout.bmp";

// Describe the structure of BMP file
#pragma pack(push, 2)          
    typedef struct {
        uint16_t sign;
        uint32_t size;
        uint32_t notused;
        uint32_t data;
        uint32_t headwidth;
        uint32_t width;
        uint32_t height;
        uint16_t numofplanes;
        uint16_t bitpix;
        uint32_t method;
        uint32_t arraywidth;
        uint32_t horizresol;
        uint32_t vertresol;
        uint32_t colnum;
        uint32_t basecolnum;
    } img;
#pragma pop

char* openImg(char* filename, img* bmp) {
    FILE* f;
    f = fopen(filename, "rb");
    if (f == 0)
    {
        free(bmp);
        printf("Error while reading!");
        exit(1);
    }
    fread(bmp, 54, 1, f);
    if ((bmp->sign != 19778) || (bmp->bitpix != 24) )
    {
        free(bmp);
        printf("File is incorrect!");
        exit(1);
    }
    char* data = (char*) malloc (bmp->arraywidth);
    fseek(f, bmp->data, SEEK_SET);
    fread(data, bmp->arraywidth, 1, f);
    fclose(f);
    return data;
}

void writebmp(char* filename, img* bmp, char* imgdata)
{
    FILE* f;
    f = fopen(filename, "wb");
    fwrite(bmp, 54, 1, f);
    fseek(f, bmp->data, SEEK_SET);
    fwrite(imgdata, bmp->arraywidth, 1, f);
    fclose(f);
}

double transform(int width, int height, char* imgdata)
{
    double start = clock();
    int i,j;
    int newwidth = 4 - ((width * 3) % 4) + (width * 3);
    char temp;
    for (i = 0; i < height / 2; i++)
        for (j = 0; j < newwidth; j++)
        {
             temp = imgdata[i*newwidth+j];
             imgdata[i*newwidth+j] = imgdata[(height-1-i)*newwidth+j];
             imgdata[(height-1-i)*newwidth+j] = temp;
        }
    double finish = clock();
    double elapsed = finish-start;

    return elapsed;
}

#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))

void gaussianblur(int width, int height, unsigned char* imgdata, float radius)
{
	int counter;
    unsigned char* red;
    unsigned char* green;
	unsigned char* blue;
    int trash = 4 - width * 3 % 4;
    int nwidth = trash +  (width * 3);
    int i, j;
	red = (unsigned char*) malloc (width*height);
	green = (unsigned char*) malloc(width*height);
	blue = (unsigned char*) malloc(width*height);
	counter = 0;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < nwidth; j = j + 3)
		if (j < nwidth - trash) {
			red[counter] = imgdata[i * nwidth + j];
			green[counter] = imgdata[i * nwidth + j + 1];
			blue[counter] = imgdata[i * nwidth + j + 2];
			counter++;
		}
	}
    double rs = ceil(radius * 2.57);
    double iy;
    double ix;
        for(i=0; i<height; i++)
                for(j=0; j<width; j++) {
                    double valr = 0;
                    double valg = 0;
                    double valb = 0;
                    double wsum = 0;
                    for(iy = i-rs; iy<i+rs+1; iy++)
                            for(ix = j-rs; ix<j+rs+1; ix++) {
                                int x = min(width-1, max(0, ix));
                                int y = min(height-1, max(0, iy));
                                double dsq = (ix-j)*(ix-j)+(iy-i)*(iy-i);
                                double wght = exp( -dsq / (2*radius*radius) ) / (3.14*2*radius*radius);
                                valr += red[y*width+x] * wght;
                                valg += green[y*width+x] * wght;
                                valb += blue[y*width+x] * wght;
                                wsum += wght;
                             }
                    red[i*width+j] = round(valr/wsum);
                    green[i*width+j] = round(valg/wsum);
                    blue[i*width+j] = round(valb/wsum);
                }
	counter = 0;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < nwidth; j = j + 3)
		{
			if (j < nwidth - trash) {
				imgdata[i * nwidth + j] = red[counter];
				imgdata[i * nwidth + j + 1] = green[counter];
				imgdata[i * nwidth + j + 2] = blue[counter];
				counter++;
			}
		}
	}
	free(red);
	free(green);
	free(blue);
}

int main(int argc, char** argv[]) {
    unsigned char* imgdata;
    int i;
    img* bmp = (img*) malloc (54);

    imgdata = openImg(str, bmp);

    gaussianblur(bmp->width, bmp->height, imgdata, 2);
    writebmp(outstr, bmp, imgdata);
    char* temp;

    free(bmp);
    free(imgdata);
    return 0;
}