package communication;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

public class CommMessageHandler extends Handler {

	final private int QUIT = 0;

	@Override
	public void handleMessage(Message msg) {
        switch (msg.what) {
            // things that this thread should do
        
            case QUIT:
                Looper.myLooper().quit();
                break;   
                
        }
	}
}
