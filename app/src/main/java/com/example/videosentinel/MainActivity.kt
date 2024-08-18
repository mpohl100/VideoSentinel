package com.example.videosentinel

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.ImageView
import android.widget.TextView
import com.example.videosentinel.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    var originalBitmap: Bitmap? = null
    var srcBitmap: Bitmap? = null
    var dstBitmap: Bitmap? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContentView(R.layout.activity_main)

        // Load the original image
        srcBitmap = BitmapFactory.decodeResource(this.resources, R.drawable.mountain)
        originalBitmap = BitmapFactory.decodeResource(this.resources, R.drawable.mountain)


        // Create and display dstBitmap in image view, we will keep updating
        // dstBitmap and the changes will be displayed on screen
        dstBitmap = srcBitmap!!.copy(srcBitmap!!.config, true)
        val imgView = findViewById<ImageView>(R.id.imageView)
        imgView.setImageBitmap(dstBitmap)
    }

    fun btnRect_click(view: View){
        // This is the actual call to the rect method inside native-lib.cpp
        this.rect(originalBitmap!!, dstBitmap!!)
    }

    /**
     * A native method that is implemented by the 'videosentinel' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String
    external fun rect(bitmapIn: Bitmap, bitmapOut: Bitmap)


    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}