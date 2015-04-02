package tester;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;
import communication.Communication;
import tester.helpers.TestMessageHandler;
import tester.helpers.TestMsg;
import tester.helpers.TestThread;

import static android.os.Message.obtain;

public class TestService extends Service {
    //Tag for logs
    private final String TAG = "Test Service";

    //Member Variables
    private final IBinder myBinder = new MyLocalBinder();
    private TestThread testThread;
    private Communication commService;

    /*Services*/
    //This method allows everything to send async messages to the tester
    //Used by the communication subsystem to tell the testers to start running performance tests
    public Handler getHandler()
    {
        return testThread.getHandler();
    }

    //Requests that we start the test suit
    public void startTest()
    {
        //Start Testing
        Log.i(TAG, "Requesting a start");
        //Tell the comm service to request a start test and then tell the testerService
        TestState.getInstance().setState(TestState.State.PREPARING, true); //NOTE: We might have to move this to the other thread to prevent issues with timeouts
        commService.sendTestRequest(getHandler());
    }

    //A request to start the service was received
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(TAG, "Received an Intent to start");
        return super.onStartCommand(intent, flags, startId);
    }

    //On creation we start the threads and initilize the subsystem
    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "Test Service Created and Threads Started");
        testThread = new TestThread("Tester");
        testThread.start();

        //We need to create the StateMachine and then set its handler
        TestState.getInstance().setHandler(testThread.getHandler());

        //Let the handler know that we are its parent
        Handler mHandler = testThread.getHandler();
        ((TestMessageHandler)mHandler).setDelegate(this);
    }

    //When destroyed we cleanly close our subsystem
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

    //Lets us know that something has bound to the service
    @Override
    public IBinder onBind(Intent intent) {
        Log.i(TAG, "Something Bound to the tester service");
        return myBinder;
    }

    //The binder for the TestService
    public class MyLocalBinder extends Binder {
        public TestService getService() {
            return TestService.this;
        }
    }

    //The services need to know about each other so we need to explictly tell the service
    //where the communication service is
    public void setCommunicator(Service service)
    {
        commService = (Communication) service;
    }
}

