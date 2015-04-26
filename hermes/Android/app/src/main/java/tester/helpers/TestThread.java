package tester.helpers;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.util.Log;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

//This class provides a thread subsystem that runs all of our performance tests
public class TestThread extends HandlerThread {

    private static final String TAG = "Hermes Test Thread";
    private String mName;

    private TestMessageHandler mHandler;

    //Mutex and conditions
    private final ReentrantLock lock = new ReentrantLock();
    private final Condition notReady = lock.newCondition();

    public TestThread(String name) {
        super(name);
        mName = name;
        mHandler = null;
        Log.i(TAG, "TestThread constructed");
    }

    //This method will return the testing handler
    //It prevents things from attempting to get the handler before the subsytem has had a chance to
    //create itself.
    public Handler getHandler()
    {
        lock.lock();
        try {
            while (mHandler == null){
                notReady.await();
            }
            return mHandler;
        }
        catch (Exception e)
        {
            Log.e(TAG,"Could not handle condition variable", e);
        }
        finally {
            lock.unlock();
        }
        return null;
    }
    //The main event loop that the thread takes
    //It enters the loopers and then handles requests untill exit
    @Override
    public void run() {
        lock.lock();
        Log.i(TAG, "Running Thread");
        Looper.prepare();
        mHandler = new TestMessageHandler();
        notReady.signalAll();
        lock.unlock();
        Looper.loop();
    }

    synchronized public String name(){
        return mName;
    }

}