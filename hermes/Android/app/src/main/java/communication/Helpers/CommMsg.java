//This class specifies the messages you can send to the Handler
//Messages must contains ints. That is why this is not an enum
package communication.Helpers;
public final class CommMsg {

    public static final int QUIT = 0;
    public static final int LOGIN = 1;
    public static final int REQUEST_TEST = 2;
    public static final int REPORT_TEST = 3;
    public static final int REQUEST_ROUTER_RESULTS = 4;

}