#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

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

char* openImg(char* filename, img* bmp);
void generateImg(img* bmp, char* imgdata);
void gaussianblur(int width, int height, unsigned char* imgdata, float radius);

int main(int argc, char *argv[]) {
    unsigned char* inputData;
    int i;
    img* bmp = (img*) malloc (60);
    char* inputImg = "input.bmp";
    char* outputImg = "output.bmp";

    int radius = atoi(argv[1]);

    inputData = openImg(inputImg, bmp);

    gaussianblur(bmp->width, bmp->height, inputData, radius);

    generateImg(bmp, inputData);

    free(bmp);
    free(inputData);
    return 0;
}

char* openImg(char* filename, img* bmp) {
    FILE* file;
    file = fopen(filename, "rb");
    if (file == 0) {
        free(bmp);
        printf("Can't open theis file!");
        exit(1);
    }
    fread(bmp, 54, 1, file);
    if ( bmp->sign != 19778){
        free(bmp);
        printf("File is incorrect!");
        exit(1);
    }
    if( bmp->bitpix != 24){
        free(bmp);
        printf("Need 24 bit bmp file!");
        exit(1);
    }
    char* data = (char*) malloc (bmp->arraywidth);
    fseek(file, bmp->data, SEEK_SET);
    fread(data, bmp->arraywidth, 1, file);
    fclose(file);
    return data;
}

void generateImg(img* bmp, char* imgdata) {
    FILE* file;
    time_t now;
    time(&now);

    char fileNameBuffer[32];
    sprintf(fileNameBuffer, "%s.bmp",ctime(&now));



    file = fopen(fileNameBuffer, "wb");
    fwrite(bmp, 54, 1, file);
    fseek(file, bmp->data, SEEK_SET);
    fwrite(imgdata, bmp->arraywidth, 1, file);
    fclose(file);
}



#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))



void gaussianblur(int width, int height, unsigned char* imgdata, float radius) {
    unsigned char* red;
    unsigned char* green;
    unsigned char* blue;




    int trash = 4 - width * 3 % 4;
    int nwidth = trash +  (width * 3);
    int i, j;
	red = (unsigned char*) malloc (width*height);
	green = (unsigned char*) malloc(width*height);
	blue = (unsigned char*) malloc(width*height);
    int pos = 0;


	for (i = 0; i < height; i++) {
		for (j = 0; j < nwidth; j = j + 3)
		if (j < nwidth - trash) {
			red[pos] = imgdata[i * nwidth + j];
			green[pos] = imgdata[i * nwidth + j + 1];
			blue[pos] = imgdata[i * nwidth + j + 2];
			pos++;
		}
	}

    double rs = ceil(radius * 2.57);
    double iy;
    double ix;

    double valr = 0;
    double valg = 0;
    double valb = 0;
    double wsum = 0;
    for(i=0; i<height; i++){
        for(j=0; j<width; j++) {
            valr = 0;
            valg = 0;
            valb = 0;
            wsum = 0;
            for(iy = i-rs; iy<i+rs+1; iy++){
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
            }
            red[i*width+j] = round(valr/wsum);
            green[i*width+j] = round(valg/wsum);
            blue[i*width+j] = round(valb/wsum);
        }
    }
        
	pos = 0;
	for (i = 0; i < height; i++) {
		for (j = 0; j < nwidth; j = j + 3) {
			if (j < nwidth - trash) {
				imgdata[i * nwidth + j] = red[pos];
				imgdata[i * nwidth + j + 1] = green[pos];
				imgdata[i * nwidth + j + 2] = blue[pos];
				pos++;
			}
		}
	}
	free(red);
	free(green);
	free(blue);
}
