package com.example.videosentinel

import android.content.Intent
import android.os.Bundle
import android.view.Menu
import android.view.MenuItem
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {

    private var isVideoMode = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.main_activity)
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
}