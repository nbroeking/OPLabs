package communication;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;

import main.MainMsg;

import static android.os.Message.obtain;

public class Communication extends Service {

    //Tag for logs
    private final String TAG = "Communication Service";

    //Member Variables
    private final IBinder myBinder = new MyLocalBinder();
    private CommThread commThread;

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        Log.i(TAG, "Received an Intent to start");
        //TODO do something useful
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onCreate(){
        super.onCreate();
        Log.i(TAG, "Comm Service Created and Threads Started");
        commThread = new CommThread("Communication");
        commThread.start();
    }

    @Override
    public void onDestroy()
    {
        Message msg = obtain();
        msg.what = MainMsg.QUIT;
        msg.obj = null;
        commThread.mHandler.sendMessage(msg);

        super.onDestroy();
        try {
            commThread.join();
        } catch (InterruptedException e) {
            Log.i(TAG, "Service could not join with tester");
            Log.i(TAG, e.toString());
        }
        Log.i(TAG, "Comm Service and Background Thread Closed");
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.i(TAG, "Something Bound to the service");
        return myBinder;
    }

    //Communication Methods

    public void login()
    {
        Log.i(TAG, "Scheduling Login");


    }
    public class MyLocalBinder extends Binder {
        public Communication getService() {
            return Communication.this;
        }
    }
}
