package tester;

import android.util.Log;

import org.apache.http.util.ByteArrayBuffer;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
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
        TestState state = TestState.getInstance();
        TestResults results = new TestResults(settings.getMobileResultsID());
        try {

            state.setState(TestState.State.TESTINGDNS, false);

            //Init the results object
            results.setRouter_id(settings.getRouterResultsID());
            results.setValid(true);
            results.setSetId(settings.getSetId());

            //Run a dns response Test
            List<Integer> times1 = runDNSTest(settings.getInvalidDomains());
            double dnsResult = 0;
            for (Integer x : times1) {
                dnsResult += x;
            }

            if( times1.size() == 0)
            {
                results.setDns(0.0);
            }
            else {
                dnsResult /= times1.size();
                results.setDns(dnsResult);
            }
            Log.d(TAG, "DNS Result = " + dnsResult);

            state.setState(TestState.State.TESTINGLATENCY, false);
            //Run a test packet latency test
            List<Integer> times2 = runDNSTest(settings.getValidDomains());
            double latencyResult = 0;
            for (Integer x : times2) {
                latencyResult += x;
            }

            if( times2.size() == 0)
            {
                results.setLatency(0.0);
            }
            else {
                latencyResult /= times2.size();
                results.setLatency(latencyResult);
            }
            Log.d(TAG, "Latency Result = " + latencyResult);

            //Packet Loss is just 1 - times we have / times we should have
            results.setPacketLoss((1 - ((times2.size() + times1.size()) / (settings.getValidDomains().size() + settings.getInvalidDomains().size()))));
            Log.d(TAG, "Packet Loss = " + results.getPacketLoss());
        }
        catch(Exception e){
            Log.e(TAG, "Error running test", e);
            results.setValid(false);
        }
        state.setState(TestState.State.TESTINGTHROUGHPUT, false);

        Thread throughput = new Thread(new ThroughputHelper(results));
        Thread load = new Thread(new LoadHelper(results));

        throughput.start();
        load.start();

        try {
            load.join();
            throughput.join();
            Log.d(TAG, "Successfully joined with the throughput test threads");
        }
        catch (Exception e){
            Log.e(TAG, "Error joining threads",e);
        }
        return results;
    }

    private class ThroughputHelper implements Runnable {

        private TestResults results;

        public ThroughputHelper(TestResults tmpResults){
            results = tmpResults;
        }
        @Override
        public void run() {
            Log.i(TAG, "Running Throughput Test");

            ByteArrayBuffer buffer = new ByteArrayBuffer(1024);


            Socket clientSocket = null;
            BufferedReader inFromServer = null;
            DataOutputStream outToServer = null;
            try {
                clientSocket = new Socket();
                clientSocket.setSoTimeout(10000);

                InetAddress throughputServer = InetAddress.getByName(settings.getThroughputServer());
                clientSocket.connect(new InetSocketAddress(throughputServer, settings.getPort()), 1000);

                outToServer = new DataOutputStream(clientSocket.getOutputStream());
                inFromServer = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

                long startTime = System.currentTimeMillis();
                char[] bytes = new char[1024*1024];
                byte[] writeBytes = new byte[1024*1024];
                for(int i = 0 ; i < writeBytes.length; i++) {
                    writeBytes[i] = 'Z';
                }
                int totalBytesRead = 0;
                try {

                    while ((System.currentTimeMillis() - startTime) < 10000) {
                        int bytesRead = inFromServer.read(bytes, 0, 1024 * 1024);
                        totalBytesRead += bytesRead;
                    }
                }
                catch (SocketTimeoutException e){
                    Log.w(TAG, "Socket Read Timedout");
                }
                //Collect Results

                //Need to report in bytes per second
                results.setThroughputDownload((double)totalBytesRead/(double)((System.currentTimeMillis()*1000.0) - startTime*1000.0));
                Log.i(TAG, "Throughput Download = " + results.getThroughputDownload());

                long totalBytesWritten = 0;
                startTime = System.currentTimeMillis();
                while( (System.currentTimeMillis() - startTime) < 10000) {
                    outToServer.write(writeBytes, 0, writeBytes.length);
                    totalBytesWritten += writeBytes.length;
                    outToServer.flush();
                }

                results.setThroughputUpload((double)totalBytesWritten/(double)(System.currentTimeMillis()*1000 - startTime*1000));
                Log.i(TAG, "Throughput Upload= " + results.getThroughputDownload());


            } catch (IOException e) {
                Log.e(TAG, "Error with throughput Test");
                results.setValid(false);
            }
            finally {
                try {
                    if (clientSocket != null) {
                        clientSocket.close();
                    }
                    if (outToServer != null) {
                        outToServer.close();
                    }
                    if (inFromServer != null) {
                        inFromServer.close();
                    }
                    Log.i(TAG, "Closed Throughput sockets");
                }
                catch(Exception e) {
                    Log.e(TAG, "Could not close socket");
                }
            }
        }
    };

    private class LoadHelper implements Runnable {

        private TestResults results;

        public LoadHelper(TestResults tmpResults){
            results = tmpResults;
        }
        @Override
        public void run() {
            Log.i(TAG, "Running Load Test");
            long startTime = System.currentTimeMillis();
            int tests = 0;

            List<Integer> times = new ArrayList<Integer>();
            while ( System.currentTimeMillis() - startTime < 10000){
                try {
                    Thread.sleep(200);
                } catch (InterruptedException e) {
                    Log.e(TAG, "Error with Timing");
                }
                List<Integer> resTimes = runDNSTest(settings.getValidDomains());
                times.addAll(resTimes);
                tests += 1;
            }

            double latencyResult = 0;
            for (int x : times) {
                latencyResult += (double)x;
            }

            if(times.size() ==0 ){
                results.setLatencyUnderLoad(0.0);
            }
            else {
                latencyResult /= times.size();
                results.setLatencyUnderLoad(latencyResult);
            }

            Log.d(TAG, "Times = " + times);
            results.setPacketLossUnderLoad(1.0-((double)times.size() / (double)(tests*settings.getValidDomains().size())));

            Log.i(TAG, "Latency Under Load Result = " +latencyResult);
            Log.i(TAG, "Packet Loss under Load = " +results.getPacketLossUnderLoad());
        }
    };

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
            }
            //Check if its valid
            short valid = (short)(receivePacket.getData()[3]);
            valid = (short)((int)valid & 0x000F);

            if(valid != 0 && valid != 3)
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
        byte[] bytes = new byte[16 + name.length() +2 ];

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
