package tester;

import android.util.Log;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketTimeoutException;

import tester.helpers.TestSettings;

/**
 * Created by nbroeking on 4/1/15.
 * This handles all the performance tests
 */
public class PerformanceTester {
    private final static String TAG = "Performance Tests";
    private TestSettings settings;
    private DatagramSocket socket;
    public PerformanceTester(TestSettings settings1)
    {
        socket = null;
        settings = settings1;
    }

    //Return dns, packet latency, packet jitter, packet loss
    public TestResults runUDPTest()
    {
        for( String x : settings.getValidDomains()) {

            //TODO:Check if should close

            if( sendDNSRequest(x)){
                getDNSResponce();
            }
        }
        return null;
    }

    // Throughput
    public TestResults runThroughputTest()
    {
        Log.i(TAG, "Run Throughput Test");
        return null;
    }

    public boolean sendDNSRequest(String string){
        Log.i(TAG, "Testing String: " + string);

        boolean returncode = true;
        socket = null;
        try {
            socket = new DatagramSocket();
            InetAddress ip = InetAddress.getByName(settings.getDNSServer());

            //Construct the DNS packets
            byte[] sendData = getContent(string);
            DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, ip, 53);
            socket.send(sendPacket);
        }
        catch(Exception e){
            Log.e(TAG,"Error with DNS", e);
            returncode = false;
            socket.close();
        }
        return returncode;
    }

    public String getDNSResponce(){
        try {
            Log.i(TAG, "GETTING RESPONCE");
            byte[] receiveData = new byte[1024];
            DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
            socket.setSoTimeout(2000);
            socket.receive(receivePacket);
            String modifiedSentence = new String(receivePacket.getData());
            Log.i(TAG, "Server said: "+ modifiedSentence);
        }
        catch (SocketTimeoutException e)
        {
            Log.e(TAG, "Socket TIMEOUT");
        }
        catch (Exception e){
            Log.e(TAG, "Couldn't resolve hostname", e);
        }
        finally {
            socket.close();
        }
        return null;
    }

    private byte[] getContent(String name)
    {
        byte[] bytes = new byte[18 + name.length() ];

        //Set the id
        bytes[0] = 0x1a;
        bytes[1] = 0x52;

        //Set the flags
        bytes[2] = 0x01;
        bytes[3] = bytes[4]= 0x00;
        bytes[5] = 0x01;

        for( int i = 0; i < 6; i++)
        {
            bytes[6+i] = 0x00;
        }
        bytes[12] = 0x0A;

        for(int i = 0; i < name.length(); i++)
        {
            bytes[13 + i] = (byte)name.toCharArray()[i];
        }

        //Set tailier

        bytes[name.length()] = 0x00;
        bytes[name.length()+1] = 0x01;
        bytes[name.length()+2] = 0x00;
        bytes[name.length()+3] = 0x01;
        return bytes;
    }
}
