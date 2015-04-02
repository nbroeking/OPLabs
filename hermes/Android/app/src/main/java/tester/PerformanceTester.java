package tester;

import android.util.Log;

import tester.helpers.TestSettings;

/**
 * Created by nbroeking on 4/1/15.
 * This handles all the performance tests
 */
public class PerformanceTester {
    private final static String TAG = "Performance Tests";
    private TestSettings settings;
    public PerformanceTester(TestSettings settings1)
    {
        settings = settings1;
    }

    public TestResults runPingTest()
    {
        Log.i(TAG, "Run Ping Test");
        return null;
    }
}
