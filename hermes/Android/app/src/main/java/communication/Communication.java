package communication;

import android.app.AlertDialog;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;

import com.oplabs.hermes.R;

import communication.Helpers.CommMsg;
import tester.TestResults;

import static android.os.Message.obtain;

//The communication service. This service handles all communication of the network.
public class Communication extends Service {

    //Tag for logs
    private final String TAG = "Communication Service";

    //Member Variables
    private final IBinder myBinder = new MyLocalBinder();
    private CommThread commThread;

    //The service can receive the intent to start.
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(TAG, "Received an Intent to start");

        return super.onStartCommand(intent, flags, startId);
    }

    //When created we start the thread.
    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "Comm Service Created and Threads Started");
        commThread = new CommThread("Communication");
        commThread.start();

        //Register for broadcasts
        IntentFilter filter = new IntentFilter("TestCompleted");
        registerReceiver(receiver, filter);
    }

    //When we are destroyed we nicely clean up our threads and close everything down
    @Override
    public void onDestroy() {
        Message msg = obtain();
        msg.what = CommMsg.QUIT;
        msg.obj = null;
        commThread.mHandler.sendMessage(msg);

        //Unregister
        unregisterReceiver(receiver);

        super.onDestroy();
        try {
            commThread.join();
        } catch (InterruptedException e) {
            Log.i(TAG, "Service could not join with tester");
            Log.i(TAG, e.toString());
        }
        Log.i(TAG, "Comm Service and Background Thread Closed");
    }

    //When something binds to the service we give it our binder so it can communicate with us
    @Override
    public IBinder onBind(Intent intent) {
        Log.i(TAG, "Something Bound to the service");
        return myBinder;
    }

    //Anything can call this in its respective thread and we will put a request to login on our
    //message queue
    public void login() {
        Log.i(TAG, "Scheduling Login");

        Message msg = obtain();
        msg.what = CommMsg.LOGIN;
        msg.obj = this;
        commThread.mHandler.sendMessage(msg);
    }

    //Anything can call this in its respective thread and we will put in a request to start a test
    //suit on our message queue
    public void sendTestRequest(Handler sender) {
        Message msg = obtain();
        msg.what = CommMsg.REQUEST_TEST;
        msg.obj = sender;
        commThread.mHandler.sendMessage(msg);
    }

    public void reportResults(TestResults results){
        Message msg = obtain();
        msg.what = CommMsg.REPORT_TEST;
        msg.obj = results;
        commThread.mHandler.sendMessage(msg);
    }

    public void requestRouterResults(){
        Message msg = obtain();
        msg.what = CommMsg.REQUEST_ROUTER_RESULTS;
        msg.obj = null;
        commThread.mHandler.sendMessage(msg);
    }
    //The class that represents our binder
    public class MyLocalBinder extends Binder {
        public Communication getService() {
            return Communication.this;
        }
    }

    private BroadcastReceiver receiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            // Implement code here to be performed when
            // broadcast is detected
            Log.i(TAG, "Received Intent in the Communication sub system");

            TestResults results = intent.getParcelableExtra("Results");
            if( results.isValid()) {
                reportResults(results);
            }
        }
    };
}
