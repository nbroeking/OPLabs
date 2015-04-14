package main.Views;

import android.app.AlertDialog;
import android.app.Fragment;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.oplabs.hermes.R;

import tester.TestResults;
import tester.TestState;


public class ResultsFragment extends Fragment {

    private static final String TAG = "Results Fragment";

    private TestState stateMachine = TestState.getInstance();

    private static TextView mobileDNS;
    private static TextView mobileLatency;
    private static TextView mobilePackets;
    private static TextView mobileLoad;
    private static TextView mobileThrough;

    private static TextView routerDNS;
    private static TextView routerLatency;
    private static TextView routerPackets;
    private static TextView routerLoad;
    private static TextView routerThrough;

    private static TextView diffDNS;
    private static TextView diffLatency;
    private static TextView diffPackets;
    private static TextView diffLoad;
    private static TextView diffThrough;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_results, container, false);

        // Set TextView IDs for mobile test results
        mobileDNS = (TextView) view.findViewById(R.id.dnsResponse);
        mobileLatency = (TextView) view.findViewById(R.id.latency);
        mobilePackets = (TextView) view.findViewById(R.id.packetLoss);
        mobileLoad = (TextView) view.findViewById(R.id.load);
        mobileThrough = (TextView) view.findViewById(R.id.throughput);

        //Get the router textfields
        routerDNS = (TextView) view.findViewById(R.id.dnsResponse2);
        routerLatency = (TextView) view.findViewById(R.id.latency2);
        routerPackets = (TextView) view.findViewById(R.id.packetLoss2);
        routerLoad = (TextView) view.findViewById(R.id.load2);
        routerThrough = (TextView) view.findViewById(R.id.throughput2);

        // Set TextView IDs for diff table
        diffDNS = (TextView) view.findViewById(R.id.dnsResponse3);
        diffLatency = (TextView) view.findViewById(R.id.latency3);
        diffPackets = (TextView) view.findViewById(R.id.packetLoss3);
        diffLoad = (TextView) view.findViewById(R.id.load3);
        diffThrough = (TextView) view.findViewById(R.id.throughput3);


        // Get latest results for mobile from TestResults
        TestResults results = stateMachine.getLatestResults();


        // Change TextView for mobile results
        updateMobile(String.format("%s ms", results.getAverageDNSResponseTime()),
                String.format("%s ms", results.getLatency()),
                String.format("%s", (results.getPacketLoss() * 100) + "%"), "0", "0");
                //String.format("%s", (results.getLoad())),
                //String.format("%s", (results.getThroughput())));

        // Hide Router and Difference tables
        // Show spinning wheel animation

        return view;
    }

    public void onRouterResults(TestResults results) {

        // Change TextView for router results
        updateRouter(String.format("%s ms", results.getAverageDNSResponseTime()),
                String.format("%s ms", results.getLatency()),
                String.format("%s", (results.getPacketLoss() * 100) + "%"),
                String.format("%s", (results.getLoad())),
                String.format("%s", (results.getThroughput())));

        updateDiffTable(String.format("%s ms", (subtractResults(routerDNS, mobileDNS))),
                String.format("%s ms", (subtractResults(routerLatency, mobileLatency))),
                String.format("%s", (subtractResults(routerPackets, mobilePackets)) + "%"),
                String.format("%s", (subtractResults(routerLoad, mobileLoad))),
                String.format("%s", (subtractResults(routerThrough, mobileThrough))));

        // Hide spinning wheel animation
        // Show Router and Difference Tables

    }

    public void updateMobile(String dns, String lat, String pkl, String ld, String thp) {
        mobileDNS.setText(dns);
        mobileLatency.setText(lat);
        mobilePackets.setText(pkl);
        mobileLoad.setText(ld);
        mobileThrough.setText(thp);
    }

    public void updateRouter(String dns, String lat, String pkl, String ld, String thp) {
        routerDNS.setText(dns);
        routerLatency.setText(lat);
        routerPackets.setText(pkl);
        routerLoad.setText(ld);
        routerThrough.setText(thp);
    }

    public void updateDiffTable(String dns, String lat, String pkl, String ld, String thp) {
        diffDNS.setText(dns);
        diffLatency.setText(lat);
        diffPackets.setText(pkl);
        diffLoad.setText(ld);
        diffThrough.setText(thp);
    }

    public int subtractResults(TextView view1, TextView view2) {
        return (Integer.parseInt(view1.getText().toString()) - Integer.parseInt(view2.getText().toString()));
    }
}
