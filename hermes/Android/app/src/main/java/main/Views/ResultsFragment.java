package main.Views;

import android.app.Fragment;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import com.oplabs.hermes.R;

import main.Application.SessionData;
import tester.TestState;

public class ResultsFragment extends Fragment {

    private WebView browser;
    private static final String ResultsURL = "/mobile/test_set/%d";

    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.fragment_results, container, false);
        browser = (WebView)view.findViewById(R.id.webView);

        String url = String.format("%s%s?user_token=%s", SessionData.getInstance().getHostname(),
                String.format(ResultsURL, TestState.getInstance().getPhoneResults().getSetId()),
                SessionData.getInstance().getSessionId());

        browser.getSettings().setLoadsImagesAutomatically(true);
        browser.getSettings().setJavaScriptEnabled(true);
        browser.setScrollBarStyle(View.SCROLLBARS_INSIDE_OVERLAY);
        browser.loadUrl(url);
        return view;
    }

    private class MyBrowser extends WebViewClient {
        @Override
        public boolean shouldOverrideUrlLoading(WebView view, String url) {
            view.loadUrl(url);
            Log.d("Results", "Should Load the url");
            return true;
        }
    }
}
