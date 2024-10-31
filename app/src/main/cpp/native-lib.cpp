#include <jni.h>
#include <string>
#include "android/bitmap.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <android/log.h>
#include "opencv-utils.h"

void bitmapToMat(JNIEnv *env, jobject bitmap, Mat& dst, jboolean needUnPremultiplyAlpha)
{
    AndroidBitmapInfo  info;
    void*              pixels = 0;

    try {
        CV_Assert( AndroidBitmap_getInfo(env, bitmap, &info) >= 0 );
        CV_Assert( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                   info.format == ANDROID_BITMAP_FORMAT_RGB_565 );
        CV_Assert( AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0 );
        CV_Assert( pixels );
        dst.create(info.height, info.width, CV_8UC3);  // Create destination Mat in RGB format
        if( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 )
        {
            Mat tmp(info.height, info.width, CV_8UC4, pixels);
            Mat tmpRGB;
            if(needUnPremultiplyAlpha) {
                cvtColor(tmp, tmpRGB, COLOR_mRGBA2RGBA);  // Unpremultiply alpha and convert to RGBA
                cvtColor(tmpRGB, dst, COLOR_RGBA2RGB);    // Convert RGBA to RGB
            } else {
                cvtColor(tmp, dst, COLOR_RGBA2RGB);       // Convert RGBA to RGB
            }
        } else {
            // info.format == ANDROID_BITMAP_FORMAT_RGB_565
            Mat tmp(info.height, info.width, CV_8UC2, pixels);
            cvtColor(tmp, dst, COLOR_BGR5652RGB);  // Convert BGR565 to RGB
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        return;
    } catch(const cv::Exception& e) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
        return;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nBitmapToMat}");
        return;
    }
}

