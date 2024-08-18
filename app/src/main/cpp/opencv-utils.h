#pragma once

#include <opencv2/core.hpp>

using namespace cv;

void myFlip(Mat src);
void myBlur(Mat src, float sigma);
void addRectangles(Mat src);
