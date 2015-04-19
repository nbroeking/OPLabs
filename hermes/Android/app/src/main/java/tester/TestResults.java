package tester;

import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;

/**
 * Created by nbroeking on 4/1/15.
 * This class specifies what we contain in a test Result
 */
public class TestResults implements Parcelable{
    private double dns;
    private double packetLoss;
    private double latency;
    private double latencyUnderLoad;
    private double throughputUpload;
    private double throughputDownload;
    private double packetLossUnderLoad;

    private boolean valid;

    private int mobileId;
    private int router_id;

    public TestResults(int id){
        valid = false;
        packetLoss = -1;
        dns = -1;
        latency = -1;
        latencyUnderLoad = 0;
        latency = 0;

        this.mobileId = id;
        Log.i("CREATE", "ID = " + id);
    }

    public TestResults(Parcel in){
        dns = in.readDouble();
        packetLoss = in.readDouble();
        latency = in.readDouble();
        latencyUnderLoad = in.readDouble();
        throughputUpload = in.readDouble();
        throughputDownload = in.readDouble();
        packetLossUnderLoad = in.readDouble();
        valid = in.readByte() != 0; //Resets the bool

        mobileId = in.readInt();
        router_id = in.readInt();
    }

    public String printValues()
    {
        return "VALID = " + valid + " dns = " + dns + " packetLoss " + packetLoss+  " latency " + latency;
    }
    public TestResults(JSONObject json){
        try {
            if (json.getString("status").equals("success")) {

                dns = json.getDouble("dns_response_avg");
                packetLoss = json.getDouble("packet_loss");
                latency = json.getDouble("latency_avg");
                throughputDownload = json.getDouble("download_throughputs_avg");
                throughputUpload = json.getDouble("upload_throughputs_avh");
                packetLossUnderLoad = json.getDouble("packet_loss_under_load");
                latencyUnderLoad = json.getDouble("download_latencies_avg");
                valid = true;
                return;
            }
            else
            {
                throw new JSONException("Failure");
            }
        } catch (JSONException e) {
            valid = false;
            packetLoss = -1;
            dns = -1;
            latency = -1;
            throughputUpload = -1;
            throughputDownload = -1;
            packetLossUnderLoad = -1;
            latencyUnderLoad = -1;
            Log.e("JSON ERROR", "Test Results constructor failed" ,e);
        }
    }
    public String getPost(){

        if( !valid){
            return "state=error";
        }
        String results = new String();

        try {
            results += "state=finished&";
            results += "dns_response_avg=" + URLEncoder.encode(Double.toString(dns), "UTF-8") + "&";
            results += "packet_loss="+ URLEncoder.encode(Double.toString(packetLoss), "UTF-8") + "&";
            results += "latency_avg=" + URLEncoder.encode(Double.toString(latency), "UTF-8") + '&';
            results += "upload_throughputs=" + URLEncoder.encode(Double.toString(throughputUpload), "UTF-8") + '&';
            results += "download_throughputs=" + URLEncoder.encode(Double.toString(throughputDownload), "UTF-8") + '&';
            results += "packet_loss_under_load=" + URLEncoder.encode(Double.toString(packetLossUnderLoad), "UTF-8") + '&';
            results += "throughput_latency=" + URLEncoder.encode(Double.toString(latencyUnderLoad), "UTF-8");

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

        dest.writeDouble(dns);
        dest.writeDouble(packetLoss);
        dest.writeDouble(latency);
        dest.writeInt(mobileId);
        dest.writeInt(router_id);

        dest.writeDouble(dns);
        dest.writeDouble(packetLoss);
        dest.writeDouble(latency);
        dest.writeDouble(latencyUnderLoad);
        dest.writeDouble(throughputUpload);
        dest.writeDouble(throughputDownload);
        dest.writeDouble(packetLossUnderLoad);
        dest.writeByte((byte) (valid ? 1 : 0));

        dest.writeInt(mobileId);
        dest.writeInt(router_id);
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

    public double getDns() {
        synchronized (this) {
            return dns;
        }
    }

    public void setDns(double dns) {
        synchronized (this) {
            this.dns = dns;
        }
    }

    public boolean isValid() {
        synchronized (this){
            return valid;
        }
    }
    public void setValid(Boolean val) {
        synchronized (this) {
            this.valid = val;
        }
    }

    public double getLatency() {

        synchronized (this) {
            return latency;
        }
    }

    public void setLatency(double latency) {
        synchronized (this) {
            this.latency = latency;
        }
    }

    public int getMobileId() {
        synchronized (this) {
            return mobileId;
        }
    }

    public void setMobileId(int id) {
        synchronized (this) {
            this.mobileId = id;
        }
    }

    public int getRouter_id() {
        synchronized (this) {
            return router_id;
        }
    }

    public void setRouter_id(int router_id) {
        synchronized (this) {
            this.router_id = router_id;
        }
    }

    public double getLatencyUnderLoad() {
        synchronized (this) {
            return latencyUnderLoad;
        }
    }

    public void setLatencyUnderLoad(double latencyUnderLoad) {
        synchronized (this) {
            this.latencyUnderLoad = latencyUnderLoad;
        }
    }

    public double getThroughputUpload() {
        synchronized (this) {
            return throughputUpload;
        }
    }

    public void setThroughputUpload(double throughputUpload) {
        synchronized (this) {
            this.throughputUpload = throughputUpload;
        }
    }

    public double getThroughputDownload() {
        synchronized (this) {
            return throughputDownload;
        }
    }

    public void setThroughputDownload(double throughputDownload) {
        synchronized (this) {
            this.throughputDownload = throughputDownload;
        }
    }

    public double getPacketLossUnderLoad() {
        synchronized (this) {
            return packetLossUnderLoad;
        }
    }

    public void setPacketLossUnderLoad(double packetLossUnderLoad) {
        synchronized (this) {
            this.packetLossUnderLoad = packetLossUnderLoad;
        }
    }
}
