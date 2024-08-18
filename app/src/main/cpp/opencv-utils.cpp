#include "opencv-utils.h"
#include "video-sentinel/webcam/webcam.h"
#include "video-sentinel/par/parallel.h"
#include "video-sentinel/detection/Rectangle.h"

#include <opencv2/imgproc.hpp>



void myFlip(Mat src) {
    flip(src, src, 0);
}

void myBlur(Mat src, float sigma) {
    GaussianBlur(src, src, Size(), sigma);
}

static webcam::FrameData frameData = {};
static par::Task nextRectangleTask = {};
static par::Executor executor = par::Executor{1};


void addRectangles(Mat src){
    const auto surrounding_rectangle = od::Rectangle{0, 0, src.cols, src.rows};
    int rings = 1;
    int gradient_threshold = 20;

    frameData = webcam::FrameData{src};
    auto flow = webcam::process_frame(frameData, src, surrounding_rectangle, rings, gradient_threshold);
    executor.run(flow);
    executor.wait_for(flow);
    
    // draw all rectangles
    for (const auto &rectangle : frameData.all_rectangles.rectangles) {
      int rectX = std::max(0, rectangle.x);
      int rectY = std::max(0, rectangle.y);
      int rectWidth = std::min(src.cols - rectX, rectangle.width);
      int rectHeight = std::min(src.rows - rectY, rectangle.height);
      const auto cv_rectangle = cv::Rect{rectX, rectY, rectWidth, rectHeight};
      cv::rectangle(src, cv_rectangle, cv::Scalar(0, 255, 0), 2);
    }
}
