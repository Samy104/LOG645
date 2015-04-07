//
// Created by secksx on 3/27/15.
//

#ifndef LAB4_MAIN_H
#define LAB4_MAIN_H
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS // Should fix this later
#include <CL/cl.hpp>
#include <stdio.h>
#include <fstream>

class lab4 {
public:
	int main(int argc, char **argv);
private:
	// Global Variables
	int maxrow;
	int maxcol;
	int deltat;
	double *matrix;
	double *newMatrix;

	std::vector<cl::Platform> all_platforms;
	std::vector<cl::Device> all_devices;
	cl::Platform platform;
	cl::Device device;
	std::string kernel_code;
	// Functions
	void InitCL();
	void InitMatrices(double size);
	void printMatrix();
	int sequential();
};


#endif //LAB4_MAIN_H
