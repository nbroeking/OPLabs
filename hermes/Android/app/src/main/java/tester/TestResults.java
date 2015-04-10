package tester;

import android.nfc.Tag;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.Serializable;
import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;

/**
 * Created by nbroeking on 4/1/15.
 * This class specifies what we contain in a test Result
 */
public class TestResults implements Parcelable{

    private double averageDNSResponseTime;
    private double packetLoss;
    private double latency;
    private double jitter;
    private boolean valid;
    private int id;

    public TestResults(int id){
        valid = false;
        packetLoss = -1;
        averageDNSResponseTime = -1;
        latency = -1;
        this.id = id;
    }

    public TestResults(Parcel in){
        valid = in.readByte() != 0; //Resets the bool
        averageDNSResponseTime = in.readDouble();
        packetLoss = in.readDouble();
        latency = in.readDouble();

        Log.w("PARCEL CREATE", "VALID = " + valid + " dns = " + averageDNSResponseTime + " packetLoss " + packetLoss+  " latency " + latency);
    }

    public String printValues()
    {
        return "VALID = " + valid + " dns = " + averageDNSResponseTime + " packetLoss " + packetLoss+  " latency " + latency;
    }
    public TestResults(JSONObject json){
        try {
            if (json.getString("status").equals("success")) {

                averageDNSResponseTime = json.getDouble("dns_response_avg");
                packetLoss = json.getDouble("packet_loss");
                latency = json.getDouble("latency_avg");
                return;
            }
            else
            {
                throw new JSONException("Failure");
            }
        } catch (JSONException e) {
            valid = false;
            packetLoss = -1;
            averageDNSResponseTime = -1;
            latency = -1;
            Log.e("JSON ERROR", "Test Results constructor failed" ,e);
        }

    }
    public String getPost(){

        String results = new String();

        try {
            results += "dns_response_avg=" + URLEncoder.encode(Double.toString(averageDNSResponseTime), "UTF-8") + "&";
            results += "packet_loss="+ URLEncoder.encode(Double.toString(packetLoss), "UTF-8") + "&";
            results += "latency_avg=" + URLEncoder.encode(Double.toString(latency), "UTF-8");
        } catch (UnsupportedEncodingException e) {
            Log.e("Results", "Error creating post", e);
        }

        return results;
    }

    public double getPacketLoss() {
        return packetLoss;
    }

    public void setPacketLoss(double packetLoss) {
        this.packetLoss = packetLoss;
    }

    // 0 is the TestResults
    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte((byte) (valid ? 1 : 0));
        dest.writeDouble(averageDNSResponseTime);
        dest.writeDouble(packetLoss);
        dest.writeDouble(latency);
        dest.writeDouble(jitter);
    }

    //Create a parcel
    public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
        public TestResults createFromParcel(Parcel in) {
            return new TestResults(in);
        }

        public TestResults[] newArray(int size) {
            return new TestResults[size];
        }
    };

    public double getAverageDNSResponseTime() {
        return averageDNSResponseTime;
    }

    public void setAverageDNSResponseTime(double averageDNSResponseTime) {
        this.averageDNSResponseTime = averageDNSResponseTime;
    }

    public boolean isValid() {
        return valid;
    }
    public void setValid() {
        this.valid = true;
    }

    public double getLatency() {
        return latency;
    }

    public void setLatency(double latency) {
        this.latency = latency;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }
}
