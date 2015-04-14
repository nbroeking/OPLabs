package tester;

import android.os.Handler;
import android.os.Message;
import android.util.Log;
import tester.helpers.TestMsg;

//Singleton class that allows everyone to know the state of the performance tests
public class TestState {
    //These are the current states that the testing subsystem can be in
    public enum State {IDLE, COMPLETED, PREPARING, TESTINGDNS, TESTINGLATENCY, TESTINGTHROUGHPUT, TESTING }

    //Members
    private State state;
    private final static String TAG = "TESTSTATE";
    private Handler handler;
    private TestResults phoneResults;
    private TestResults routerResults;

    //Singleton Members
    private static TestState ourInstance = new TestState();
    public static TestState getInstance() {
        return ourInstance;
    }

    private TestState() {
        state = State.IDLE;
        handler = null;
        phoneResults = null;
        routerResults = null;
    }
    //Results
    public TestResults getPhoneResults() {
        synchronized (this) {
            return phoneResults;
        }
    }

    public void setPhoneResults(TestResults latestResults) {
        this.phoneResults = latestResults;
    }

    //Returns the state of the tests
    //Threadsafe
    public State getState()
    {
        synchronized (this) {
            return state;
        }
    }

    //Changes the state of the tests
    //Has the option for a timeout.
    //The timeout is used for network communication that could not happen
    public void setState(State arg, boolean needsTimeout)
    {
        synchronized (this) {
            state = arg;
            if (arg != State.IDLE && needsTimeout) {
                Message msg = Message.obtain();
                msg.what = TestMsg.STATE_TIMEOUT;
                msg.obj = state;

                handler.sendMessageDelayed(msg, 20000);
            }
            Log.i(TAG, "State changed to " + state.toString());
        }
    }

    //This method specifys how we handle a timout
    public void timeout( State type)
    {
        synchronized (this){
            if ( type == state)
            {
                Log.i(TAG, "State Timeout");
                setState(State.IDLE, false);
                //Notify lots of people that there was a timeout

            }
        }
    }

    public TestResults getRouterResults() {
        synchronized (this) {
            return routerResults;
        }
    }

    public void setRouterResults(TestResults routerResults) {
        synchronized (this) {
            this.routerResults = routerResults;
        }
    }
    //We need explicily tell the statemachine that the handler is the test subsystem
    public void setHandler(Handler handler1)
    {
        this.handler = handler1;
    }
}

