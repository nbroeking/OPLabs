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

    private static TextView mobileDNS;
    private static TextView mobileLatency;
    private static TextView mobilePackets;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_results, container, false);

        mobileDNS = (TextView) view.findViewById(R.id.dnsResponse);
        mobileLatency = (TextView) view.findViewById(R.id.latency);
        mobilePackets = (TextView) view.findViewById(R.id.packetLoss);

        TestResults results = stateMachine.getLatestResults();
        update(String.format("%s ms", results.getAverageDNSResponseTime()),
               String.format("%s ms", results.getLatency()),
               String.format("%s", (results.getPacketLoss() * 100) + "%"));

        return view;
    }

    public void update(String dns, String lat, String pkl) {
        mobileDNS.setText(dns);
        mobileLatency.setText(lat);
        mobilePackets.setText(pkl);
    }
}
