package tester;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

import tester.helpers.TestMsg;

public class TestState {
    public enum State {IDLE, COMPLETED, PREPARING, TESTINGPING}

    private State state;
    private final static String TAG = "TESTSTATE";
    private Handler stateMachine;

    public TestState(Handler handler){
        state = State.IDLE;
        stateMachine = handler;
    }

    public State getState()
    {
        synchronized (this) {
            return state;
        }
    }
    public void setState(State arg)
    {
        synchronized (this) {
            state = arg;
            if (arg != State.IDLE) {
                Message msg = Message.obtain();
                msg.what = TestMsg.STATE_TIMEOUT;
                msg.obj = state;

                stateMachine.sendMessageDelayed(msg, 2000);
            }
            Log.i(TAG, "State changed to " + state.toString());
        }
    }

    public void timeout( State type)
    {
        synchronized (this){
            if ( type == state)
            {
                Log.i(TAG, "State Timeout");
                setState(State.IDLE);
                //Notify lots of people that there was a timeout

            }
        }
    }
}

