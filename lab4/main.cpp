//
// Created by Samy Lemcelli on 3/27/15.
//

#include "main.h"

// Global Variables
int maxrow, maxcol, deltat;

int main(int argc, char* argv[])
{
    /**** START OF OpenCL Checks ****/
    //Do the checks for the devices and platforms
    std::vector<cl::Platform> all_platforms;
    std::vector<cl::Device> all_devices;
    cl::Platform::get(&all_platforms);
    cl::Platform platform =all_platforms[0];
    cl::Device device =all_devices[0];

    platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

    if(all_platforms.size()==0){
        printf("There were no platforms found\n)");
        return 1;
    }
    if(all_devices.size()==0){
        printf("There were no devices found\n)");
        return 1;
    }

    printf("The device used is %s\n", device.getInfo<CL_DEVICE_NAME>());

    cl::Context context({device});
    cl::Program::Sources sources;

    // Get the kernel code from the file
    std::string kernel_code;
    ifstream filestream;
    filestream.open("main.cl");
    filestream >> kernel_code;
    sources.push_back({kernel_code.c_str(),kernel_code.length()});

    cl::Program program(context,sources);
    if(program.build({device})!=CL_SUCCESS){
        printf("Program build returned the following error %s\n",program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device));
        exit(1);
    }
    /**** END OF OpenCL Checks ****/

    /**** Start of our implementation ****/
    int matrixSize;
    double td, h;

    //Initialisation des paramètres
    if(argc != 6)
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

    // create buffers on the device
    double matrix[];
    double newMatrix[];
    cl::Buffer matrixBuffer(context,CL_MEM_READ_WRITE,sizeof(double)*10);
    cl::Buffer newMatrixBuffer(context,CL_MEM_READ_WRITE,sizeof(double)*10);

    int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int B[] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0};

    //create queue to which we will push commands for the device.
    cl::CommandQueue queue(context, device);

    //write arrays A and B to the device
    queue.enqueueWriteBuffer(matrixBuffer,CL_TRUE,0,sizeof(int)*10,A);
    queue.enqueueWriteBuffer(newMatrixBuffer,CL_TRUE,0,sizeof(int)*10,B);


    //run the kernel
    cl::KernelFunctor simple_add(cl::Kernel(program,"simple_add"),queue,cl::NullRange,cl::NDRange(10),cl::NullRange);
    simple_add(matrixBuffer, newMatrixBuffer,buffer_C);

    //alternative way to run the kernel
    /*cl::Kernel kernel_add=cl::Kernel(program,"simple_add");
    kernel_add.setArg(0,matrixBuffer);
    kernel_add.setArg(1,buffer_B);
    kernel_add.setArg(2,buffer_C);
    queue.enqueueNDRangeKernel(kernel_add,cl::NullRange,cl::NDRange(10),cl::NullRange);
    queue.finish();*/

    int C[10];
    //read result C from the device to array C
    queue.enqueueReadBuffer(buffer_C,CL_TRUE,0,sizeof(int)*10,C);

    std::cout<<" result: \n";
    for(int i=0;i<10;i++){
        std::cout<<C[i]<<" ";
    }

    return 0;

}
