#include <iostream>
#include <vector>
#include <getopt.h>

#include <opencv2/opencv.hpp>

#include "inference.h"
#include "driver.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    std::string projectBasePath = "/home/wdxm/code/yolov8_CPP_Inference_OpenCV_ONNX";
    bool runOnGPU = true;

    Inference inf(projectBasePath + "/source/models/yolov8s.onnx", cv::Size(640, 480),
                  projectBasePath + "/source/classes/classes.txt", runOnGPU);

    // inf.base_exam(projectBasePath);

    Driver driver;
    int pci_driver_fd = driver.open_pci_driver();
    if (pci_driver_fd)
    {
        cout << "PCIe Device Open Success !!!" << endl;
        close(pci_driver_fd);
    }
    else
    {
        cout << "PCIe Device Open Fail !!!" << endl;
    }
    cout << "Exit main !!!" << endl;
    return 0;
}
