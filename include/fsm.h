/*!
 * \file fsm.h
 * \date 2024-05-07
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 * \author Alessandro Bridi [ale.bridi15@gmail.com]
 * 
 * \brief Finite State Machine implementation
 */

#ifndef FSM_H
#define FSM_H

#include <stdint.h>

/*! \brief Type definition for a state function */
typedef void (*state_function)(void *data);

/*! \brief Type definition for a transition function */
typedef void (*transition_function)(void *data);

/*!
 * \brief Return codes for FSM operations
 */
enum FSMReturnCode {
    FSM_RC_OK,                 /*!< Operation completed successfully */
    FSM_RC_ERROR,              /*!< An error occurred */
    FSM_RC_INVALID_MACHINE,    /*!< The FSM structure is invalid */
    FSM_RC_INVALID_STATE,      /*!< The specified state is invalid */
    FSM_RC_INVALID_TRANSITION, /*!< The specified transition is invalid */
};

/*!
 * \brief Structure representing a state in the FSM
 */
struct State {
    uint8_t id;              /*!< The ID of the state*/
    state_function function; /*!< The function to be run at the state*/
    uint8_t next_default;    /*!< The default next state (left empty for repeat states)*/

    Transition *transitions; /*!< The transitions from this state*/
    uint8_t num_transitions; /*!< The number of transitions from this state*/
};

/*!
 * \brief Structure representing a transition in the FSM
 */
struct Transition {
    uint8_t to;                   /*!< The state ID of the state to which to move*/
    transition_function function; /*!< OPTIONAL: the transition function to run in between the state change*/
};

/*!
 * \brief Structure representing the FSM handler
 * 
 * \warning The machine_states pointer must be statically allocated
 */
struct FSMHandler {

    uint8_t current_state;   /*!< The current state in which the FSM is*/
    uint8_t requested_state; /*!< The requested state to go to at the end of the current state*/

    State *machine_states; /*!< The states of the FSM*/
    uint8_t state_count;   /*!< The number of states in the FSM*/
};

#endif /* FSM_H */