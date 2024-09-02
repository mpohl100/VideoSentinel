package com.example.videosentinel

import android.content.Intent
import android.view.Menu
import android.view.MenuItem
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import androidx.appcompat.app.AppCompatActivity
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.*
import androidx.camera.core.Preview
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.core.content.ContextCompat
import android.view.View
import android.widget.FrameLayout
import com.example.videosentinel.databinding.ActivityMainBinding
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors



class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private lateinit var cameraExecutor: ExecutorService
    private var isFrameProcessing = false
    private val handler = Handler(Looper.getMainLooper())
    private lateinit var overlayView: RectangleOverlayView
    private var isVideoMode = false


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Initialize camera executor
        cameraExecutor = Executors.newSingleThreadExecutor()

        // Setup Camera Preview and Overlay View
        setupCameraPreview()
        setupOverlayView()

        // Enable preview
        createpreview()

        // Start drawing rectangles
        startDrawingRectangles()

    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.main_menu, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        return when (item.itemId) {
            R.id.action_toggle_mode -> {
                isVideoMode = !isVideoMode
                item.title = if (isVideoMode) "Video" else "Photo"
                if (isVideoMode) {
                    // do nothing
                } else {
                    startPhotoActivity()
                }
                true
            }
            else -> super.onOptionsItemSelected(item)
        }
    }

    private fun startPhotoActivity() {
        val intent = Intent(this, PhotoActivity::class.java)
        startActivity(intent)
    }

    private fun setupCameraPreview() {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(this)
        cameraProviderFuture.addListener({
            val cameraProvider = cameraProviderFuture.get()
            val preview = Preview.Builder().build()
            val cameraSelector = CameraSelector.DEFAULT_BACK_CAMERA
            val imageAnalysis = ImageAnalysis.Builder().build().apply {
                setAnalyzer(cameraExecutor, { imageProxy ->
                    processFrame(imageProxy)
                    imageProxy.close()
                })
            }

            preview.setSurfaceProvider(binding.previewView.surfaceProvider)

            try {
                cameraProvider.unbindAll()
                cameraProvider.bindToLifecycle(
                    this, cameraSelector, preview, imageAnalysis
                )
            } catch (exc: Exception) {
                exc.printStackTrace()
            }
        }, ContextCompat.getMainExecutor(this))
    }

    private fun setupOverlayView() {
        overlayView = RectangleOverlayView(this)
        binding.root.addView(overlayView, FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.MATCH_PARENT
        ))
    }

    private fun processFrame(imageProxy: ImageProxy) {
        if (!isFrameProcessing) {
            isFrameProcessing = true

            // Convert ImageProxy to Bitmap
            val bitmap = binding.previewView.bitmap
            bitmap?.let {
                // Check if native code is ready to receive a new frame
                if (shallframebeposted()) {
                    setframe(it)
                }
            }

            isFrameProcessing = false
        }
    }

    private fun startDrawingRectangles() {
        handler.post(object : Runnable {
            override fun run() {
                // Check if new rectangles are available
                if (arenewrectanglesavailable()) {
                    val rectangles = getrectangles()
                    overlayView.updateRectangles(rectangles)
                }
                handler.postDelayed(this, 10)  // Check every 10 milliseconds
            }
        })
    }

    override fun onDestroy() {
        super.onDestroy()
        cameraExecutor.shutdown()

        // Disable preview
        droppreview()
    }

    // JNI methods
    external fun createpreview()
    external fun droppreview()
    external fun shallframebeposted(): Boolean
    external fun setframe(bitmap: Bitmap)
    external fun arenewrectanglesavailable(): Boolean
    external fun getrectangles(): Array<Rectangle>

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}