package com.example.videosentinel

import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AppCompatActivity

class SettingsActivity : AppCompatActivity() {
    // Initialize SettingsManager with context in the constructor
    private val settingsManager: SettingsManager by lazy {
        SettingsManager(applicationContext)
    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_settings)

        // below line is to change
        // the title of our action bar.
        supportActionBar?.setTitle("Settings")

        // below line is used to check if
        // frame layout is empty or not.
        // below line is used to check if
        // frame layout is empty or not.
        if (findViewById<View?>(R.id.idFrameLayout) != null) {
            if (savedInstanceState != null) {
                return
            }
            // below line is to inflate our fragment.
            fragmentManager.beginTransaction().add(R.id.idFrameLayout, SettingsFragment()).commit()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        // Apply the settings when the activity is closed
        settingsManager.prepareAndApplySettings()
    }
}
