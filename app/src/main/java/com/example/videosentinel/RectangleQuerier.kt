package com.example.videosentinel

import android.graphics.Rect

class RectangleQuerier(private val rectangleOverlayView: RectangleOverlayView) {

    fun computeRectangles(imageRect: Rect){
        // get rectangles from C++ and set them in the rectangleOverlay
        if(arenewrectanglesavailable()){
            val rectangles = getrectangles()
            rectangleOverlayView.clear()
            rectangles.forEach {
                val objectGraphic = ObjectGraphic(rectangleOverlayView, it, imageRect)
                rectangleOverlayView.add(objectGraphic)
            }
            rectangleOverlayView.postInvalidate()
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