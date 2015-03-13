package tester;

public class TestState {
    public enum State {IDLE, COMPLETED, TESTINGPING}

    private State state;

    public TestState(){
        state = State.IDLE;
    }

    synchronized public State getState()
    {
        return state;
    }
    synchronized public void setState(State arg)
    {
        state = arg;
    }
}

