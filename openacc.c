# include <stdio.h>
# include <stdlib.h>
# include <malloc.h>
# include <stdint.h>
# include <time.h>
# include <math.h>
# include <sys/time.h>
# include <omp.h>

# define IMAGESIZE 54

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

//unsigned char* openImg(int inputFileNumber, img* in);
void generateImg(unsigned char* imgdata, img* bmp);
#pragma acc routine worker
int setBoundary(int i , int min , int max);

int main(int argc, char *argv[]){
    int radius = atoi(argv[1]);
    int inputFileNumber = atoi(argv[2]);
    unsigned char* imgdata;
    img* bmp = (img*) malloc (IMAGESIZE);
    //imgdata = openImg(inputFileNumber, bmp);

    char inPutFileNameBuffer[32];
    sprintf(inPutFileNameBuffer, "%d.bmp",inputFileNumber);
    FILE* file;
    if (!(file = fopen(inPutFileNameBuffer, "rb"))) {
        printf("File not found!");
        free(bmp);
        exit(1);
    }
    fread(bmp, 54, 1, file);
    if( bmp->bitpix != 24){
        free(bmp);
        printf("Need 24 bit bmp file!");
        exit(1);
    }
    
    unsigned char* data = (unsigned char*) malloc (bmp->arraywidth);
    fseek(file, bmp->data, SEEK_SET);
    fread(data, bmp->arraywidth, 1, file);
    fclose(file);
    imgdata = data;


    int width = bmp->width;
    int height = bmp->height;

    int i, j;       
    int rgb_width =  width * 3 ;
    if ((width * 3  % 4) != 0) {
       rgb_width += (4 - (width * 3 % 4));  
    }

    unsigned char* red;
    unsigned char* green;
    unsigned char* blue;
    red = (unsigned char*) malloc (width*height);
    green = (unsigned char*) malloc(width*height);
    blue = (unsigned char*) malloc(width*height);

    int pos = 0; 
    for (i = 0; i < height; i++) {
        for (j = 0; j < width * 3; j += 3, pos++){
            red[pos] = imgdata[i * rgb_width + j];
            green[pos] = imgdata[i * rgb_width + j + 1];
            blue[pos] = imgdata[i * rgb_width + j + 2];
        }
    }

    struct timeval start_time, stop_time, elapsed_time; 
    gettimeofday(&start_time,NULL);
    
    #pragma acc data copy(red[0:width*height]),copy(green[0:width*height]),copy(blue[0:width*height])
    for( i = 0 ; i < height; i++){
        for(j = 0 ; j < width ; j++) {
            double row;
            double col;
            double redSum = 0;
            double greenSum = 0;
            double blueSum = 0;
            double weightSum = 0;
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
            red[i*width+j] = (redSum/weightSum);
            green[i*width+j] = (greenSum/weightSum);
            blue[i*width+j] = (blueSum/weightSum);
            redSum = 0;
            greenSum = 0;
            blueSum = 0;
            weightSum = 0;
        }
    }
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("%f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);

    pos = 0;
    for (i = 0; i < height; i++ ) {
        for (j = 0; j < width* 3 ; j += 3 , pos++) {
            imgdata [i * rgb_width  + j] = red[pos];
            imgdata [i * rgb_width  + j + 1] = green[pos];
            imgdata [i * rgb_width  + j + 2] = blue[pos];
        }
    }

    generateImg(imgdata, bmp);
    free(red);
    free(green);
    free(blue);
    free(bmp);
    return 0;
}

unsigned char* openImg(int inputFileNumber, img* in) {
    char inPutFileNameBuffer[32];
    sprintf(inPutFileNameBuffer, "%d.bmp",inputFileNumber);
    FILE* file;
    if (!(file = fopen(inPutFileNameBuffer, "rb"))) {
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
    unsigned char* data = (unsigned char*) malloc (in->arraywidth);
    fseek(file, in->data, SEEK_SET);
    fread(data, in->arraywidth, 1, file);
    fclose(file);
    return data;
}

void generateImg(unsigned char* imgdata , img* out) {
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

#pragma acc routine worker
int setBoundary(int i , int min , int max){
    if( i < min) return min;
    else if( i > max ) return max;
    return i;  
}
