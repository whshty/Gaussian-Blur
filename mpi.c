# include <stdio.h>
# include <stdlib.h>
# include <malloc.h>
# include <stdint.h>
# include <time.h>
# include <math.h>
# include <sys/time.h>
# include <omp.h>

#include "mpi.h"

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

char* openImg(int inputFileNumber, img* bmp);
void generateImg(char* imgdata, img* bmp);
int setBoundary(int i , int min , int max);

int main(int argc, char *argv[]){
    unsigned char* imgdata;
    img* bmp = (img*) malloc (IMAGESIZE);
    char *inputImg = "input.bmp";
    int radius = atoi(argv[1]);
    //imgdata  = openImg(inputImg, bmp);
    int inputFileNumber = atoi(argv[2]);  
    imgdata = openImg(inputFileNumber, bmp);
    
    int width = bmp->width;
    int height = bmp->height;
    int SIZE = width * height * sizeof(unsigned char);

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

    unsigned char redBuffer[width*height];
    unsigned char greenBuffer[width*height];
    unsigned char blueBuffer[width*height];

    int my_PE_num;
    int threadNumber;
    int nStart, nStop;

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_PE_num);
    MPI_Comm_size(MPI_COMM_WORLD, &threadNumber);
    int subSize = height / threadNumber;

    nStart = my_PE_num * subSize;
    nStop = (my_PE_num + 1) * subSize;

    if( my_PE_num == 0 ){
        int k, n_proc;
        MPI_Comm_size(MPI_COMM_WORLD, &n_proc);
        for( k = 1 ; k < n_proc; k++ ){
            MPI_Recv(&redBuffer, SIZE, MPI_UNSIGNED_CHAR, k, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Recv(&greenBuffer, SIZE, MPI_UNSIGNED_CHAR, k, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Recv(&blueBuffer, SIZE, MPI_UNSIGNED_CHAR, k, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            
            int otherThreadStart = k * subSize;
            int otherThreadStop = (k+1) * subSize;

            for( i = otherThreadStart; i < otherThreadStop ; i++){
                for( j = 0 ; j < width ; j++ ){
                    red[i*width+j] = redBuffer[i*width+j];
                    green[i*width+j] = greenBuffer[i*width+j];
                    blue[i*width+j] =blueBuffer[i*width+j];
                }
            }

        }
        for( i = nStart ; i < nStop; i++){
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
                red[i*width+j] = round(redSum/weightSum);
                green[i*width+j] = round(greenSum/weightSum);
                blue[i*width+j] = round(blueSum/weightSum);
                redSum = 0;
                greenSum = 0;
                blueSum = 0;
                weightSum = 0;
            }
        }
    }

    else {
        for( i = nStart ; i < nStop; i++){
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
                red[i*width+j] = round(redSum/weightSum);
                green[i*width+j] = round(greenSum/weightSum);
                blue[i*width+j] = round(blueSum/weightSum);
                redSum = 0;
                greenSum = 0;
                blueSum = 0;
                weightSum = 0;
            }
        }
        for( i = nStart ; i < nStop ; i++){
            for( j = 0 ; j < width ;j++){
                redBuffer[i*width+j] = red[i*width+j];
                greenBuffer[i*width+j] = green[i*width+j];
                blueBuffer[i*width+j] = blue[i*width+j];
            }
        }
        
        MPI_Send(&redBuffer, SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&greenBuffer, SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&blueBuffer, SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("%f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);

    if( my_PE_num == 0 ) {
        pos = 0;
        for (i = 0; i < height; i++ ) {
            for (j = 0; j < width* 3 ; j += 3 , pos++) {
                imgdata [i * rgb_width  + j] = red[pos];
                imgdata [i * rgb_width  + j + 1] = green[pos];
                imgdata [i * rgb_width  + j + 2] = blue[pos];
            }
        }
        generateImg(imgdata , bmp);
    }


    
    MPI_Finalize();
    free(red);
    free(green);
    free(blue);
    free(bmp);
    return 0;
}


char* openImg(int inputFileNumber, img* in) {
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
