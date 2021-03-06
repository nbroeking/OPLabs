package main.Views;

import android.app.Fragment;
import android.net.http.SslError;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.SslErrorHandler;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import com.oplabs.hermes.R;

import main.Application.SessionData;
import tester.TestState;

//This class Controls the results fragment
//It displays the activity bar and the animation
public class ResultsFragment extends Fragment {

    private WebView browser;
    private static final String ResultsURL = "/mobile/test_set/%d";

    //When we create the view we want to load the results page
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        Log.d("Results", "Reload");
        View view = inflater.inflate(R.layout.fragment_results, container, false);
        browser = (WebView)view.findViewById(R.id.webView);

        String url = String.format("%s%s?user_token=%s", SessionData.getInstance().getHostname(),
                String.format(ResultsURL, TestState.getInstance().getPhoneResults().getSetId()),
                SessionData.getInstance().getSessionId());

        browser.setWebViewClient(new MyBrowser());

        browser.getSettings().setLoadsImagesAutomatically(true);
        browser.getSettings().setJavaScriptEnabled(true);
        browser.setScrollBarStyle(View.SCROLLBARS_INSIDE_OVERLAY);
        browser.loadUrl(url);

        return view;
    }

    //THis is a delegate that controls the WebView
    private class MyBrowser extends WebViewClient {
        @Override
        public void onReceivedError(WebView view, int errorCode, String description, String failingUrl) {
            super.onReceivedError(view, errorCode, description, failingUrl);
            Log.d("Results", "Error with browser");
        }

        //Load the URL
        @Override
        public boolean shouldOverrideUrlLoading(WebView view, String url) {
            view.loadUrl(url);
            Log.d("Results", "Should Load the url");
            return true;
        }

        //This method allows us to request webpages from self signed certs
        @Override
        public void onReceivedSslError (WebView view, SslErrorHandler handler, SslError error) {
            handler.proceed();
        }
    }
}
