#include "opencv-utils.h"
#include "video-sentinel/webcam/webcam.h"
#include "video-sentinel/par/parallel.h"
#include "video-sentinel/deduction/ComparisonParams.h"
#include "video-sentinel/deduction/SkeletonParams.h"
#include "video-sentinel/detection/Rectangle.h"
#include "video-sentinel/preview/preview.h"
#include "video-sentinel/preview/SingleObjectPreview.h"

#include <opencv2/imgproc.hpp>

#include <memory>
#include <mutex>
#include <optional>


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

// ---------------------------------------------------------------------------------------------------------------
// preview

struct SingleObjectPreviewParams{
    bool activate_filter = true;
    std::string ascii_art;
    deduct::SkeletonParams skeleton_params;
    deduct::ComparisonParams comparison_params;
};

static std::optional<SingleObjectPreviewParams> single_object_preview_params = std::nullopt;
static std::shared_ptr<preview::VideoPreview> video_preview = nullptr;
static std::mutex video_preview_mutex = {};

void set_single_object_preview_settings(bool activate_filter, const std::string& ascii_art, int angle_step_skeleton, double comparison_tolerance, bool comparison_only_outer)
{
    single_object_preview_params = SingleObjectPreviewParams{ activate_filter, ascii_art, deduct::SkeletonParams{angle_step_skeleton}, deduct::ComparisonParams{comparison_tolerance, comparison_only_outer}};
    drop_preview();
    create_preview();
}

void create_preview(){
    std::unique_lock<std::mutex> lock{video_preview_mutex};
    if(single_object_preview_params.has_value() && single_object_preview_params->activate_filter){
        auto* vp = new preview::SingleObjectPreview{4, single_object_preview_params->ascii_art, single_object_preview_params->skeleton_params, single_object_preview_params->comparison_params};
        video_preview.reset(vp);
        return;
    }
    auto* vp = new preview::VideoPreview{4};
    video_preview.reset(vp);
}
void drop_preview(){
    std::unique_lock<std::mutex> lock{video_preview_mutex};
    video_preview = nullptr;
}

bool shall_frame_be_posted()
{
    std::unique_lock<std::mutex> lock{video_preview_mutex};
    if(!video_preview){
        return false;
    }
    video_preview->update_calculation_status();
    return video_preview->get_frame_calculation_status() == preview::FrameCalculationStatus::NOT_STARTED ||
      video_preview->get_frame_calculation_status() == preview::FrameCalculationStatus::DONE;
}

void set_frame(Mat src)
{
    std::unique_lock<std::mutex> lock{video_preview_mutex};
    if(!video_preview){
        return;
    }
    int rings = 1;
    int gradient_threshold = 15;
    video_preview->set_mat(
            src,
            od::Rectangle{0, 0, src.cols, src.rows}, rings,
            gradient_threshold);
}

bool are_new_rectangles_available()
{
    std::unique_lock<std::mutex> lock{video_preview_mutex};
    if(!video_preview){
        return false;
    }
    return video_preview->get_rectangles_query_status() == preview::RectanglesQueryStatus::NOT_REQUESTED;
}

std::vector<Rectangle> get_rectangles()
{
    std::unique_lock<std::mutex> lock{video_preview_mutex};
    if(!video_preview){
        return {};
    }
    std::vector<Rectangle> ret;
    const auto preview_rects = video_preview->get_all_rectangles();
    for(const auto& rect : preview_rects){
        auto result_rect = Rectangle{};
        result_rect.x = rect.x;
        result_rect.y = rect.y;
        result_rect.width = rect.width;
        result_rect.height = rect.height;
        ret.push_back(result_rect);
    }
    return ret;
}
