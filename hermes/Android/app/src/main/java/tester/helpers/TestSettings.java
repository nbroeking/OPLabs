package tester.helpers;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by nbroeking on 4/1/15.
 * The settings that we use to run a performance test
 */
public class TestSettings {

    private List<String> validDomains;
    private List<String> invalidDomains;
    private String DNSServer;
    private int timeout;
    private int setId;
    private int routerResultsID;
    private int mobileResultsID;

    //Throughput settings
    private String throughputServer;
    private int port;

    public TestSettings(){
        validDomains = new ArrayList<String>();
        invalidDomains = new ArrayList<String>();
    }

    public void addValidDomain(String string){
        validDomains.add(string);
    }
    public void addInvalidDomain(String string){
        invalidDomains.add(string);
    }
    public List<String> getValidDomains(){
        return validDomains;
    }
    public List<String> getInvalidDomains(){
        return invalidDomains;
    }
    public int getTimeout(){return timeout;}
    public void setTimeout(int timeout1){ timeout = timeout1;}
    public void setDNSServer(String string)
    {
        DNSServer = string;
    }
    public String getDNSServer()
    {
        return DNSServer;
    }
    public int getSetId() {
        return setId;
    }
    public void setSetId(int setId) {
        this.setId = setId;
    }
    public int getRouterResultsID() {
        return routerResultsID;
    }
    public void setRouterResultsID(int routerResultsID) {
        this.routerResultsID = routerResultsID;
    }

    public int getMobileResultsID() {
        return mobileResultsID;
    }

    public void setMobileResultsID(int mobileResultsID) {
        this.mobileResultsID = mobileResultsID;
    }

    public String getThroughputServer() {
        return throughputServer;
    }

    public void setThroughputServer(String throughputServer) {
        this.throughputServer = throughputServer;
    }

    public int getPort() {
        return port;
    }

    public void setPort(int port) {
        this.port = port;
    }
}
