package tester.helpers;

import android.content.Intent;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import tester.PerformanceTester;
import tester.TestResults;
import tester.TestService;
import tester.TestState;

//This handler is associated with the Tester service and thread
public class TestMessageHandler extends Handler {
    private final static String TAG = "TEST MESSAGE HANDLER";

    private TestService parent;

    public TestMessageHandler() {
        parent = null;
    }
    public void setDelegate(TestService value)
    {
        parent = value;
    }

    //Handles all messages passed from the services.
    //They are handled on the testing thread
	@Override
	public void handleMessage(Message msg) {
        switch (msg.what) {
            //If we receive a request to quit we will close down our looper
            case TestMsg.QUIT:
                Looper.myLooper().quit();
                break;

            //Perform the tests
            case TestMsg.START_TEST:
                //This next line should be replaced with a call to start the test suit
                TestState.getInstance().setState(TestState.State.TESTINGPING, false);
                PerformanceTester tester = new PerformanceTester(new TestSettings());

                //Run Ping Test If null there was an error and we return to complete with error information
                TestResults pingResults = tester.runPingTest();

                if(pingResults == null)
                {
                    //Error
                    TestState.getInstance().setState(TestState.State.COMPLETED, false);
                    Intent intent = new Intent();
                    intent.setAction("TestCompleted");
                    intent.putExtra("Results", "");

                    parent.sendBroadcast(intent);
                    return;
                }

                //Other Tests
                break;
            //Called when there has been a state timeout
            //NOTE: wont get called till the end of a handle call
            case TestMsg.STATE_TIMEOUT:
                TestState.getInstance().timeout(((TestState.State)msg.obj));
                break;

            default:
                Log.i(TAG,"Test Subsystem received an unknown message");
                break;
        }
	}
}