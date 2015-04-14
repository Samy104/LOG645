//
// Laboratoire sur OpenCL
// Authors: Samy Lemcelli
//

#ifndef LAB4_MAIN_H
#define LAB4_MAIN_H
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS // Only used for AMD!
#include <CL/cl.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cerrno>

class lab4 {
public:
	int main(int argc, char **argv);
private:
	// Global Variables
	int maxrow;
	int maxcol;
	int deltat;
	float *matrix;
	float *newMatrix;
	cl_device_id *device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;
	cl_platform_id platform_id = NULL;
	cl_uint deviceNum = 0;
	cl_uint platformNum = 0;
	cl_int status = 0;
	// Functions
	void InitCL();
	void InitMatrices(float size);
	void printMatrix();
	float sequential(int maxrow, int maxcol, int deltat, float td, float h);
	float parallel(int maxrow, int maxcol, int deltat, float td, float h);
	char* oclLoadProgSource(const char* cFilename, const char* cPreamble, size_t*
		szFinalLength);
};


#endif //LAB4_MAIN_H
