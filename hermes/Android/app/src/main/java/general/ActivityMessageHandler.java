package communication;

import android.os.Handler;
import android.os.Message;

import interfaces.CommunicationDelegate;
import main.MainMsg;
import main.SessionData;

public class ActivityMessageHandler extends Handler {

    private SessionData data;
    private CommunicationDelegate delegate;

    public ActivityMessageHandler(CommunicationDelegate value) {
        data = SessionData.getInstance();
        delegate = value;
    }

    @Override
    public void handleMessage(Message msg) {
        switch (msg.what) {
            // things that this thread should do
            case MainMsg.NOTIFYLOGIN:
                delegate.notifyLogin();
                break;
        }
    }
}