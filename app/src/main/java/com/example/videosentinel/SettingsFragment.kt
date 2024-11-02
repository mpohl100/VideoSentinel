package com.example.videosentinel

import android.os.Bundle
import android.preference.PreferenceFragment

class SettingsFragment : PreferenceFragment() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        // below line is used to add preference
        // fragment from our xml folder.
        addPreferencesFromResource(R.xml.preferences)
    }

}
