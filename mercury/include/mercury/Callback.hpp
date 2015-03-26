#ifndef CALLBACK_HPP_
#define CALLBACK_HPP_

/*
 * Author: jrahm
 * created: 2015/03/26
 * Callback.hpp: <description>
 */

namespace mercury {

/**
 * @brief callback for the mercury process
 */
class Callback {
    /**
     * Called when mercury stops.
     * This will generally only be called in the case
     * of an error
     */
    void onStop();
};

}

#endif /* CALLBACK_HPP_ */
