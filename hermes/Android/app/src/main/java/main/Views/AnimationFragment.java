package main.Views;

import android.app.Fragment;
import android.graphics.drawable.AnimationDrawable;
import android.nfc.Tag;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebView;
import android.widget.ImageView;
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
    private TestState stateMachine = TestState.getInstance();

    private TextView stateView;

    public AnimationFragment() {
    }

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

    @Override
    public void onStart() {
        super.onStart();
        //timer.schedule(timerTask, 250);
    }


    @Override
    public void onStop() {
        super.onStop();
        //timer.cancel();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View aniView = inflater.inflate(R.layout.fragment_animation, container, false);

        ImageView aniImg = (ImageView) aniView.findViewById(R.id.theAnimation);
        //aniImg.setBackgroundResource(R.drawable.loading_animation);

        //AnimationDrawable animation = (AnimationDrawable) aniImg.getBackground();

        aniImg.setAdjustViewBounds(true);
        aniImg.setScaleType(ImageView.ScaleType.FIT_CENTER);
        aniImg.setImageDrawable(getResources().getDrawable(R.drawable.loading_animation));

        AnimationDrawable animation = (AnimationDrawable) aniImg.getDrawable();

        animation.start();

        stateView = (TextView)aniView.findViewById(R.id.textView);
        stateView.setText("Preparing");

        return aniView;
    }
}
