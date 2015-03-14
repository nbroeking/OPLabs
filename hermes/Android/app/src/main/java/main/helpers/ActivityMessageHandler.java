package main.helpers;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

import interfaces.CommunicationDelegate;

//This handler is running on the main thread and can call UI methods
public class ActivityMessageHandler extends Handler {

    private CommunicationDelegate delegate;

    public ActivityMessageHandler(CommunicationDelegate value) {
        delegate = value;
    }

    @Override
    public void handleMessage(Message msg) {
        switch (msg.what) {
            // things that this thread should do
            case MainMsg.NOTIFYLOGIN:
                delegate.notifyComm();
                break;

            default:
                Log.e("ACTIVITY MESSAGE HANDLER", "Received an unknown message");
        }
    }
}