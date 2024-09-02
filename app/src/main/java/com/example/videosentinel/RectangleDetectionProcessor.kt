package com.example.videosentinel

import android.annotation.SuppressLint
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import android.graphics.Bitmap

class RectangleDetectionProcessor(private val rectangleOverlayView: RectangleOverlayView) : ImageAnalysis.Analyzer{

    private val rectangleQuerier = RectangleQuerier(rectangleOverlayView)

    @SuppressLint("UnsafeExperimentalUsageError", "UnsafeOptInUsageError")
    override fun analyze(imageProxy: ImageProxy) {
        // post image to C++ code
        if(shallframebeposted()){
            val bitmap = imageProxy.toBitmap()
            setframe(bitmap)
        }
        rectangleQuerier.computeRectangles()
    }

    // JNI methods
    external fun shallframebeposted(): Boolean
    external fun setframe(bitmap: Bitmap)

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}