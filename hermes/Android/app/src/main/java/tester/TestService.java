package tester;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;

import communication.Communication;

import static android.os.Message.obtain;

public class TestService extends Service {

    //Tag for logs
    private final String TAG = "Test Service";

    //Member Variables
    private final IBinder myBinder = new MyLocalBinder();
    private TestThread testThread;

    private TestState state;

    /*Services*/
    private Communication commService;
    public TestState.State getState() {

        return state.getState();
    }

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
        state = new TestState();
    }

    @Override
    public void onDestroy() {
        Message msg = obtain();
        msg.what = TestMsg.QUIT;
        msg.obj = null;
        testThread.mHandler.sendMessage(msg);
        commService = null;  //Just to make sure we don't mess automatic reference counting

        super.onDestroy();
        try {
            testThread.join();
        } catch (InterruptedException e) {
            Log.i(TAG, "Service could not join with tester");
            Log.i(TAG, e.toString());
        }
        Log.i(TAG, "test Service and Background Thread Closed");
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.i(TAG, "Something Bound to the service");
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

