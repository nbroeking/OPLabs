package tester;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * Created by nbroeking on 4/1/15.
 * This class specifies what we contain in a test Result
 */
public class TestResults implements Parcelable{

    private int averageDNSResponseTime;
    private double packetLoss;
    private boolean valid;

    public TestResults(){
        valid = false;
    }
    public double getPacketLoss() {
        return packetLoss;
    }

    public void setPacketLoss(double packetLoss) {
        this.packetLoss = packetLoss;
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {

    }

    public int getAverageDNSResponseTime() {
        return averageDNSResponseTime;
    }

    public void setAverageDNSResponseTime(int averageDNSResponseTime) {
        this.averageDNSResponseTime = averageDNSResponseTime;
    }

    public boolean isValid() {
        return valid;
    }

    public void setValid() {
        this.valid = true;
    }
}
