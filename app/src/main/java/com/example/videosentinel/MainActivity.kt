package com.example.videosentinel

import android.content.pm.PackageManager
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.Toast
import androidx.camera.view.PreviewView
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.example.videosentinel.CameraManager
import com.example.videosentinel.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var cameraManager: CameraManager

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        createCameraManager()
        checkForPermission()
        onClicks()
    }

    private fun checkForPermission() {
        if (allPermissionsGranted()) {
            cameraManager.startCamera()
        } else {
            ActivityCompat.requestPermissions(
                this,
                REQUIRED_PERMISSIONS,
                REQUEST_CODE_PERMISSIONS
            )
        }
    }

    private fun onClicks() {
        var button = findViewById<Button>(R.id.btnSwitch);
        button.setOnClickListener {
            cameraManager.changeCameraSelector()
        }
        var buttonSwitch = findViewById<Button>(R.id.btnSettings);
        button.setOnClickListener{
            openSettingsActivity()
        }
    }

    private fun openSettingsActivity() {
        val intent = Intent(this, SettingsActivity::class.java)
        startActivity(intent)
    }

    override fun onRequestPermissionsResult(
        requestCode: Int, permissions: Array<String>, grantResults:
        IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_CODE_PERMISSIONS) {
            if (allPermissionsGranted()) {
                cameraManager.startCamera()
            } else {
                Toast.makeText(this, "Permissions not granted by the user.", Toast.LENGTH_SHORT)
                    .show()
                finish()
            }
        }
    }

    private fun createCameraManager() {
        var preview = findViewById<PreviewView>(R.id.previewView_finder)
        var rectangleOverlay = findViewById<RectangleOverlayView>(R.id.rectangleOverlay_finder)
        cameraManager = CameraManager(
            this,
            preview,
            this,
            rectangleOverlay,
        )
    }

    private fun processPicture(){

    }

    private fun allPermissionsGranted() = REQUIRED_PERMISSIONS.all {
        ContextCompat.checkSelfPermission(baseContext, it) == PackageManager.PERMISSION_GRANTED
    }

    companion object {
        private const val REQUEST_CODE_PERMISSIONS = 10
        private val REQUIRED_PERMISSIONS = arrayOf(android.Manifest.permission.CAMERA)
    }

}