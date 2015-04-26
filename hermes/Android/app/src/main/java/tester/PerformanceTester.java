package tester;

import android.nfc.Tag;
import android.util.Log;

import org.apache.http.util.ByteArrayBuffer;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
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
 * First we run a DNS Response test
 * then latency test
 * calculate packet loss
 * at the same time then we run a throughput test
 * and a load test
 */

//NOTE: There is a bug with the upload throughput test
//Java does not let us know how many bytes we wrote so we estimate
public class PerformanceTester {
    private final static String TAG = "Performance Tests";
    private TestSettings settings;
    private DatagramSocket socket;
    private long timeStart;
    private long timeEnd;
    private boolean finished;

    //Initialize the performance test
    public PerformanceTester(TestSettings settings1)
    {
        socket = null;
        settings = settings1;
        timeStart = timeEnd = 0;

    }

    //Runs all the tests
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
            List<Integer> times1 = runDNSTest(settings.getInvalidDomains(), false);
            double dnsResult = 0;
            for (Integer x : times1) {
                dnsResult += x;
            }
            if( times1.size() == 0)
            {
                results.setDns(0.0);
                results.setDnsSD(0.0);
            }
            else {
                dnsResult /= times1.size();
                results.setDns(dnsResult);
                results.setDnsSD(this.standerdDev(times1));
            }
            //Start Testing Latency
            state.setState(TestState.State.TESTINGLATENCY, false);
            //Run a test packet latency test
            List<Integer> times2 = runDNSTest(settings.getValidDomains(), false);
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

            //Set the stddev for the Latency test
            results.setLatencySD(standerdDev(times2));

