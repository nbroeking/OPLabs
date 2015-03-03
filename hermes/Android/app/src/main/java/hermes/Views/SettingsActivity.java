package hermes.Views;

import android.content.SharedPreferences;
import android.nfc.Tag;
import android.os.Bundle;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceFragment;
import android.preference.PreferenceGroup;
import android.preference.PreferenceManager;
import android.util.Log;

import com.oplabs.hermes.R;

import java.util.List;

import main.SessionData;

public class SettingsActivity extends PreferenceActivity {

    private final String TAG = "HERMES SETTINGS";

    @Override
    public void onStop()
    {
        //This is slightly overkill setting the variables every time but It is the most readable
        //and cleanest looking option so that's what we are doing.
        super.onStop();
        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(this);
        String email = sharedPref.getString("email", getResources().getString(R.string.defaultemail));
        String password = sharedPref.getString("password", getResources().getString(R.string.defaultpassword));
        String hostname = sharedPref.getString("hostname", getResources().getString(R.string.defaulthostname));

        //Set the data
        SessionData data = SessionData.getInstance();
        data.setEmail(email);
        data.setPassword(password);
        data.setHostname(hostname);
    }
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public void onBuildHeaders(List<Header> target) {
        loadHeadersFromResource(R.xml.pref_headers, target);
    }


    @Override
    protected boolean isValidFragment(String fragmentName) {
        return GeneralPrefs.class.getName().equals(fragmentName);
    }

    //Fragment in charge of managing the settings
    public static class GeneralPrefs extends PreferenceFragment implements SharedPreferences.OnSharedPreferenceChangeListener {

        @Override
        public void onCreate(Bundle savedInstanceState) {
            Log.i("General Prefs", "On Create");
            super.onCreate(savedInstanceState);
            addPreferencesFromResource(R.xml.preferences);
        }
        @Override
        public void onResume() {
            Log.i("RESUME", "RESUME");
            super.onResume();
            getPreferenceManager().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);
            for (int i = 0; i < getPreferenceScreen().getPreferenceCount(); ++i) {
                Preference preference = getPreferenceScreen().getPreference(i);
                if (preference instanceof PreferenceGroup) {
                    PreferenceGroup preferenceGroup = (PreferenceGroup) preference;
                    for (int j = 0; j < preferenceGroup.getPreferenceCount(); ++j) {
                        updatePreference(preferenceGroup.getPreference(j));
                    }
                } else {
                    updatePreference(preference);
                }
            }
        }

        @Override
        public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
            updatePreference(findPreference(key));
            Log.i("SETTINGS FRAG","Preference Changed");
        }

        private void updatePreference(Preference preference) {
            if (preference instanceof EditTextPreference)
            {
                Log.i("Settings FRAG", "EDIT TEXT PREF");
                EditTextPreference textPreference = (EditTextPreference) preference;
                if( !(textPreference.getKey()).equals("password")) {
                    preference.setSummary(textPreference.getText());
                }
            }
        }
    }
}
