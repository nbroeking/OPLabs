package main.Views;

import android.app.Fragment;
import android.graphics.drawable.AnimationDrawable;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import com.oplabs.hermes.R;

import tester.TestState;

/**
 * This class controls the animation for the Results Activity
 */
public class AnimationFragment extends Fragment {

    private TestState stateMachine = TestState.getInstance();

    public AnimationFragment() {
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

        return aniView;
    }
}
