package main.Views;

import android.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.oplabs.hermes.R;

import tester.TestResults;
import tester.TestState;


public class ResultsFragment extends Fragment {

    public ResultsFragment() {
    }

    private TestState stateMachine = TestState.getInstance();

    private static TextView textview;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_results, container, false);

        textview = (TextView) view.findViewById(R.id.results);

        TestResults results = stateMachine.getLatestResults();
        update("DNS Response = " + results.getAverageDNSResponseTime() + " ms\nLatency = " + results.getLatency() + " ms\nPacket Loss = " + results.getPacketLoss() + "%");

        return view;
    }

    public void update(String text) {
        textview.setText(text);
    }
}
