#pragma once

#include "ITask.hpp"

#include <opencv2/opencv.hpp>

#include <string>

namespace Image
{

class Matrix
{
public:
    Matrix(std::string img) : image(cv::imread(img.c_str(), 1))
    {
    }

    void show()
    {
        if (image.data)
        {
            cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
            imshow("Display Image", image);

            cv::waitKey(0);
        }
    }

private:
    cv::Mat image;
};

class LoadFromRes : public ITask
{
public:
    LoadFromRes(std::string res) : m(res)
    {
    }

    ElementType execute()
    {
        m.show();

        return {};
    }

private:
    Matrix m;
};

}
