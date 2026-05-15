/*!
 * \file fsm-api.c
 * \date 2026-05-14
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 * 
 * \brief Implementation of the Finite State Machine
 */

#include "eagletrt-fsm-api.h"

enum FSMReturnCode fsm_api_init(struct FSMHandler *handler, const struct State *state_list, uint8_t state_count, uint8_t initial_state) {
    if (handler == NULL || state_list == NULL) {
        return FSM_RC_NULL_POINTER;
    }
    if (state_count <= 1) {
        return FSM_RC_INVALID_MACHINE;
    }
    if (initial_state >= state_count) {
        return FSM_RC_INVALID_STATE;
    }

    // Validate machine
    for (uint8_t i = 0; i < state_count; i++) {

        const struct State state = state_list[i];

        if ((state.num_transitions != 0 && state.transitions == NULL) /**/
            || state.function == NULL                                 /**/
            || state.next_default >= state_count                      /**/
            || (state.num_transitions == 0 && state.repeat == false)  /**/
            || state.id >= state_count                                /**/
            || state.id != i /**/) {
            return FSM_RC_INVALID_MACHINE;
        }

        bool ok_next_default = false;

        for (uint8_t n = 0; n < state.num_transitions; n++) {
            const struct Transition transition = state.transitions[n];
            if (transition.to >= state_count) {
                return FSM_RC_INVALID_TRANSITION;
            }
            if (transition.to == state.next_default) {
                ok_next_default = true;
            }
        }
        if (!ok_next_default && !state.repeat) {
            return FSM_RC_INVALID_MACHINE;
        }
    }

    // Fill handler
    memset(handler, 0, sizeof(*handler));

    handler->current_state = initial_state;
    handler->requested_state = initial_state;
    handler->state_count = state_count;
    handler->machine_states = state_list;

    return FSM_RC_OK;
}

enum FSMReturnCode fsm_api_run_state(struct FSMHandler *handler, void *data) {
    if (handler == NULL) {
        return FSM_RC_NULL_POINTER;
    }

    const struct State current_state = handler->machine_states[handler->current_state];

    // Set next state
    if (!current_state.repeat && handler->requested_state == handler->current_state) { // What should the behavior be here, should it discard any trigger state made in between the end of the routine and this line or not ??
        handler->requested_state = current_state.next_default;
    }

    // Run state
    current_state.function(data);

    if (handler->requested_state == handler->current_state) {
        return FSM_RC_OK;
    }

    for (uint8_t i = 0; i < current_state.num_transitions; i++) {
        if (current_state.transitions[i].to == handler->requested_state) {
            if (current_state.transitions[i].function != NULL) {
                current_state.transitions[i].function(data);
            }
            handler->current_state = handler->requested_state;
            return FSM_RC_OK;
        }
    }

    return FSM_RC_INVALID_TRANSITION;
}

enum FSMReturnCode fsm_api_trigger_event(struct FSMHandler *handler, uint8_t state_id) {
    if (handler == NULL) {
        return FSM_RC_NULL_POINTER;
    }
    if (state_id >= handler->state_count) {
        return FSM_RC_INVALID_STATE;
    }

    if (state_id == handler->requested_state) {
        return FSM_RC_OK;
    }

    const struct State current_state = handler->machine_states[handler->current_state];

    for (uint8_t i = 0; i < current_state.num_transitions; i++) {
        if (current_state.transitions[i].to == state_id) {
            handler->requested_state = state_id;
            return FSM_RC_OK;
        }
    }

    return FSM_RC_INVALID_TRANSITION;
}

uint8_t fsm_api_get_state(const struct FSMHandler *handler) {
    if (handler == NULL) {
        return 0;
    }
    return handler->current_state;
}