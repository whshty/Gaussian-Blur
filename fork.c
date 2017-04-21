# include <stdio.h>
# include <sys/time.h>
# include <stdlib.h> // For atoi and exit
# include <unistd.h> // For fork
# include <fcntl.h> // For shm_open flags
# include <sys/mman.h> // For shm_open and mmap
# include <sys/wait.h> // For wait
# include <malloc.h>
# include <stdint.h>
# include <time.h>
# include <math.h>


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


unsigned char* setup_memory(char *name, int size);
char* openImg(char* filename, img* bmp);
void generateImg(char* imgdata, img* bmp);
int setBoundary(int i , int min , int max);

int main(int argc, char *argv[]){
    img* bmp = (img*) malloc (IMAGESIZE);
    
    char *inputImg = "input.bmp";
    int radius = atoi(argv[1]);

    char *nameIn = "imageIn";
    char *nameOut = "imageOut";
    unsigned char* imgdata = setup_memory(nameIn,IMAGESIZE);
    imgdata = openImg(inputImg, bmp);
    int width = bmp->width;
    int height = bmp->height;


    char *nameRed = "colorRed";
    char *nameGreen = "colorGreen";
    char *nameBlue = "colorBlue";
    int SIZE = width * height * sizeof(unsigned char);
    unsigned char* red = setup_memory(nameRed,SIZE);
    unsigned char* green = setup_memory(nameGreen,SIZE);
    unsigned char* blue = setup_memory(nameBlue,SIZE);
    
    int i, j;       
    int rgb_width =  width * 3 ;
    if ((width * 3  % 4) != 0) {
       rgb_width += (4 - (width * 3 % 4));  
    }

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

    int nStart;
    int nStop;
    int isFirstProcess = 0;

    pid_t pid;
    pid = fork(); 

    if (pid > 0) { 
        isFirstProcess = 1; 
        nStart = 0;
        nStop = height / 2;
    }
    else { 
        nStart = height / 2;
        nStop = height;
    }


    double row;
    double col;
    double redSum = 0;
    double greenSum = 0;
    double blueSum = 0;
    double weightSum = 0;

    for( i = nStart ; i < nStop; i++){
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
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("%f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);

	pos = 0;
	for (i = 0; i < height; i++ ) {
		for (j = 0; j < width* 3 ; j += 3 , pos++) {
			imgdata[i * rgb_width  + j] = red[pos];
			imgdata[i * rgb_width  + j + 1] = green[pos];
			imgdata[i * rgb_width  + j + 2] = blue[pos];
		}
	}

    if( isFirstProcess == 1 ){
        wait(NULL);
        generateImg(imgdata, bmp);
    }
    if( isFirstProcess == 0){
        return EXIT_SUCCESS;
    }
	if (isFirstProcess) { 
		if (shm_unlink(nameIn) == -1 |  shm_unlink(nameRed) == -1 | shm_unlink(nameGreen) == -1 | shm_unlink(nameBlue) == -1) {
			printf("Error removing memory\n");
			exit(-1);
		}		
	}
    free(bmp);
    return 0;
}


unsigned char* setup_memory(char *name, int size) {
	int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, size);
	unsigned char *m = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (m == MAP_FAILED) {
		printf("Map failed\n");
		exit(-1);
	}
	return m;
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

