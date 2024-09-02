package com.example.videosentinel

import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

class RectangleQuerier(private val rectangleOverlayView: RectangleOverlayView) {

    fun computeRectangles(){
        // get rectangles from C++ and set them in the rectangleOverlay
        if(arenewrectanglesavailable()){
            val rectangles = getrectangles()
            rectangleOverlayView.updateRectangles(rectangles)
        }
    }


    // JNI methods
    external fun arenewrectanglesavailable(): Boolean
    external fun getrectangles(): Array<Rectangle>

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}