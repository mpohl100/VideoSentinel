package com.example.videosentinel

import android.app.Activity
import android.os.Bundle
import android.util.Log
import android.view.TextureView
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.ImageFormat
import android.graphics.YuvImage
import android.graphics.Rect
import android.media.Image.Plane
import android.widget.ImageView
import java.io.ByteArrayOutputStream
import java.nio.ByteBuffer
import androidx.camera.core.*
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.core.content.ContextCompat
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

class VideoActivity : Activity() {

    private lateinit var cameraExecutor: ExecutorService
    private lateinit var viewFinder: TextureView
    private lateinit var imageViewFiltered: ImageView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_video)

        viewFinder = findViewById(R.id.viewFinder)
        imageViewFiltered = findViewById(R.id.imageViewFiltered)
        cameraExecutor = Executors.newSingleThreadExecutor()

        startCamera()
    }

    private fun startCamera() {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(this)

        cameraProviderFuture.addListener({
            val cameraProvider: ProcessCameraProvider = cameraProviderFuture.get()

            val preview = Preview.Builder().build().also {
                it.setSurfaceProvider(viewFinder.getSurfaceProvider())
            }

            val imageAnalysis = ImageAnalysis.Builder()
                .build()
                .also {
                    it.setAnalyzer(cameraExecutor, { imageProxy ->
                        val bitmap = imageProxy.toBitmap()
                        val filteredBitmap = imageProxy.toBitmap()
                        applyCppFilter(bitmap, filteredBitmap)
                        // TODO: Display the filteredBitmap on your TextureView or another view
                        displayFilteredBitmap(filteredBitmap)
                        imageProxy.close()
                    })
                }

            val cameraSelector = CameraSelector.DEFAULT_BACK_CAMERA

            try {
                cameraProvider.unbindAll()
                cameraProvider.bindToLifecycle(
                    this, cameraSelector, preview, imageAnalysis
                )
            } catch (exc: Exception) {
                Log.e("VideoActivity", "Use case binding failed", exc)
            }

        }, ContextCompat.getMainExecutor(this))
    }

    private fun ImageProxy.toBitmap(): Bitmap {
        // Conversion code from ImageProxy to Bitmap
        val image = this.image ?: return Bitmap.createBitmap(1, 1, Bitmap.Config.ARGB_8888)
        val yuvImage = YuvImage(
            planesToByteArray(image.planes),
            ImageFormat.NV21, width, height, null
        )
        val out = ByteArrayOutputStream()
        yuvImage.compressToJpeg(Rect(0, 0, width, height), 100, out)
        val yuv = out.toByteArray()
        return BitmapFactory.decodeByteArray(yuv, 0, yuv.size)
    }

    private fun planesToByteArray(planes: Array<Plane>): ByteArray {
        val buffer = ByteBuffer.allocate(planes.sumOf { it.buffer.remaining() })
        for (plane in planes) {
            buffer.put(plane.buffer)
        }
        return buffer.array()
    }

    private fun displayFilteredBitmap(filteredBitmap: Bitmap) {
        imageViewFiltered.setImageBitmap(filteredBitmap)
        imageViewFiltered.visibility = ImageView.VISIBLE
    }

    private fun applyCppFilter(bitmap: Bitmap, dest: Bitmap){
        rect(bitmap, dest) // Call your C++ filter here
    }

    override fun onDestroy() {
        super.onDestroy()
        cameraExecutor.shutdown()
    }

    external fun rect(bitmapIn: Bitmap, bitmapOut: Bitmap)

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }

}
