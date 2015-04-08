package tester;

import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;

import java.io.Serializable;

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

    public TestResults(){
        valid = false;
        packetLoss = -1;
        averageDNSResponseTime = -1;
        latency = -1;
    }

    public TestResults(Parcel in){
        valid = in.readByte() != 0; //Resets the bool
        averageDNSResponseTime = in.readDouble();
        packetLoss = in.readDouble();
        latency = in.readDouble();
        jitter = in.readDouble();
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

    public double getJitter() {
        return jitter;
    }

    public void setJitter(double jitter) {
        this.jitter = jitter;
    }
}
