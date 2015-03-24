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

/**
 * @param Stim_T must define std::string toString() and must be orderable
 * @param State_T must define std::string toString() and must be orderable
 */
template <class Stim_T, class Internal_T, class State_T>
class StateMachine {
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
        os::ScopedLock __sl(m_mutex);
        _sendStimulus(stim);
    }

    /**
     * @brief Schedule a stim to be send after a certain timeout. This will be cleared if another stim comes first
     * 
     * @param timeout the amout of time to wait before sending the stim
     * @param to_send The stim to send
     */
    void setTimeoutStim( os::timeout_t timeout, Stim_T to_send ) {
        // os::ScopedLock __sl(m_mutex);

        // schedule_timeout(timeout, to_send);   
        std::pair<Stim_T, os::timeout_t> p;
        p.first = to_send; p.second = timeout;
        m_timeouts.push(p);
    }

    void setEdge( State_T current_state, Stim_T stim, State_T (Internal_T::*fn)() ) {
        m_state_map[ std::make_pair(current_state, stim) ] = fn;
    }

    State_T getCurrentState() {
        return m_current_state;
    }

    /**
     * @brief Remove the currently active timeout
     */
    void clearTimeout() {
        os::ScopedLock __sl(m_mutex);
        _clearTimeout();
    }

protected:
    logger::LogContext* m_log;

private:
    void _clearTimeout() {
        m_log->printfln(DEBUG, "Clearing timeout stim %s", toString(m_timer.to_send).c_str());
        m_scheduler.cancel(&m_timer);
    }

    void _sendStimulus(Stim_T stim) {
        _clearTimeout();

        m_log->printfln(DEBUG, "Processing stim %s", toString(stim).c_str());
        State_T (Internal_T::*fn)() = m_state_map[ std::make_pair(m_current_state, stim) ];

        if ( ! fn ) {
            /* state undefined, return */
            m_log->printfln(WARN, "Stim %s undefined in state %s",
                toString(stim).c_str(), toString(m_current_state).c_str());
            return;
        }

        /* transition to a new state */
        transitionState( (m_delegate.*fn)() );
    }
    void transitionState( State_T new_state ) {
        process_timeouts();
        m_log->printfln(DEBUG, "Transitioning from state %s to state %s",
            toString(m_current_state).c_str(), toString(new_state).c_str());
        m_current_state = new_state;
    }

    void process_timeouts() {
        /* Add all timeouts to the scheduler */
        while( ! m_timeouts.empty() ) {
            std::pair<Stim_T, os::timeout_t> l_pair;
            l_pair = m_timeouts.front();
            schedule_timeout(l_pair.second, l_pair.first);
            m_timeouts.pop();
        }
    }

    void schedule_timeout( os::timeout_t timeout, Stim_T to_send) {
        m_log->printfln(DEBUG, "Setting timeout for stim %s", 
            toString(to_send).c_str());

        m_timer.to_send = to_send;
        m_timer.engaged = true;

        m_scheduler.cancel(&m_timer);

        m_timer.to_send = to_send;
        m_timer.engaged = true;
        m_timer.sm = this;

        m_scheduler.schedule(&m_timer, timeout);
    }

    class TimeoutSend: public os::Runnable {
    public:
        Stim_T to_send;
        StateMachine* sm;
        bool engaged;

        void run() OVERRIDE {
            /* Lock the mutex and send a stimulus without
             * the mutex */
            os::ScopedLock __sl(sm->m_mutex);
            if(engaged) {
                sm->_sendStimulus(to_send);
            }
        }
        
    };

    State_T m_current_state ;
    TimeoutSend m_timer;

    /* The delegate is what implements
     * all the functionality */
    Internal_T& m_delegate;

    std::map< std::pair<State_T, Stim_T> ,
              State_T (Internal_T::*)() > m_state_map;

    containers::BlockingQueue< std::pair<Stim_T, os::timeout_t> > m_timeouts;
    os::Mutex m_mutex;
    os::Scheduler& m_scheduler;
};

#endif /* STATEMACHINE_HPP_ */
