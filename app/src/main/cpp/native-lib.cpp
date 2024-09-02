#include <jni.h>
#include <string>
#include "android/bitmap.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
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
Java_com_example_videosentinel_CameraManager_createpreview(
        JNIEnv *env,
        jobject /* this */) {
    create_preview();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_videosentinel_CameraManager_droppreview(
        JNIEnv *env,
        jobject /* this */) {
    drop_preview();
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_videosentinel_RectangleDetectionProcessor_shallframebeposted(
        JNIEnv *env,
        jobject /* this */) {
    return shall_frame_be_posted();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_videosentinel_RectangleDetectionProcessor_setframe(
        JNIEnv *env,
        jobject /* this */,
        jobject bitmapIn) {
    Mat src;
    bitmapToMat(env, bitmapIn, src, false);
    set_frame(src);
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_videosentinel_RectangleQuerier_arenewrectanglesavailable(
        JNIEnv *env,
        jobject /* this */) {
    return are_new_rectangles_available();
}

extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_example_videosentinel_RectangleQuerier_getrectangles(JNIEnv* env, jobject /* this */) {
    // Call the native function to get the vector of rectangles
    std::vector<Rectangle> rectangles = get_rectangles();

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

    return rectArray;
}
