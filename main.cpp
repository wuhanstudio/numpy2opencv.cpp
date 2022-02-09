#include <malloc.h>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "npy.hpp"

double clockToMilliseconds(clock_t ticks){
    // units/(units/time) => time (seconds) * 1000 = milliseconds
    return (ticks/(double)CLOCKS_PER_SEC)*1000.0;
}

clock_t deltaTime = 0;
unsigned int frames = 0;
double  frameRate = 30;
double  averageFrameTimeMilliseconds = 33.333;

int main(int argc, char* argv[])
{
    std::vector<unsigned long> shape;
    std::vector<double> data;
    bool fortran_order;

    if(argc < 2) 
    {
        std::cout << "Usage: " << argv[0] << " <noise_file_720p>" << std::endl;
        exit(1);
    }

    npy::LoadArrayFromNumpy(argv[1], shape, fortran_order, data);

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

        int width = img.cols;
        int height = img.rows;

        cv::resize(img, img, cv::Size(shape[0], shape[1]), cv::INTER_AREA);

        // cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

        clock_t beginFrame = clock();

        for(int i = 0; i < img.rows; i++) {
            for(int j = 0; j < img.cols; j++) {
                // get pixel
                cv::Vec3b& color = img.at<cv::Vec3b>(i, j);
                int* color1 = &d[i * img.cols * 3 + j * 3 ];

                uint8_t temp; 

                temp = color[0];
                color[0] += color1[2];
                if ((color1[2] < 0) && (color[0] > temp))
                    color[0] = 0;
                if ((color1[2] > 0) && (color[0] < temp))
                    color[0] = 255;

                temp = color[1];
                color[1] += color1[1];
                if ((color1[1] < 0) && (color[1] > temp))
                    color[1] = 0;
                if ((color1[1] > 0) && (color[1] < temp))
                    color[1] = 255;

                temp = color[2];
                color[2] += color1[0];
                if ((color1[0] < 0) && (color[2] > temp))
                    color[2] = 0;
                if ((color1[0] > 0) && (color[2] < temp))
                    color[2] = 255;
            }
        }

        // for(int i = 0; i < img.rows; i++) {
        //     for(int j = 0; j < img.cols; j++) {
        //         // get pixel
        //         cv::Vec3b& color = img.at<cv::Vec3b>(i, j);
        //         int* color1 = &d[i * img.cols * 3 + j * 3 ];
        //         // cv::Vec<int, 3> color1 = M.at<cv::Vec<int, 3>>(i, j);

        //         if ((color1[0] + color1[1] + color1[2]) != 0)
        //         {
        //             // std::cout << i << ", " << j << std::endl;
        //             for (size_t k = 0; k < 3; k++)
        //             {
        //                 if (color1[k] < 0) {
        //                     if (int(color[2-k]) <= int((-color1[k])))
        //                         color[2-k] = 0;
        //                     else
        //                         color[2-k] += color1[k];
        //                 }
        //                 else {
        //                     if ( (255 - color[2-k]) <= color1[k])
        //                         color[2-k] = 255;
        //                     else
        //                         color[2-k] += color1[k];
        //                 }
        //             }
        //         }
        //     }
        // }

        cv::resize(img, img, cv::Size(width, height), cv::INTER_AREA);

        clock_t endFrame = clock();

        deltaTime += endFrame - beginFrame;
        frames ++;

        // if you really want FPS
        if( clockToMilliseconds(deltaTime)>1000.0) { 
            // every second
            frameRate = (double)frames*0.5 + frameRate*0.5; //more stable
            frames = 0;
            deltaTime -= CLOCKS_PER_SEC;
            averageFrameTimeMilliseconds  = 1000.0/(frameRate==0?0.001:frameRate);

            std::cout<<"CPU time was:"<<averageFrameTimeMilliseconds<<std::endl;
        }

        // cv::cvtColor(img, img, cv::COLOR_RGB2BGR);

        cv::imshow("Display window", img);
        cv::waitKey(1); // Wait for a keystroke in the window
    }

    return 0;
}
