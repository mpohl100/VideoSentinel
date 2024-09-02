package com.example.videosentinel

import android.annotation.SuppressLint
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy

class RectangleDetectionProcessor(private val rectangleOverlayView: RectangleOverlayView) : ImageAnalysis.Analyzer{

    private val rectangleQuerier = RectangleQuerier(rectangleOverlayView)

    @SuppressLint("UnsafeExperimentalUsageError", "UnsafeOptInUsageError")
    override fun analyze(imageProxy: ImageProxy) {
        // TODO: post image to C++ code
        // val mediaImage = imageProxy.image

        rectangleQuerier.computeRectangles()
    }
}