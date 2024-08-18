package com.example.videosentinel

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.TextView
import com.example.videosentinel.databinding.ActivityMainBinding
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    var srcBitmap: Bitmap? = null
    var dstBitmap: Bitmap? = null
    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(R.layout.activity_main)

        // Load the original image
        srcBitmap = BitmapFactory.decodeResource(this.resources, R.drawable.mountain)

        // Create and display dstBitmap in image view, we will keep updating
        // dstBitmap and the changes will be displayed on screen
        dstBitmap = srcBitmap!!.copy(srcBitmap!!.config, true)
        imageView.setImageBitmap(dstBitmap)

        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()
    }

    fun btnRect_click(view: View){
        // This is the actual call to the rect method inside native-lib.cpp
        this.rect(srcBitmap!!, srcBitmap!!)
    }

    /**
     * A native method that is implemented by the 'videosentinel' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String
    external fun rect(bitmapIn: Bitmap, bitmapOut: Bitmap)


    companion object {
        // Used to load the 'videosentinel' library on application startup.
        init {
            System.loadLibrary("videosentinel")
        }
    }
}