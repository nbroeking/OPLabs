package tester;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;

import communication.Communication;
import interfaces.CommunicationDelegate;
import tester.helpers.TestMessageHandler;
import tester.helpers.TestMsg;
import tester.helpers.TestThread;

import static android.os.Message.obtain;

public class TestService extends Service implements CommunicationDelegate{

    //Tag for logs
    private final String TAG = "Test Service";

    //Member Variables
    private final IBinder myBinder = new MyLocalBinder();
    private TestThread testThread;
    private TestState state;
    private Communication commService;

    /*Services*/
    //This method allows everything to send async messages to the tester
    public Handler getHandler()
    {
        return testThread.getHandler();
    }

    //Allows everyone to know the state
    public TestState.State getState() {

        return state.getState();
    }

    public TestState getStateMachine()
    {
        return state;
    }
    //Requests that we start the test suit
    public void startTest()
    {
        //Start Testing
        Log.i(TAG, "Requesting a start");
        //Tell the comm service to request a start test and then tell the testerService
        state.setState(TestState.State.PREPARING); //NOTE: We might have to move this to the other thread to prevent issues with timeouts
        commService.sendTestRequest(this);
    }

    //We were notified that we can start testing
    @Override
    public void notifyComm(){Log.w(TAG, "We were notified when we were not expecting it");}
    //Methods to start and stop the service
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(TAG, "Received an Intent to start");
        return super.onStartCommand(intent, flags, startId);
    }
    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "Test Service Created and Threads Started");
        testThread = new TestThread("Tester");
        testThread.start();

        //Should block untill handler is ready
        Handler mHandler = testThread.getHandler();
        state = new TestState(mHandler);
        ((TestMessageHandler)mHandler).setDelegate(this);
    }

    @Override
    public void onDestroy() {
        Message msg = obtain();
        msg.what = TestMsg.QUIT;
        msg.obj = null;
        testThread.getHandler().sendMessage(msg);
        commService = null;  //Just to make sure we don't mess automatic reference counting

        super.onDestroy();

        try {
            testThread.join();
        } catch (InterruptedException e) {
            Log.i(TAG, "Service could not join with tester");
            Log.i(TAG, e.toString());
        }
        Log.i(TAG, "Test Service and Background Thread Closed");
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.i(TAG, "Something Bound to the tester service");
        return myBinder;
    }

    public class MyLocalBinder extends Binder {
        public TestService getService() {
            return TestService.this;
        }
    }

    /* I am not proud of this method but we need to let the test thread know about the comm service
    * So that the comm thread can send messages up to the server
    */
    public void setCommunicator(Service service)
    {
        commService = (Communication) service;
    }
}

