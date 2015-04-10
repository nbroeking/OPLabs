package tester;

import android.nfc.Tag;
import android.util.Log;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketTimeoutException;
import java.util.ArrayList;
import java.util.List;

import tester.helpers.TestSettings;

/**
 * Created by nbroeking on 4/1/15.
 * This handles all the performance tests
 */
public class PerformanceTester {
    private final static String TAG = "Performance Tests";
    private TestSettings settings;
    private DatagramSocket socket;

    private long timeStart;
    private long timeEnd;

    public PerformanceTester(TestSettings settings1)
    {
        socket = null;
        settings = settings1;
        timeStart = timeEnd = 0;
    }

    //Return dns, packet latency, packet jitter, packet loss
    public TestResults runTests()
    {
        TestResults results = new TestResults(settings.getResultID());
        results.setValid();

        //Run a dns response Test
        List<Integer> times1 = runDNSTest(settings.getInvalidDomains());
        int dnsResult = 0;
        for (Integer x : times1)
        {
            dnsResult += x;
        }
        dnsResult /= times1.size();
        results.setAverageDNSResponseTime(dnsResult);
        Log.d(TAG, "DNS Result = " + dnsResult);

        //Run a test packet latency test
        List<Integer> times2 = runDNSTest(settings.getValidDomains());
        int latencyResult = 0;
        for (Integer x: times2)
        {
            latencyResult+=x;
        }
        latencyResult/= times2.size();
        results.setLatency(latencyResult);
        Log.d(TAG, "Latency Result = " + latencyResult);

        //Packet Loss is just 1 - times we have / times we should have
        results.setPacketLoss((1- (times2.size()/settings.getValidDomains().size())));
        Log.d(TAG, "Packet Loss = " + results.getPacketLoss());

        //TODO: Run a packet jitter response

        //TODO: Run a throughput response

        return results;
    }

    public List<Integer> runDNSTest(List<String> domains)
    {
        List<Integer> resultTimes = new ArrayList<Integer>();
        //Run a dns test
        short id = 0;
        for( String x : domains) {
            id += 1;
            if( sendDNSRequest(x,id)){
                if( getDNSResponse(id))
                {
                    long elapsedTime = timeEnd - timeStart;

                    resultTimes.add((int)elapsedTime);
                }
            }
        }

        return resultTimes;
    }
    // Throughput
    public TestResults runThroughputTest()
    {
        Log.i(TAG, "Run Throughput Test");
        return null;
    }

    public boolean sendDNSRequest(String string, short id){
        boolean returncode = true;
        socket = null;
        try {
            socket = new DatagramSocket();
            InetAddress ip = InetAddress.getByName(settings.getDNSServer());

            //Construct the DNS packets
            byte[] sendData = getContent(string, id);
            DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, ip, 53);
            timeStart = System.currentTimeMillis();
            socket.send(sendPacket);
        }
        catch(Exception e){
            Log.e(TAG,"Error with DNS", e);
            returncode = false;
            socket.close();
        }
        return returncode;
    }

    public boolean getDNSResponse(short x){
        try {
            short id = -1;

            DatagramPacket receivePacket = null;
            while(id != x) {
                byte[] buffer = new byte[128];
                receivePacket = new DatagramPacket(buffer, buffer.length);
                socket.setSoTimeout(settings.getTimeout());
                socket.receive(receivePacket);
                byte[]data = receivePacket.getData();

                id = ((short)(((int)data[0] << 8) | ((int)data[1])));

                //Testing bytes
               /* StringBuilder sb = new StringBuilder();
                for (byte b : receivePacket.getData()) {
                    sb.append(String.format("%02X ", b));
                }
                Log.i(TAG, "Server said: "+ sb.toString());*/
            }
            //Check if its valid
            short valid = (short)(receivePacket.getData()[3]);
            valid = (short)((int)valid & 0x000F);

            if( valid != 0 && valid != 3)
            {
                Log.e(TAG, "DNS error"+ valid);
                return false;
            }
            timeEnd = System.currentTimeMillis();
        }
        catch (SocketTimeoutException e)
        {
            Log.e(TAG, "Socket TIMEOUT");
            return false;
        }
        catch (Exception e){
            Log.e(TAG, "Couldn't resolve hostname", e);
            return false;
        }
        finally {
            socket.close();
        }
        return true;
    }

    private byte[] getContent(String name, short id)
    {
        String[] lists = name.split("\\.");


        byte[] bytes = new byte[18 + name.length() + lists.length + 1 ];

        //Set the id
        bytes[0] = (byte)((id>>8)&0xFF);
        bytes[1] = (byte)(id&0xFF);

        //Set the flags
        bytes[2] = 0x01;
        bytes[3] = bytes[4]= 0x00;
        bytes[5] = 0x01;

        for( int i = 0; i < 6; i++)
        {
            bytes[6+i] = 0x00;
        }

        //Add the sections
        int index = 12;

        for( String section: lists)
        {
            bytes[index] = (byte)section.length();
            index++;
            for(int i = 0; i < section.length(); i++)
            {
                bytes[index + i] = (byte)section.toCharArray()[i];
            }
            index+= section.length();
        }
        //Add another 0 to mark the end of the section

        bytes[index] = 0x00;
        index++;

        //Set trailer

        bytes[index] = 0x00;
        bytes[index+1] = 0x01;
        bytes[index+2] = 0x00;
        bytes[index+3] = 0x01;
        return bytes;
    }
}