void matToBitmap(JNIEnv* env, Mat src, jobject bitmap, jboolean needPremultiplyAlpha)
{
    AndroidBitmapInfo  info;
    void*              pixels = 0;

    try {
        CV_Assert( AndroidBitmap_getInfo(env, bitmap, &info) >= 0 );
        CV_Assert( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                   info.format == ANDROID_BITMAP_FORMAT_RGB_565 );
        CV_Assert( src.dims == 2 && info.height == (uint32_t)src.rows && info.width == (uint32_t)src.cols );
        CV_Assert( src.type() == CV_8UC3 || src.type() == CV_8UC1 );  // Ensure input Mat is in RGB format or grayscale
        CV_Assert( AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0 );
        CV_Assert( pixels );
        if( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 )
        {
            Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if(src.type() == CV_8UC1)
            {
                cvtColor(src, tmp, COLOR_GRAY2RGBA);  // Convert grayscale to RGBA
            } else if(src.type() == CV_8UC3){
                cvtColor(src, tmp, COLOR_RGB2RGBA);   // Convert RGB to RGBA
            }
        } else {
            // info.format == ANDROID_BITMAP_FORMAT_RGB_565
            Mat tmp(info.height, info.width, CV_8UC2, pixels);
            if(src.type() == CV_8UC1)
            {
                cvtColor(src, tmp, COLOR_GRAY2BGR565);  // Convert grayscale to BGR565
            } else if(src.type() == CV_8UC3){
                cvtColor(src, tmp, COLOR_RGB2BGR565);   // Convert RGB to BGR565
            }
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        return;
    } catch(const cv::Exception& e) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
        return;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nMatToBitmap}");
        return;
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_videosentinel_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_videosentinel_MainActivity_rect(JNIEnv *env, jobject p_this, jobject bitmapIn, jobject bitmapOut) {
    Mat src;
    bitmapToMat(env, bitmapIn, src, false);
    addRectangles(src);
    matToBitmap(env, src, bitmapOut, false);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_videosentinel_SettingsManager_applySettings(
        JNIEnv* env,
        jobject /* this */,
        jboolean activateFilter,
        jstring asciiArt,
        jint angleStepSkeleton,
        jdouble comparisonTolerance,
        jboolean comparisonOnlyOuter
) {
    // Convert jstring to std::string
    const char* asciiArtChars = env->GetStringUTFChars(asciiArt, nullptr);
    std::string asciiArtCpp(asciiArtChars);
    env->ReleaseStringUTFChars(asciiArt, asciiArtChars);

    // Prepare the C++ struct
    bool activate_filter = static_cast<bool>(activateFilter);
    std::string ascii_art = asciiArtCpp;
    int angle_step_skeleton = angleStepSkeleton;
    double comparison_params_tolerance = comparisonTolerance;
    bool comparison_params_only_outer = static_cast<bool>(comparisonOnlyOuter);

    // Call the original C++ function
    set_single_object_preview_settings(
            activate_filter,
            ascii_art,
            angle_step_skeleton,
            comparison_params_tolerance,
            comparison_params_only_outer
    );
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_videosentinel_CameraManager_createpreview(
        JNIEnv *env,
        jobject /* this */) {
    __android_log_print(ANDROID_LOG_DEBUG, "Cpp.CreatePreview", "Creating video preview");
    create_preview();
    __android_log_print(ANDROID_LOG_DEBUG, "Cpp.CreatePreview", "Successfully created video preview");

}

extern "C" JNIEXPORT void JNICALL
Java_com_example_videosentinel_CameraManager_droppreview(
        JNIEnv *env,
        jobject /* this */) {
    __android_log_print(ANDROID_LOG_DEBUG, "Cpp.DropPreview", "Dropping video preview");
    drop_preview();
    __android_log_print(ANDROID_LOG_DEBUG, "Cpp.DropPreview", "Successfully dropped video preview");
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_videosentinel_RectangleDetectionProcessor_shallframebeposted(
        JNIEnv *env,
        jobject /* this */) {
    __android_log_print(ANDROID_LOG_DEBUG, "Cpp.QueryFrame", "Ask whether a frame shall be posted");
    auto ret = shall_frame_be_posted();
    __android_log_print(ANDROID_LOG_DEBUG, "Cpp.QueryFrame", "Frame shall be posted: %b", ret);
    return ret;
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_videosentinel_RectangleDetectionProcessor_setframe(
        JNIEnv *env,
        jobject /* this */,
        jobject bitmapIn) {
    __android_log_print(ANDROID_LOG_DEBUG, "Cpp.SetFrame", "Posting a frame to the video preview");
    Mat src;
    bitmapToMat(env, bitmapIn, src, false);
    int rows = src.rows;
    int cols = src.cols;
    int channels = src.channels();
    __android_log_print(ANDROID_LOG_DEBUG, "Cpp.SetFrame", "Mat Dimensions - Rows: %d, Cols: %d, Channels: %d", rows, cols, channels);
    set_frame(src);
    __android_log_print(ANDROID_LOG_DEBUG, "Cpp.SetFrame", "Successfully posted a frame to the video preview");
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_videosentinel_RectangleQuerier_arenewrectanglesavailable(
        JNIEnv *env,
        jobject /* this */) {
    __android_log_print(ANDROID_LOG_DEBUG, "Cpp.QueryRectangles", "Ask whether new rectangles are available");
    auto ret =  are_new_rectangles_available();
    __android_log_print(ANDROID_LOG_DEBUG, "Cpp.QueryRectangles", "New rectangles are available; %b", ret);
    return ret;
}

extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_example_videosentinel_RectangleQuerier_getrectangles(JNIEnv* env, jobject /* this */) {
    __android_log_print(ANDROID_LOG_DEBUG, "Cpp.GetRectangles", "Getting new rectangles");
    // Call the native function to get the vector of rectangles
    std::vector<Rectangle> rectangles = get_rectangles();

    __android_log_print(ANDROID_LOG_DEBUG, "Cpp.GetRectangles", "Received new rectangles nr.: %i", rectangles.size());


    // Get the Kotlin Rectangle class and its constructor
    jclass rectClass = env->FindClass("com/example/videosentinel/Rectangle");
    jmethodID constructor = env->GetMethodID(rectClass, "<init>", "(IIII)V");

    // Create a new Java array of Rectangle objects
    jobjectArray rectArray = env->NewObjectArray(rectangles.size(), rectClass, nullptr);

    // Populate the array with Rectangle objects
    for (size_t i = 0; i < rectangles.size(); ++i) {
        const Rectangle& rect = rectangles[i];
        jobject rectObject = env->NewObject(rectClass, constructor, rect.x, rect.y, rect.width, rect.height);
        env->SetObjectArrayElement(rectArray, i, rectObject);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "Cpp.GetRectangles", "Returning new kotlin rectangles nr.: %i", env->GetArrayLength(rectArray));

    return rectArray;
}
