package tester.helpers;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import interfaces.CommunicationDelegate;
import tester.TestService;
import tester.TestState;

//This handler is associated with the Tester service and thread

public class TestMessageHandler extends Handler {
    private final static String TAG = "TEST MESSAGE HANDLER";

    private CommunicationDelegate delegate;

    public TestMessageHandler() {
        delegate = null;
    }
    public void setDelegate(CommunicationDelegate value)
    {
        delegate = value;
    }
	@Override
	public void handleMessage(Message msg) {
        switch (msg.what) {
            // things that this thread should do
            case TestMsg.QUIT:
                Looper.myLooper().quit();
                break;

            case TestMsg.START_TEST:
                //This next line should be replaced with a call to start the test suit
                ((TestService)delegate).getStateMachine().setState(TestState.State.TESTINGPING);
                break;

            case TestMsg.STATE_TIMEOUT:
                ((TestService)delegate).getStateMachine().timeout(((TestState.State)msg.obj));
                break;

            default:
                Log.i(TAG,"Test Subsystem received an unknown message");
                break;
        }
	}
}