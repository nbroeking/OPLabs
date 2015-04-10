package tester.helpers;

import android.content.Intent;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import java.net.DatagramSocket;

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
                //Create the information to return the state of the test

                Intent intent = new Intent();
                intent.setAction("TestCompleted");

                if( msg.obj == null){
                    Log.e(TAG, "Received an error from the comm start test sub system");
                    intent.putExtra("Results", new TestResults(-1));
                }
                else {
                    //a call to start the test suit
                    TestState.getInstance().setState(TestState.State.TESTING, false);
                    PerformanceTester tester = new PerformanceTester((TestSettings) msg.obj);
                    TestResults results = tester.runTests();
                    intent.putExtra(("Results"), results);

                    //We have the state machine hold on to the results in case hermes isn't the main app
                    TestState.getInstance().setLatestResults(results);
                }
                TestState.getInstance().setState(TestState.State.COMPLETED, false);
                parent.sendBroadcast(intent);
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