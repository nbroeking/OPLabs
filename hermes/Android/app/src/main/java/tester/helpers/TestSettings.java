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
    private boolean valid;

    public TestSettings(){
        validDomains = new ArrayList<String>();
        invalidDomains = new ArrayList<String>();
        valid = true;
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

    public void setTesting()
    {
        for( int i = 0; i < 3; i++)
        {
            validDomains.add("www.google.com");
            invalidDomains.add("www.yodelaho.com");
        }
    }
    public void setDNSServer(String string)
    {
        DNSServer = string;
    }
    public String getDNSServer()
    {
        return DNSServer;
    }


    public boolean isValid() {
        return valid;
    }

    public void setValid() {
        this.valid = true;
    }
}
