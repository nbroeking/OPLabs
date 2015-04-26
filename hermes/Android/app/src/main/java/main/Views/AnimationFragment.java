package main.Views;

import android.app.Fragment;
import android.graphics.drawable.AnimationDrawable;
import android.nfc.Tag;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebView;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.oplabs.hermes.R;

import java.util.Timer;
import java.util.TimerTask;

import tester.TestState;

/**
 * This class controls the animation for the Results Activity
 */
public class AnimationFragment extends Fragment {

    private final static String Tag ="Animation Frag";
    private TextView stateView;

    //Call backs to update the progress bar and the text field
    private final Handler handler = new Handler();
    private final long totalTime = 35000; //Milliseconds its hardcoded right now im sorry
    private long start = 0;
    private boolean shouldUpdate;
    private AnimationDrawable animation;
    private ProgressBar bar;
    private static Boolean started = false;

    final Runnable updater;

    //When we create ourselves we need to create the updater that updates the progress bar
    //I know this is polling and I am embarrassed by this but we need a way to seamlessly update
    //these values so this is a quick and dirty way to do this.
    public AnimationFragment() {
        shouldUpdate = true;
        updater = new Runnable() {
            @Override
            public void run() {
                stateView.setText(TestState.getInstance().getStateAsString());

                long timediff = System.currentTimeMillis() - TestState.getInstance().getStartTime();
                double progress = (double)((((double)timediff)/(double)totalTime));
                bar.setProgress((int)(100*progress));

                if( timediff >= totalTime){
                    shouldUpdate = false;
                }
                if( shouldUpdate) {
                    handler.postDelayed(this, 34);
                }
                else{
                    Log.d(Tag, "Done updating the progress bar");
                }
            }
        };
    }

    //Create the timer and set the updater
    private Timer timer = new Timer();
    private TimerTask timerTask = new TimerTask() {
        @Override
        public void run() {

            getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    stateView.setText(TestState.getInstance().getStateAsString());
                }
            });
            Log.d(Tag, "Resetting Timer");
        }
    };

    //When we create the view we have to load the animation and the progress bar
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View aniView = inflater.inflate(R.layout.fragment_animation, container, false);

        ImageView aniImg = (ImageView) aniView.findViewById(R.id.theAnimation);

        aniImg.setAdjustViewBounds(true);
        aniImg.setScaleType(ImageView.ScaleType.FIT_CENTER);
        aniImg.setImageDrawable(getResources().getDrawable(R.drawable.loading_animation));

        animation = (AnimationDrawable) aniImg.getDrawable();

        stateView = (TextView)aniView.findViewById(R.id.textView);
        stateView.setText("Preparing");

        //Set the progress bar
        bar = (ProgressBar)aniView.findViewById(R.id.progressBar);
        bar.setIndeterminate(false);
        bar.setMax(100);
        return aniView;
    }

    //On start we start updating the animation and the updater
    @Override
    public void onStart() {
        super.onStart();
        animation.start();

        shouldUpdate = true;
        handler.postDelayed(updater, 34);
    }

    //When the activity stops we stop all animation so we don't waist cpu
    @Override
    public void onStop() {
        super.onStop();
        //timer.cancel();
        animation.stop();
        shouldUpdate = false;

        if( TestState.getInstance().getState() == TestState.State.COMPLETED){
            started = false;
        }
    }
}
