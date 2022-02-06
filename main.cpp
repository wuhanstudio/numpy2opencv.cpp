#include <malloc.h>
#include "npy.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

int main()
{
    std::vector<unsigned long> shape;
    bool fortran_order;
    std::vector<double> data;

    npy::LoadArrayFromNumpy("noise.npy", shape, fortran_order, data);
    std::cout << "shape: ";
    for (size_t i = 0; i<shape.size(); i++)
        std::cout << shape[i] << ", ";
    std::cout << std::endl;

    uint8_t* d = (uint8_t*)malloc(sizeof(uint8_t) * data.size());
    for (size_t i = 0; i<data.size(); i++)
        d[i] = int(data[i] * 255);

    cv::Mat M(cv::Size(shape[1], shape[0]), CV_8UC3, d);

    // std::cout << "fortran order: " << (fortran_order ? "+" : "-");
    // std::cout << std::endl;

    // std::cout << "data: ";
    // for (size_t i = 0; i<data.size(); i++)
    //     std::cout << data[i] << ", ";
    // std::cout << std::endl << std::endl;

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        return 1;
    }

    while(1) {
        cv::Mat img;
        bool bSuccess = cap.read(img); // read a new frame from video 

        // Breaking the while loop at the end of the video
        if (bSuccess == false) 
        {
            std::cout << "Found the end of the video" << std::endl;
            break;
        }
        cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
        
	// cv::Mat N(cv::Size(img.cols, img.rows), CV_8UC3, cv::Scalar(0));
        // M.copyTo( N(cv::Rect( N.cols / 2 - shape[1] / 2, N.rows / 2 - shape[0] / 2, shape[1], shape[0] )) );
        
	cv::resize(img, img, cv::Size(M.rows, M.cols), cv::INTER_AREA);

	img = img + M;

        cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
        cv::imshow("Display window", img);
        cv::waitKey(1); // Wait for a keystroke in the window
    }

    return 0;
}

