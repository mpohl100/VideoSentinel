#pragma once

#include <opencv2/core.hpp>
#include <vector>

using namespace cv;

void myFlip(Mat src);
void myBlur(Mat src, float sigma);
void addRectangles(Mat src);

struct Rectangle{
    int x = 0;
    int y = 0;
    int width = 1;
    int height = 1;
};

void create_preview();
void drop_preview();
bool shall_frame_be_posted();
void set_frame(Mat src);
bool are_new_rectangles_available();
std::vector<Rectangle> get_rectangles();