            //Packet Loss is just 1 - times we have / times we should have
            results.setPacketLoss((1 - ((double)(times2.size() + times1.size()) / (double)(settings.getValidDomains().size() + settings.getInvalidDomains().size()))));

        }
        catch(Exception e){
            Log.e(TAG, "Error running test", e);
            results.setValid(false);
        }

        //Start testing throughput
        state.setState(TestState.State.TESTINGTHROUGHPUT, false);

        //Create two threads one for throughput and one for load
        Thread throughput = new Thread(new ThroughputHelper(results));
        Thread load = new Thread(new LoadHelper(results));

        //Start them
        throughput.start();
        load.start();

        //Attempt to join with he two threads
        try {
            load.join();
            throughput.join();
            Log.d(TAG, "Successfully joined with the throughput test threads");
        }
        catch (Exception e){
            Log.e(TAG, "Error joining threads",e);
        }

        Log.i(TAG, "Results = " + results.printValues());
        return results;
    }

    //This class implements runnable that runs the throughput test
    private class ThroughputHelper implements Runnable {

        private TestResults results;

        InputStream inFromServer;
        OutputStream outToServer;

        public ThroughputHelper(TestResults tmpResults){
            results = tmpResults;
        }

        //Run in a new thread
        @Override
        public void run() {
            Log.i(TAG, "Running Throughput Test");

            ByteArrayBuffer buffer = new ByteArrayBuffer(1024);
            Socket clientSocket = null;

            try {
                //Initialize the socket
                clientSocket = new Socket();
                clientSocket.setSoTimeout(10000);

                InetAddress throughputServer = InetAddress.getByName(settings.getThroughputServer());
                clientSocket.connect(new InetSocketAddress(throughputServer, settings.getPort()), 1000);

                outToServer = clientSocket.getOutputStream();
                inFromServer = clientSocket.getInputStream();


                //Create the buffers to read and write too
                byte[] bytes = new byte[1024*1024];
                byte[] writeBytes = new byte[1024*1024];
                for(int i = 0 ; i < writeBytes.length; i++) {
                    writeBytes[i] = 'Z';
                }

                //Set a timeout in case the sockets block
                finished = false;

                //We spawn a new thread that handles the timeout
                //This is not the cleanest way to do things
                //I am sorry for the sleep but Java sockets
                //don't support write timeouts
                Thread timeout = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        synchronized (PerformanceTester.this) {

                            try {
                                Thread.sleep(23000);
                            } catch (Exception e) {
                                Log.e(TAG, "Error with socket timeout");
                            }
                            finally {
                                //This will timeout the socket
                                if( !finished) {
                                    try{
                                    socket.close();
                                    outToServer.close();
                                    inFromServer.close();
                                    }
                                    catch (Exception e){
                                        Log.e(TAG, "Error closing streams in timeout");
                                    }
                                    Log.i(TAG, "We timed out our socket");
                                }
                            }
                        }
                    }
                });
                timeout.start();

                //Running a download test
                int totalBytesRead = 0;
                long startTime = System.currentTimeMillis();
                try {
                    //Read as many bytes as we can in 10 seconds
                    while ((System.currentTimeMillis() - startTime) < 10000) {
                        //int bytesRead = inFromServer.read(bytes, 0, 1024 * 1024);
                        int bytesRead = inFromServer.read(bytes, 0, 1024*1024);
                        totalBytesRead += bytesRead;
                    }
                }
                catch (SocketTimeoutException e){
                    Log.w(TAG, "Socket Read Timeout");
                }

                //Need to report in bytes per second
                results.setThroughputDownload((double)totalBytesRead/(((System.currentTimeMillis()) - startTime)/1000.0));
                Log.i(TAG, "Total Bytes Read = " + totalBytesRead);

                //Running an upload test
                long totalBytesWritten = 0;

                startTime = System.currentTimeMillis();
                while( (System.currentTimeMillis() - startTime) < 10000) {
                    //Write as many bytes as we can in 10 seconds
                    outToServer.write(writeBytes);
                    totalBytesWritten += writeBytes.length;
                }
                long endTime = System.currentTimeMillis();

                //prevent the timeout from screwing us up
                synchronized (this){
                    finished = true;
                }

                Log.i(TAG, "Total Bytes written = " + totalBytesWritten);
                results.setThroughputUpload((double)totalBytesWritten/(double)((endTime - startTime)/1000.0));

                try {
                    Log.i(TAG, "Joining timeout thread");
                    timeout.join();
                    Log.i(TAG, "Joined timeout thread");
                } catch (InterruptedException e) {
                    Log.e(TAG, "Could not join timeout thread");
                }


            } catch (IOException e) {
                Log.e(TAG, "Error with throughput Test");
                results.setValid(false);
            }
            //Close down the sockets
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

    //This class runs the load test at the same time as the throughput test
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

            //For the download test we want to run a latency test the same way as above
            List<Integer> times = new ArrayList<Integer>();
            while ( System.currentTimeMillis() - startTime < 10000){
                List<Integer> resTimes = runDNSTest(settings.getValidDomains(), true);
                times.addAll(resTimes);
                tests += 1;
            }

            if(times.size() ==0 ){
                results.setLatencyUnderLoad(new ArrayList<Integer>());
            }
            else {
                results.setLatencyUnderLoad(times);
            }
            results.setPacketLossUnderLoad(1.0-((double)times.size() / (double)(tests*settings.getValidDomains().size())));
            Log.d(TAG, "Load Completed");
        }
    };

    //TO run a dns test we hand craft a dns packet and send it via UDP
    //We then time the time it takes to respond
    public List<Integer> runDNSTest(List<String> domains, Boolean needsDelay)
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
                    if( needsDelay){
                        try {
                            Thread.sleep(200);
                        }
                        catch (Exception e){
                            Log.e(TAG, "Error sleeping in dns test");
                        }
                    }
                }
            }
        }
        return resultTimes;
    }

    //Sends the string to to the dns server specified in the settings object
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

    //Gets the dns response for the correct id
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

    //Returns a dns byte stream for the specified id
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
    /**
     * This method calculates the standard Deviation for a list
     */
    private double standerdDev(List<Integer> lst){
        //Calculate the mean
        double mean = 0.0;
        for( int x: lst){
            mean += (double)x;
        }
        mean = mean/ lst.size();

        //Calculate the dev
        double dev = 0.0;
        for( int x: lst){
            dev+= Math.pow(((double)x-mean),2);
        }
        dev = dev / ( lst.size()-1);
        dev = Math.sqrt(dev);

        return dev;
    }
}
