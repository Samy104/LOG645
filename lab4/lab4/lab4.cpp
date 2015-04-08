//
// Laboratoire sur OpenCL
// Authors: Samy Lemcelli
//

#include "lab4.h"

using namespace std;

#define		ENABLED_SEQ		1
#define		ENABLED_PAR		1


// Global Variables
std::vector<cl::Platform> all_platforms;
std::vector<cl::Device> all_devices;
cl::Platform platform;
cl::Device device;
std::string kernel_code;
cl_uint status;

int maxrow = 0;
int maxcol = 0;

double *matrix;
double *newMatrix;

void InitCL()
{
	
	cl::Platform::get(&all_platforms);
	platform = all_platforms[0];
	platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
	device = all_devices[0];

	platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

	//Do the checks for the devices and platforms
	if (all_platforms.size() == 0){
		printf("There were no platforms found\n");
	}
	if (all_devices.size() == 0){
		printf("There were no devices found\n");
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

//////////////////////////////////////////////////////////////////////////////
//! Loads a Program file and prepends the cPreamble to the code.
//!
//! @return the source string if succeeded, 0 otherwise
//! @param cFilename program filename
//! @param cPreamble code that is prepended to the loaded file, typically a set of
//! @param szFinalLength returned length of the code string
//////////////////////////////////////////////////////////////////////////////
char* oclLoadProgSource(const char* cFilename, const char* cPreamble, size_t*
	szFinalLength)
{
	// locals
	FILE* pFileStream = NULL;
	size_t szSourceLength;
	// open the OpenCL source code file
	if (fopen_s(&pFileStream, cFilename, "rb") != 0)
	{
		return NULL;
	}
	size_t szPreambleLength = strlen(cPreamble);
	// get the length of the source code
	fseek(pFileStream, 0, SEEK_END);
	szSourceLength = ftell(pFileStream);
	fseek(pFileStream, 0, SEEK_SET);
	// allocate a buffer for the source code string and read it in
	char* cSourceString = (char *)malloc(szSourceLength + szPreambleLength + 1);
	memcpy(cSourceString, cPreamble, szPreambleLength);
	if (fread((cSourceString)+szPreambleLength, szSourceLength, 1, pFileStream) != 1)
	{
		fclose(pFileStream);
		free(cSourceString);
		return 0;
	}
	// close the file and return the total length of the combined (preamble + source)string
	fclose(pFileStream);
	if (szFinalLength != 0)
	{
		*szFinalLength = szSourceLength + szPreambleLength;
	}
	cSourceString[szSourceLength + szPreambleLength] = '\0';
	return cSourceString;
}

float sequential(int maxrow, int maxcol, int deltat, double td, double h)
{
	typedef std::chrono::high_resolution_clock Clock;
	printf("\nStarting the sequential code: \n");
	auto timeStart = Clock::now();

	int alteration, 
		currentRow, prevRow, nextRow,
		row, col;
	int matrixSize = maxrow*maxcol;

	double tdh2 = (td / (h*h));
	double invtdh2 = (1.0 - tdh2*4.0);
	// Start the alteration
	for (alteration = 0; alteration < deltat; alteration++)
	{
		// Start calulations
		for (row = 1; row < (maxrow - 1); row++)
		{
			prevRow = (row - 1)*maxcol;
			currentRow = row*maxcol;
			nextRow = (row + 1)*maxcol;

			for (col = 1; col < (maxcol - 1); col++)
			{
				newMatrix[currentRow + col] = invtdh2*matrix[currentRow + col] + tdh2 * (matrix[prevRow + col] + matrix[nextRow + col] + matrix[currentRow + col - 1] + matrix[currentRow + col + 1]);
			}
		}
		memcpy(matrix, newMatrix, matrixSize * sizeof(double));
	}

	printf("Final Matrix Sequential: \n");
	//printMatrix();
	auto timeEnd = Clock::now();
	float duration = (float)std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count();
	printf("Ending the sequential code.\nEnded in %.2f ms \n", duration);
	return duration;
}

float parallel(int maxrow, int maxcol, int deltat, double td, double h)
{
	printf("\nStarting the parralel code: \n");
	InitCL();
	cl::Context context({ device });
	cl::Program::Sources sources;

	// Get the kernel code from the file
	size_t kernelCodeSize = 0;
	kernel_code = oclLoadProgSource("main.cl", "", &kernelCodeSize);
	sources.push_back({ kernel_code.c_str(), kernel_code.length() });
	//printf("Kernel code %s\n", kernel_code);

	// Check if the kernel program compiled.
	cl::Program program(context, sources);
	if (program.build({ device }) != CL_SUCCESS){
		printf("Program build returned the following error %s\n", program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device));
		return 0;
	}

	/**** Start of our implementation ****/
	typedef std::chrono::high_resolution_clock Clock;
	auto timeStart = Clock::now();

	double tdh2 = (td / (h*h));
	double invtdh2 = (1.0 - tdh2*4.0);
	int matrixSize = maxrow*maxcol;

	//create queue to which we will push commands for the device.
	cl::CommandQueue queue(context, device);

	// Create the buffers on the device
	cl::Buffer matrixBuffer(context, CL_MEM_READ_WRITE, sizeof(double)*matrixSize);
	cl::Buffer newMatrixBuffer(context, CL_MEM_READ_WRITE, sizeof(double)*matrixSize);
	cl::Buffer booleanSwitchBuffer(context, CL_MEM_READ_WRITE, sizeof(int));
	// Write to the buffers to the device
	queue.enqueueWriteBuffer(matrixBuffer, CL_TRUE, 0, sizeof(double)*matrixSize, matrix);
	queue.enqueueWriteBuffer(newMatrixBuffer, CL_TRUE, 0, sizeof(double)*matrixSize, newMatrix);

	//Clear way to call the kernel and the parameters
	cl::Kernel kernel_prog = cl::Kernel(program, "pixelCalculation");
	kernel_prog.setArg(0, matrixBuffer);
	kernel_prog.setArg(1, newMatrixBuffer);
	kernel_prog.setArg(2, maxrow);
	kernel_prog.setArg(3, maxcol);
	kernel_prog.setArg(4, deltat);
	kernel_prog.setArg(5, tdh2);
	kernel_prog.setArg(6, invtdh2);
	kernel_prog.setArg(7, booleanSwitchBuffer);
	int alteration = 0;
	int modulo = 0; // Used to alterate between buffers to save writes
	for (alteration = 0; alteration < deltat; alteration++)
	{
		modulo = alteration % 2;
		queue.enqueueWriteBuffer(booleanSwitchBuffer, CL_TRUE, 0, sizeof(int), &modulo);
		queue.enqueueNDRangeKernel(kernel_prog, cl::NullRange, cl::NDRange(matrixSize), cl::NullRange);
	}

	if (modulo == 0)
	{
		queue.enqueueReadBuffer(newMatrixBuffer, CL_TRUE, 0, sizeof(double)*matrixSize, newMatrix);
	}
	else
	{
		queue.enqueueReadBuffer(matrixBuffer, CL_TRUE, 0, sizeof(double)*matrixSize, newMatrix);
	}
	
	queue.finish();

	// Final Matrix from the Device
	

	printf("Final Matrix Parrallel: \n");
	//printMatrix();

	auto timeEnd = Clock::now();
	float duration = (float)std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count();
	printf("Ending the parralel code. \nEnded in %.2f ms \n", duration);

	return duration;
}


int main(int argc, char **argv)
{
	int deltat = 0;
	double td = 0.0;
	double h = 0.0;
	int matrixSize = 0;
	double acceleration = 0.0;

	//Initialisation des param
	if (argc != 6)
	{
		printf("Please insert the right number of parameters. You only have %d params.\n", argc);
		std::cin >> deltat;
		return 0;
	}

	maxrow = atoi(argv[1]);
	maxcol = atoi(argv[2]);
	deltat = atoi(argv[3]);
	td = atof(argv[4]);
	h = atof(argv[5]);
	matrixSize = maxrow*maxcol;

	InitMatrices(matrixSize);

	printf("Initial Matrix\n");
	//printMatrix();

	if (ENABLED_SEQ == 1)
	{
		acceleration = sequential(maxrow, maxcol, deltat, td, h);
	}

	if (ENABLED_PAR == 1)
	{
		InitMatrices(matrixSize); // Reinitialize the matrices since the sequential part modified them.
		acceleration = acceleration / parallel(maxrow, maxcol, deltat, td, h);
	}

	// Acceleration = TSequential / TParallel
	printf("The acceleration is %f \n", acceleration);

	std::cin >> deltat;

	// Free the memory
	free(matrix);
	free(newMatrix);

	// Program done. Can we have 100% on this lab ;)
	return 1;
}
