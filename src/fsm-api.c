/*!
 * \file fsm.h
 * \date 2026-05-14
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 * 
 * \brief Implementation of the Finite State Machine
 */

#include "fsm-api.h"

enum FSMReturnCode fsm_api_init(struct FSMHandler *handler, struct State *state_list, uint8_t state_count, uint8_t initial_state) {
    if (handler == NULL || state_list == NULL) {
        return FSM_RC_NULL_POINTER;
    }
    if (state_count <= 1) {
        return FSM_RC_INVALID_MACHINE;
    }
    if (initial_state >= state_count) {
        return FSM_RC_INVALID_STATE;
    }

    // Fill handler
    memset(handler, 0, sizeof(handler));

    handler->current_state = initial_state;
    handler->requested_state = initial_state;
    handler->state_count = state_count;
    handler->machine_states = state_list;

    // Validate machine
    for (int i = 0; i < state_count; i++) {

        struct State state = state_list[i];

        if (state.num_transitions == 0           /**/
            || state.function == NULL            /**/
            || state.next_default >= state_count /**/
            || state.transitions == NULL         /**/
            || state.num_transitions == 0        /**/
            || state.id >= state_count           /**/
            || state.id != i /**/) {
            return FSM_RC_INVALID_MACHINE;
        }
        for (int n = 0; n < state.num_transitions; n++) {
            struct Transition transition = state.transitions[n];
            if (transition.to >= state_count) {
                return FSM_RC_INVALID_TRANSITION;
            }
            state_list[transition.to].valid_state = true;
        }
    }

    state_list[initial_state].valid_state = true;

    // Validate states

    for (int i = 0; i < state_count; i++) {

        struct State state = state_list[i];
        if (!state.valid_state) {
            return FSM_RC_INVALID_MACHINE;
        }
    }
}