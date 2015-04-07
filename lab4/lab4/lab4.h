//
// Laboratoire sur OpenCL
// Authors: Samy Lemcelli
//

#ifndef LAB4_MAIN_H
#define LAB4_MAIN_H
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS // Should fix this later
#include <CL/cl.hpp>
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
	float sequential(int maxrow, int maxcol, int deltat, double td, double h);
	float parallel(int maxrow, int maxcol, int deltat, double td, double h);
	std::string get_file_contents(const char *filename);
};


#endif //LAB4_MAIN_H
