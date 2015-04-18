#ifndef STATEMACHINE_HPP_
#define STATEMACHINE_HPP_

/*
 * Author: jrahm
 * created: 2015/02/06
 * StateMachine.hpp: <description>
 */

#include <map>

#include <containers/BlockingQueue.hpp>

#include <log/LogManager.hpp>

#include <os/Runnable.hpp>
#include <os/Scheduler.hpp>
#include <os/ScopedLock.hpp>
#include <os/Time.hpp>

#include <proc/Process.hpp>
#include <typeinfo>

/**
 * @brief A class to handle access for an internal state machine
 *
 * This class is responsible for providing a thread-safe interface
 * to a class representing a state machine (defined by Internal_T).
 *
 * The state machine can be stimulated with various stims (defined by Stim_T)
 * and can have a variety of different states (defined by State_T)
 *
 * There is no <i>compilation</i> requirement about the interface of the internal
 * class, but, for logging purposes, Stim_T and State_T must define a
 * method std::string toString(Stim_T).
 *
 * To use the state machine, you must define the states and transitions.
 * To do thi we add a mapping of the form (State, Stim) -> Subroutine. As such,
 * you must set these edges with the setEdge method. This takes a state, stimulus
 * and a function and makes an entry stating when a stim of that type is received
 * in that state, call the function and the function returns the new state.
 *
 * @note It is very _important_ that the edge function do _not_ block.
 *
 * @param Stim_T must define std::string toString() and must be orderable
 * @param State_T must define std::string toString() and must be orderable
 */
template <class Stim_T, class Internal_T, class State_T>
class StateMachine: public os::ManagedRunnable {
public:
    StateMachine(Internal_T& internal, State_T initial_state, os::Scheduler& scheduler):
        m_delegate(internal),
        m_scheduler(scheduler) {
        m_timer.sm = this;
        m_timer.engaged = false;
        m_current_state = initial_state;

        m_log = &logger::LogManager::instance().getLogContext("Process", "StateMachine");
    }

    /**
     * @brief Send a stimulus to the state machine
     * @param stim The stimulus to send
     */
    void sendStimulus( Stim_T stim ) {
        Command cmd;
        cmd.type = STIM;
        cmd.un.stim = stim;

        m_log->printfln(TRACE, "[%p] Pushing onto command queue %s", this, toString(stim).c_str());
        m_command_queue.push(cmd);
    }

    /**
     * @brief Schedule a stim to be send after a certain timeout. This will be cleared if another stim comes first
     * 
     * @param timeout the amout of time to wait before sending the stim
     * @param to_send The stim to send
     */
    void setTimeoutStim( os::timeout_t timeout, Stim_T to_send ) {
        // os::ScopedLock __sl(m_mutex);

        schedule_timeout(timeout, to_send);   
    }

    void setEdge( State_T current_state, Stim_T stim, State_T (Internal_T::*fn)() ) {
        m_state_map[std::make_pair(current_state, stim)] = fn;
    }

    State_T getCurrentState() {
        return m_current_state;
    }

    /**
     * @brief Remove the currently active timeout
     */
    void clearTimeout() {
        if(m_timer.engaged) {
            m_log->printfln(DEBUG, "Clearing timeout stim %s", toString(m_timer.to_send).c_str());
            m_scheduler.cancel(&m_timer);
        }
    }

    void run() {
        m_log->printfln(TRACE, "Running");
        Command cmd ;

        while(1) {
            m_log->printfln(TRACE, "Waiting for next command");
            cmd = m_command_queue.front();
            m_command_queue.pop();
            m_log->printfln(TRACE, "Command read from queue");
            switch(cmd.type) {
            case EXIT:
                m_log->printfln(INFO, "Receieved request to stop");
                _cleanup();
                return;

            case STIM:
                Stim_T stim = cmd.un.stim;
                process_stim(stim);
                break;
            }
        }
    }

    void stop() {
        Command cmd;
        cmd.type = EXIT;
        m_command_queue.push(cmd);
    }

protected:
    logger::LogContext* m_log;

private:
    void _cleanup() {
        return;
    }

    void process_stim(Stim_T stim) {
        m_log->printfln(DEBUG, "<%s> - Processing stim %s", typeid(Internal_T).name(), toString(stim).c_str());
        typename StateMapT::iterator itr = m_state_map.find(std::make_pair(m_current_state, stim));

        if(itr == m_state_map.end()) {
            /* state undefined, return */
            m_log->printfln(WARN, "<%s> - Stim %s undefined in state %s",
                typeid(Internal_T).name(),
                toString(stim).c_str(), toString(m_current_state).c_str());
            return;
        }

        clearTimeout();
        State_T (Internal_T::*fn)() = itr->second;
        transitionState((m_delegate.*fn)());
    }

    void transitionState( State_T new_state ) {
        m_log->printfln(DEBUG, "<%s> - Transitioning from state %s to state %s",
            typeid(Internal_T).name(),
            toString(m_current_state).c_str(), toString(new_state).c_str());
        m_current_state = new_state;
    }

    void schedule_timeout( os::timeout_t timeout, Stim_T to_send) {
        std::string str = toString(to_send);
        m_log->printfln(DEBUG, "Setting timeout %lu for stim %s",
            (u32_t) timeout,
            str.c_str());

        m_scheduler.cancel(&m_timer);

        m_timer.to_send = to_send;
        m_timer.engaged = true;
        m_timer.sm = this;

        m_scheduler.schedule(&m_timer, timeout);
    }

    enum CommandType {
        STIM,
        EXIT
    };
    struct Command {
        CommandType type;
        union {
            Stim_T stim;
        } un;
    };


    class TimeoutSend: public os::Runnable {
    public:
        Stim_T to_send;
        StateMachine* sm;
        bool engaged;

        void run() OVERRIDE {
            this->engaged = false;
            sm->sendStimulus(to_send);
        }
        
    };

    State_T m_current_state ;

    /* the currently _maybe_ activated timer */
    TimeoutSend m_timer;

    /* The delegate is what implements
     * all the functionality */
    Internal_T& m_delegate;

    /* The edge map */
    typedef std::map<std::pair<State_T, Stim_T>, State_T (Internal_T::*)()> StateMapT;
    StateMapT m_state_map;

    os::Scheduler& m_scheduler;

    /* The queue of commands */
    containers::BlockingQueue<Command> m_command_queue;
};

#endif /* STATEMACHINE_HPP_ */
