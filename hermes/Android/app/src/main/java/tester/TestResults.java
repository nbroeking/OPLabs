package tester;

import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by nbroeking on 4/1/15.
 * This class specifies what we contain in a test Result
 */
public class TestResults implements Parcelable{
    private double dns;
    private double packetLoss;
    private double latency;
    private List<Integer> latencyUnderLoad;
    private double throughputUpload;
    private double throughputDownload;
    private double packetLossUnderLoad;
    private double dnsSD;
    private double latencySD;

    private boolean valid;

    private int mobileId;
    private int router_id;
    private int setId;

    public TestResults(int id){
        valid = false;
        packetLoss = -1;
        dns = -1;
        latency = -1;
        latencyUnderLoad = null;
        latency = 0;

        this.mobileId = id;
        Log.i("CREATE", "ID = " + id);
    }

    public TestResults(Parcel in){
        dns = in.readDouble();
        packetLoss = in.readDouble();
        latency = in.readDouble();

        latencyUnderLoad = new ArrayList<>();
        int sizeOfList = in.readInt();
        for( int i = 0; i < sizeOfList; i++){
            latencyUnderLoad.add(in.readInt());
        }

        throughputUpload = in.readDouble();
        throughputDownload = in.readDouble();
        packetLossUnderLoad = in.readDouble();
        latencySD = in.readDouble();
        dnsSD = in.readDouble();
        valid = in.readByte() != 0; //Resets the bool

        mobileId = in.readInt();
        router_id = in.readInt();
    }

    public String printValues()
    {
        return "VALID = " + valid +
                "\ndns = " + dns +
                "\npacketLoss " + packetLoss+
                "\nlatency " + latency +
                "\ndownload " + throughputDownload /1000/1000*8 + "Mbps" +
                "\nupload " + throughputUpload /1000/1000*8 + "Mbps" +
                "\nlatency under load " + latencyUnderLoad +
                "\npacket loss under load " + packetLossUnderLoad +
                "\nlatency std " + latencySD +
                "\n dns std " + dnsSD;
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
            results += "throughput_latency=" + URLEncoder.encode(latencyUnderLoad.toString(), "UTF-8") + '&';
            results += "latency_sdev=" + URLEncoder.encode(Double.toString(latencySD), "UTF-8") + '&';
            results += "dns_response_sdev=" + URLEncoder.encode(Double.toString(dnsSD), "UTF-8");

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

        dest.writeInt(latencyUnderLoad.size());

        for( int x : latencyUnderLoad){
            dest.writeInt(x);
        }

        dest.writeDouble(throughputUpload);
        dest.writeDouble(throughputDownload);
        dest.writeDouble(packetLossUnderLoad);
        dest.writeDouble(latencySD);
        dest.writeDouble(dnsSD);
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

    public List<Integer> getLatencyUnderLoad() {
        synchronized (this) {
            return latencyUnderLoad;
        }
    }

    public void setLatencyUnderLoad(List<Integer> latencyUnderLoad) {
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

    public double getLatencySD() {
        synchronized (this) {
            return latencySD;
        }
    }

    public void setLatencySD(double latencySD) {
        synchronized (this) {
            this.latencySD = latencySD;
        }
    }
    public int getSetId() {

        synchronized (this){
            return setId;
        }
    }

    public void setSetId(int setId) {

        synchronized (this){
            this.setId = setId;
        }
    }

    public double getDnsSD() {
        synchronized (this) {
            return dnsSD;
        }
    }

    public void setDnsSD(double dnsSD) {
        synchronized (this) {
            this.dnsSD = dnsSD;
        }
    }
}
