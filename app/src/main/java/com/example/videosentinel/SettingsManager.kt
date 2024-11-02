package com.example.videosentinel

import android.content.Context
import android.content.SharedPreferences
import androidx.preference.PreferenceManager

data class SettingsData(
    var activateFilter: Boolean = true,
    var asciiArt: String = "",
    var angleStepSkeleton: Int = 15,
    var comparisonTolerance: Double = 0.05,
    var comparisonOnlyOuter: Boolean = true
)

class SettingsManager(context: Context) {
    private val sharedPreferences: SharedPreferences = PreferenceManager.getDefaultSharedPreferences(context)

    // Load saved settings or default values
    var settings: SettingsData = loadSettings()

    private fun loadSettings(): SettingsData {
        return SettingsData(
            activateFilter = sharedPreferences.getBoolean("activate_filter", true),
            asciiArt = sharedPreferences.getString("ascii_art", "") ?: "",
            angleStepSkeleton = sharedPreferences.getInt("angle_step_skeleton", 15),
            comparisonTolerance = sharedPreferences.getFloat("comparison_tolerance", 0.05f).toDouble(),
            comparisonOnlyOuter = sharedPreferences.getBoolean("comparison_only_outer", true)
        )
    }

    // Save settings to SharedPreferences
    fun saveSettings(data: SettingsData) {
        with(sharedPreferences.edit()) {
            putBoolean("activate_filter", data.activateFilter)
            putString("ascii_art", data.asciiArt)
            putInt("angle_step_skeleton", data.angleStepSkeleton)
            putFloat("comparison_tolerance", data.comparisonTolerance.toFloat())
            putBoolean("comparison_only_outer", data.comparisonOnlyOuter)
            apply()
        }
    }

    // Call the C++ function with the current settings
    external fun applySettings()

    companion object {
        init {
            System.loadLibrary("native-lib") // Replace with your library name
        }
    }

    // Method to prepare the data for C++ call
    fun prepareAndApplySettings() {
        val data = settings
        saveSettings(data)
        applySettings(
            data.activateFilter,
            data.asciiArt,
            data.angleStepSkeleton,
            data.comparisonTolerance,
            data.comparisonOnlyOuter
        )
    }

    private external fun applySettings(
        activateFilter: Boolean,
        asciiArt: String,
        angleStepSkeleton: Int,
        comparisonTolerance: Double,
        comparisonOnlyOuter: Boolean
    )
}
