//
// Created by Samy Lemcelli on 3/27/15.
//

#include "lab4.h"

using namespace std;

#define		ENABLED_SEQ		0


// Global Variables
std::vector<cl::Platform> all_platforms;
std::vector<cl::Device> all_devices;
cl::Platform platform;
cl::Device device;
std::string kernel_code;

int maxrow = 0;
int maxcol = 0;

double *matrix;
double *newMatrix;

void InitCL()
{
	
	cl::Platform::get(&all_platforms);
	platform = all_platforms[0];
	device = all_devices[0];

	platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

	//Do the checks for the devices and platforms
	if (all_platforms.size() == 0){
		printf("There were no platforms found\n");
		exit(1);
	}
	if (all_devices.size() == 0){
		printf("There were no devices found\n");
		exit(1);
	}

	printf("The device used is %s\n", device.getInfo<CL_DEVICE_NAME>());
}

void InitMatrices(int size)
{
	int row, col, currentRow;
	// Initialise the matrix
	matrix = (double*)calloc(size, sizeof(double));
	for (row = 0; row < maxrow; row++)
	{
		currentRow = row*maxcol;
		for (col = 0; col < maxcol; col++)
		{
			matrix[currentRow + col] = row*(maxrow - row - 1) * col*(maxcol - col - 1);
		}
	}
	newMatrix = (double*)calloc(size, sizeof(double));
	memcpy(newMatrix, matrix, size * sizeof(double));
}

void printMatrix()
{
	int row, col;
	printf("-------------------------------------------------------------------\n");
	for (row = maxrow - 1; row >= 0; row--)
	{
		printf("|");
		for (col = 0; col < maxcol; col++)
		{
			printf("%.2f\t", newMatrix[row*maxcol + col]);
		}
		printf("|\n");
	}
	printf("-------------------------------------------------------------------\n");
}

int sequential()
{
	return 1;
}


int main(int argc, char **argv)
{
	int deltat = 0;
	double td = 0.0;
	double h = 0.0;
	int matrixSize = 0;

	//Initialisation des param
	if (argc != 6)
	{
		printf("Please insert the right number of parameters. You only have %d params.\n", argc);
		return 0;
	}
	maxrow = atoi(argv[1]);
	maxcol = atoi(argv[2]);
	deltat = atoi(argv[3]);
	td = atof(argv[4]);
	h = atof(argv[5]);
	matrixSize = maxrow*maxcol;

	InitMatrices(matrixSize);

	if (ENABLED_SEQ == 1)
	{
		sequential();
	}

	InitCL();
	cl::Context context({ device });
	cl::Program::Sources sources;

	// Get the kernel code from the file
	ifstream filestream;
	filestream.open("main.cl");
	filestream >> kernel_code;
	sources.push_back({ kernel_code.c_str(), kernel_code.length() });

	printf("Kernel code %s",kernel_code);

	// Check if the kernel program compiled.
	cl::Program program(context, sources);
	if (program.build({ device }) != CL_SUCCESS){
		printf("Program build returned the following error %s\n", program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device));
		exit(1);
	}

	/**** Start of our implementation ****/
	double tdh2 = (td / (h*h));
	double invtdh2 = (1.0 - tdh2*4.0);

    //create queue to which we will push commands for the device.
    cl::CommandQueue queue(context, device);

	// Create the buffers on the device
	cl::Buffer matrixBuffer(context, CL_MEM_READ_WRITE, sizeof(double)*matrixSize);
	cl::Buffer newMatrixBuffer(context, CL_MEM_READ_WRITE, sizeof(double)*matrixSize);
    // Write to the buffers to the device
    queue.enqueueWriteBuffer(matrixBuffer,CL_TRUE,0,sizeof(int)*matrixSize,matrix);
	queue.enqueueWriteBuffer(newMatrixBuffer, CL_TRUE, 0, sizeof(int)*matrixSize, newMatrix);

    //Clear way to call the kernel and the parameters
    cl::Kernel kernel_prog=cl::Kernel(program,"pixelCalculation");
	kernel_prog.setArg(0, matrixBuffer);
	kernel_prog.setArg(1, newMatrixBuffer);
	kernel_prog.setArg(2, maxrow);
	kernel_prog.setArg(3, maxcol);
	kernel_prog.setArg(4, deltat);
	kernel_prog.setArg(5, tdh2);
	kernel_prog.setArg(6, invtdh2);
	
	queue.enqueueNDRangeKernel(kernel_prog, cl::NullRange, cl::NDRange(matrixSize), cl::NullRange);
    queue.finish();

    // Final Matrix from the Device
	queue.enqueueReadBuffer(newMatrixBuffer, CL_TRUE, 0, sizeof(double)*matrixSize, newMatrix);

   printf("Final Matrix: \n");
   printMatrix();



	exit(0);

}
