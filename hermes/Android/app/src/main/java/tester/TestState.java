package tester;

import android.os.Handler;
import android.os.Message;
import android.util.Log;
import tester.helpers.TestMsg;

//Singleton class that allows everyone to know the state of the performance tests
//We store all information related to a test here
public class TestState {

    //These are the current states that the testing subsystem can be in
    public enum State {IDLE, COMPLETED, PREPARING, TESTINGDNS, TESTINGLATENCY, TESTINGTHROUGHPUT, TESTING }

    //Members
    private State state;
    private final static String TAG = "TESTSTATE";
    private Handler handler;
    private TestResults phoneResults;
    private long startTime;

    //Singleton Members
    private static TestState ourInstance = new TestState();
    public static TestState getInstance() {
        return ourInstance;
    }

    private TestState() {
        state = State.IDLE;
        handler = null;
        phoneResults = null;
        startTime = -1;
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
    //Thread safe
    public State getState(){
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

            if( state == State.COMPLETED){
                startTime = -1;
            }
            Log.i(TAG, "State changed to " + state.toString());
        }
    }

    //This method specifys how we handle a timeout
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

    //Returns the state as a string so we can display it on the gui
    public String getStateAsString(){
        switch (state) {
            case IDLE:
                return "Idle";

            case COMPLETED:
                return "Completed";

            case PREPARING:
                return "Preparing";

            case TESTINGDNS:
                return "Testing DNS";

            case TESTINGLATENCY:
                return "Testing Latency";

            case TESTINGTHROUGHPUT:
                return "Testing Throughput";

            default:
                return "";
        }
    }
    //We need explicitly tell the state machine that the handler is the test subsystem
    public void setHandler(Handler handler1)
    {
        this.handler = handler1;
    }

    public long getStartTime() {
        synchronized (this) {
            if (startTime <= 0) {
                startTime = System.currentTimeMillis();
            }
            return startTime;
        }
    }

    //We keep track of the start time of a throughput test for animation purposes
    public void setStartTime(long startTime) {
        synchronized (this) {
            this.startTime = startTime;
        }
    }

}

