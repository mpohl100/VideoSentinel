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
        supportFragmentManager
            .beginTransaction()
            .replace(android.R.id.content, SettingsFragment())
            .commit()
    }

    override fun onDestroy() {
        super.onDestroy()
        // Apply the settings when the activity is closed
        settingsManager.prepareAndApplySettings()
    }
}
