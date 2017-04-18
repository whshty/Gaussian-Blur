# include <stdio.h>
# include <stdlib.h>
# include <malloc.h>
# include <stdint.h>
# include <time.h>
# include <math.h>

# define IMAGESIZE 60

# pragma pack(push, 2)          
    typedef struct {
        char sign;
        int size;
        int notused;
        int data;
        int headwidth;
        int width;
        int height;
        short numofplanes;
        short bitpix;
        int method;
        int arraywidth;
        int horizresol;
        int vertresol;
        int colnum;
        int basecolnum;
    } img;
# pragma pop

char* openImg(char* filename, img* bmp);
void generateImg(char* imgdata, img* bmp);
void gaussianblur(unsigned char* imgdata, int width, int height, int radius);

int main(int argc, char *argv[]) {
    unsigned char* inputData;
    int i;
    img* bmp = (img*) malloc (IMAGESIZE);
    char* inputImg = "input.bmp";

    int radius = atoi(argv[1]);
    inputData = openImg(inputImg, bmp);
    gaussianblur(inputData, bmp->width, bmp->height,radius);
    generateImg(inputData, bmp);

    free(bmp);
    free(inputData);
    return 0;
}

char* openImg(char* filename, img* in) {
    FILE* file;
    if (!(file = fopen(filename, "rb"))) {
        printf("File not found!");
        free(in);
        exit(1);
    }
    fread(in, 54, 1, file);
    if( in->bitpix != 24){
        free(in);
        printf("Need 24 bit bmp file!");
        exit(1);
    }
    char* data = (char*) malloc (in->arraywidth);
    fseek(file, in->data, SEEK_SET);
    fread(data, in->arraywidth, 1, file);
    fclose(file);
    return data;
}

void generateImg(char* imgdata , img* out) {
    FILE* file;
    time_t now;
    time(&now);
    char fileNameBuffer[32];
    sprintf(fileNameBuffer, "%s.bmp",ctime(&now));
    file = fopen(fileNameBuffer, "wb");
    fwrite(out, IMAGESIZE, 1, file);
    fseek(file, out->data, SEEK_SET);
    fwrite(imgdata, out->arraywidth, 1, file);
    fclose(file);
}



int setBoundary(int i , int min , int max){
    if( i < min) return min;
    else if( i > max ) return max;
    return i;  
}

void gaussianblur(unsigned char* imgdata, int width, int height, int radius) {
    char* red;
    char* green;
    char* blue;
    red = (unsigned char*) malloc (width*height);
    green = (unsigned char*) malloc(width*height);
    blue = (unsigned char*) malloc(width*height);
    int i, j;
    int pos = 0;
    
    int rgb_width =  width * 3 ;
    if ((width * 3  % 4) != 0) {
       rgb_width += (4 - (width * 3 % 4));  
    }


	for (i = 0; i < height; i++) {
		for (j = 0; j < width * 3; j += 3, pos++){
            red[pos] = imgdata[i * rgb_width + j];
            green[pos] = imgdata[i * rgb_width + j + 1];
            blue[pos] = imgdata[i * rgb_width + j + 2];
            
        }
	}

    double row;
    double col;
    double redSum = 0;
    double greenSum = 0;
    double blueSum = 0;
    double weightSum = 0;

    for( i = 0 ; i < height; i++){
        for(j = 0 ; j < width ; j++) {

            for(row = i-radius; row <= i + radius; row++){
                for(col = j-radius; col<= j + radius; col++) {
                    int x = setBoundary(col,0,width-1);
                    int y = setBoundary(row,0,height-1);
                    int tempPos = y * width + x;

                    double square = (col-j)*(col-j)+(row-i)*(row-i);
                    double sigma = radius*radius;
                    double weight = exp(-square / (2*sigma)) / (3.14*2*sigma);

                    redSum += red[tempPos] * weight;
                    greenSum += green[tempPos] * weight;
                    blueSum += blue[tempPos] * weight;
                    weightSum += weight;
                }    
            }
            red[i*width+j] = round(redSum/weightSum);
            green[i*width+j] = round(greenSum/weightSum);
            blue[i*width+j] = round(blueSum/weightSum);

            redSum = 0;
            greenSum = 0;
            blueSum = 0;
            weightSum = 0;
        }
    }
        
	pos = 0;
	for (i = 0; i < height; i++ ) {
		for (j = 0; j < width* 3 ; j += 3 , pos++) {
			imgdata[i * rgb_width  + j] = red[pos];
			imgdata[i * rgb_width  + j + 1] = green[pos];
			imgdata[i * rgb_width  + j + 2] = blue[pos];
		}
	}
	free(red);
	free(green);
	free(blue);
}
