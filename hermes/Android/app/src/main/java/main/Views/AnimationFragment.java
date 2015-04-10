package main.Views;

import android.support.v4.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.oplabs.hermes.R;

/**
 * This class controls the animation for the Results Activity
 */
public class AnimationFragment extends Fragment {

    public AnimationFragment() {
    }

    private static TextView textview;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_results, container, false);

        textview = (TextView) view.findViewById(R.id.results);

        return view;
    }

    public void update(String text) {
        textview.setText(text);
    }
}
