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


unsigned char* setup_memory(char *name, int size);
char* openImg(char* filename, img* bmp);
void generateImg(char* imgdata, img* bmp);
void gaussianblur(unsigned char* imgdata, int width, int height, int radius);

int main(int argc, char *argv[]){
	//unsigned char* inputData;
    int i;
    img* bmp = (img*) malloc (IMAGESIZE);
    char* inputImg = "input.bmp";

    int radius = atoi(argv[1]);
    const int SIZE = bmp->width * bmp->height * sizeof(unsigned char);


    char *IMG = "Image";
    unsigned char* inputData = setup_memory(IMG,SIZE);


    inputData = openImg(inputImg, bmp);
    gaussianblur(inputData, bmp->width, bmp->height,radius);
    generateImg(inputData, bmp);

    free(bmp);
    free(inputData);
    return 0;
	/*
    int N = atoi(argv[1]); // Array size (NxN)
	int nStart, nStop; // Starting/stopping indexes for each process
	int i, j, k; // Required for some gcc settings
	const int SIZE = N * N * sizeof(double); // size for the array
	char *nameA = "ArrayA";
	char *nameB = "ArrayB";
	char *nameC = "ArrayC";
	double *A = setup_memory(nameA, SIZE);
	double *B = setup_memory(nameB, SIZE);
	double *C = setup_memory(nameC, SIZE);
	int isFirstProcess = 0; // Need to the 1st process to unlink memory

	// Fill arrays
	for (i = 0; i < N; i++){
	     for (j = 0; j < N; j++){
			A[i*N+j] = i + j;
			B[i*N+j] = i + j;
	     }
	}

	pid_t pid;
	pid = fork(); // Create process 2
	if (pid > 0) { // if process 1
		isFirstProcess = 1; // Need to the 1st process to unlink memory
		nStart = 0;
		nStop = N / 4 - 1;
	}
	else { // for process 2
		pid = fork(); // Create process 3
		if (pid > 0) { // if process 2
			nStart = N / 4;
			nStop = N / 2 - 1;
		}
		else { // for process 3
			pid = fork(); // Create process 4
			if (pid > 0) { // if process 3
				nStart = N / 2;
				nStop = N * 3 / 4 - 1;
			}
			else { // for process 4
				nStart = N * 3 / 4;
				nStop = N - 1;
			}
		}
	}
	printf("nStart=%d, nStop=%d \n", nStart, nStop);

	struct timeval start_time, stop_time, elapsed_time;  // timers
	gettimeofday(&start_time,NULL); // Unix timer
	// Matrix multiply
	// For each row i of C
	for (i = nStart; i <= nStop; i++) {
		// For each column j of C 
		for (j = 0; j < N; j++) {
			// Compute C(i,j) 
			double cij = 0;
			for(k = 0; k < N; k++) {
				cij += A[i*N+k] * B[k*N+j];
			}
			C[i*N+j] = cij;
		}
	}
	gettimeofday(&stop_time,NULL);
	timersub(&stop_time, &start_time, &elapsed_time); // Unix time subtract routine
	wait(NULL);
	printf("%f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);

	wait(NULL); // Wait on child
	// remove the shared memory segments
	if (isFirstProcess) { // Parent
		// Print array (for testing only)
		int TEST_MODE = 0;
		if (TEST_MODE == 1) {
			for (i = 0; i < N; i++) {
				 for (j = 0; j < N; j++){
					printf("%.1f ", C[i*N+j]);
				 }
				 printf("\n");
			}
		}
		// Release memory
		if (shm_unlink(nameA) == -1 | shm_unlink(nameB) == -1 | shm_unlink(nameC) == -1) {
			printf("Error removing memory\n");
			exit(-1);
		}
	}
	*/
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

void gaussianblur(unsigned char* imgdata, int width, int height, int radius) {

	char *nameA = "Red";
	char *nameB = "Green";
	char *nameC = "Yellow";
	char *nameD = "Image";
	const int SIZE = width * height * sizeof(unsigned char);

    unsigned char* red = setup_memory(nameA, SIZE);;
    unsigned char* green = setup_memory(nameB,SIZE);
    unsigned char* blue = setup_memory(nameC,SIZE);

    imgdata = setup_memory(nameD,SIZE);
   	//double *C = setup_memory(nameC, SIZE);
    //red = (unsigned char*) malloc (width*height);
    //green = (unsigned char*) malloc(width*height);
    //blue = (unsigned char*) malloc(width*height);
    int i, j;
    int pos = 0;
    
    int rgb_width =  width * 3 ;
    if ((width * 3  % 4) != 0) {
       rgb_width += (4 - (width * 3 % 4));  
    }

    int nStart;
    int nStop;
    int isFirstProcess = 0;

	pid_t pid;
	pid = fork(); 
	if (pid > 0) { 
		isFirstProcess = 1; 
		nStart = 0;
		nStop = height / 2 - 1;
	}
	else { 
		nStart = height / 2;
		nStop = height;
	}



	for (i = nStart; i < nStop; i++) {
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
        
	pos = 0;
	for (i = nStart; i < nStop; i++ ) {
		for (j = 0; j < width* 3 ; j += 3 , pos++) {
			imgdata[i * rgb_width  + j] = red[pos];
			imgdata[i * rgb_width  + j + 1] = green[pos];
			imgdata[i * rgb_width  + j + 2] = blue[pos];
		}
	}

	

	wait(NULL); 

	if (isFirstProcess == 0) { 
		if (shm_unlink(nameA) == - 1| shm_unlink(nameB) == -1 | shm_unlink(nameC) == -1) {
			printf("Error removing memory\n");
			exit(-1);
		}
		
	}

}