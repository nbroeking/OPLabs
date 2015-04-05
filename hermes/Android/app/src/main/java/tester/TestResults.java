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

    private int averageDNSResponseTime;
    private double packetLoss;
    private boolean valid;

    public TestResults(){
        valid = false;
        packetLoss = 0;
        averageDNSResponseTime = 0;
    }

    public TestResults(Parcel in){

        averageDNSResponseTime = in.readInt();
        packetLoss = in.readDouble();
        valid = in.readByte() != 0; //Resets the bool

        Log.d("TEST Parcel", "Checking: " + averageDNSResponseTime);

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
        dest.writeInt(averageDNSResponseTime);
        dest.writeDouble(packetLoss);
        dest.writeByte((byte) (valid ? 1 : 0));
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
