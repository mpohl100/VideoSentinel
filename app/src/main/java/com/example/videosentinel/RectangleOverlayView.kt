package com.example.videosentinel

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.util.AttributeSet
import android.view.View
import androidx.camera.core.CameraSelector

class RectangleOverlayView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : View(context, attrs, defStyleAttr) {

    var cameraSelector: Int = CameraSelector.LENS_FACING_FRONT
    private val paint = Paint().apply {
        color = Color.RED
        strokeWidth = 1f
        style = Paint.Style.STROKE
    }
    private var rectangles: Array<Rectangle> = emptyArray()

    fun updateRectangles(newRectangles: Array<Rectangle>) {
        rectangles = newRectangles
        invalidate()
    }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)
        rectangles.forEach { rect ->
            canvas.drawRect(rect.x.toFloat(), rect.y.toFloat(),
                (rect.x + rect.width).toFloat(),
                (rect.y + rect.height).toFloat(), paint)
        }
    }

    fun isFrontMode() = cameraSelector == CameraSelector.LENS_FACING_FRONT

    fun toggleSelector() {
        cameraSelector =
            if (cameraSelector == CameraSelector.LENS_FACING_BACK) CameraSelector.LENS_FACING_FRONT
            else CameraSelector.LENS_FACING_BACK
    }
}