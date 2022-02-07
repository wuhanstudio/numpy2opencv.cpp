#include <malloc.h>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "npy.hpp"

int main()
{
    std::vector<unsigned long> shape;
    std::vector<double> data;
    bool fortran_order;

    npy::LoadArrayFromNumpy("noises.npy", shape, fortran_order, data);

    std::cout << "shape: ";
    for (size_t i = 0; i<shape.size(); i++)
        std::cout << shape[i] << ", ";
    std::cout << std::endl;

    // std::cout << "fortran order: " << (fortran_order ? "+" : "-");
    // std::cout << std::endl;

    // std::cout << "data: ";
    // for (size_t i = 0; i < data.size(); i++)
    //     std::cout << data[i] << ", ";
    // std::cout << std::endl << std::endl;

    int* d = (int*)malloc(sizeof(int) * data.size());
    for (size_t i = 0; i < data.size(); i++)
        d[i] = int(data[i] * 255);

    // cv::Mat M(cv::Size(shape[0], shape[1]), CV_32SC3, d);
    
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        return 1;
    }

    cv::Mat img;

    while(1) {
        bool bSuccess = cap.read(img);

        // Breaking the while loop at the end of the video
        if (bSuccess == false) 
        {
            std::cout << "Found the end of the video" << std::endl;
            break;
        }

        cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
        cv::resize(img, img, cv::Size(1280, 720), cv::INTER_AREA);

        for(int i = 0; i < img.rows; i++) {
            for(int j = 0; j < img.cols; j++) {
                // get pixel
                cv::Vec3b color = img.at<cv::Vec3b>(i, j);
                int* color1 = &d[i * img.cols * 3 + j * 3 ];
                // cv::Vec<int, 3> color1 = M.at<cv::Vec<int, 3>>(i, j);

                if ((color1[0] + color1[1] + color1[2]) != 0)
                {
                    // std::cout << i << ", " << j << std::endl;
                    for (size_t k = 0; k < 3; k++)
                    {
                        if (color1[k] < 0) {
                            if (int(color[k]) <= int((-color1[k])))
                                color[k] = 0;
                            else
                                color[k] += color1[k];
                        }
                        else {
                            if ( (255 - color[k]) <= color1[k])
                                color[k] = 255;
                            else
                                color[k] += color1[k];
                        }
                    }

                    // set pixel
                    color = cv::Vec<uchar, 3>(color);  
                    img.at<cv::Vec3b>(i, j) = color;
                }
            }
        }

        cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
        cv::imshow("Display window", img);
        cv::waitKey(1); // Wait for a keystroke in the window
    }

    return 0;
}
