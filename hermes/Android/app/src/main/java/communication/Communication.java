package communication;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

/**
 * Created by NBroeking on 2/15/15.
 */
public class Communication extends Service {
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        //TODO do something useful
        return Service.START_NOT_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        //TODO for communication return IBinder implementation
        return null;
    }
}
