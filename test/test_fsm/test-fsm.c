/*!
 * \file test-timebase.c
 * \date 2026-05-14
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Unit tests for the FSM
 */

#include "unity.h"
#include "fsm-api.h"
#include "fff.h"
DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC(routine_0, void *);
FAKE_VOID_FUNC(routine_1, void *);
FAKE_VOID_FUNC(routine_2, void *);
FAKE_VOID_FUNC(routine_3, void *);

FAKE_VOID_FUNC(transition_0_1, void *);
FAKE_VOID_FUNC(transition_2_3, void *);

enum StatesID {
    STATE_0,
    STATE_1,
    STATE_2,
    STATE_3,
    STATE_COUNT
};

struct Transition state_0_transitions[] = {
    {
        .function = transition_0_1,
        .to = STATE_1,
    }
};
struct Transition state_1_transitions[] = {
    {
        .function = NULL,
        .to = STATE_2,
    },
    {
        .function = NULL,
        .to = STATE_3,
    }
};
struct Transition state_2_transitions[] = {
    {
        .function = transition_2_3,
        .to = STATE_3,
    }
};
struct State default_states[] = {
    {
        .id = STATE_0,
        .function = routine_0,
        .repeat = false,
        .next_default = STATE_1,
        .transitions = state_0_transitions,
        .num_transitions = FSM_TRANSITION_LEN(state_0_transitions),
    },
    {
        .id = STATE_1,
        .function = routine_1,
        .repeat = false,
        .next_default = STATE_2,
        .transitions = state_1_transitions,
        .num_transitions = FSM_TRANSITION_LEN(state_1_transitions),
    },
    {
        .id = STATE_2,
        .function = routine_2,
        .repeat = true,
        .next_default = 0, // Ignored for repeat states
        .transitions = state_2_transitions,
        .num_transitions = FSM_TRANSITION_LEN(state_2_transitions),
    },
    {
        .id = STATE_3,
        .function = routine_3,
        .repeat = true,
        .next_default = STATE_1,
        .transitions = NULL, // No transitions from this state
        .num_transitions = 0,
    },
};

struct FSMHandler handler;

void setUp(void) {
    RESET_FAKE(routine_0);
    RESET_FAKE(routine_1);
    RESET_FAKE(routine_2);
    RESET_FAKE(routine_3);

    RESET_FAKE(transition_0_1);
    RESET_FAKE(transition_2_3);

    fsm_api_init(&handler, default_states, STATE_COUNT, STATE_0);
}

void tearDown(void) {
}

void invalidate_states(void) {
    default_states[0].valid_state = false;
    default_states[1].valid_state = false;
    default_states[2].valid_state = false;
    default_states[3].valid_state = false;
}

// Initialization tests

void test_fsm_init_null_handler(void) {

    enum FSMReturnCode rc = fsm_api_init(NULL, default_states, STATE_COUNT, STATE_0);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_NULL_POINTER, rc, "Expected FSM_RC_NULL_POINTER when handler is NULL");
}

void test_fsm_init_null_state_list(void) {

    enum FSMReturnCode rc = fsm_api_init(&handler, NULL, STATE_COUNT, STATE_0);
    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_NULL_POINTER, rc, "Expected FSM_RC_NULL_POINTER when state list is NULL");
}

void test_fsm_init_invalid_state_count(void) {

    invalidate_states();

    enum FSMReturnCode rc = fsm_api_init(&handler, default_states, 1, STATE_0);
    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_INVALID_MACHINE, rc, "Expected FSM_RC_INVALID_MACHINE when state count is less than or equal to 1");
}

void test_fsm_init_invalid_initial_state(void) {

    invalidate_states();

    enum FSMReturnCode rc = fsm_api_init(&handler, default_states, STATE_COUNT, STATE_COUNT); // Invalid initial state ID

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_INVALID_STATE, rc, "Expected FSM_RC_INVALID_STATE when initial state ID is invalid");
}

void test_fsm_init_null_state_function(void) {

    invalidate_states();

    struct State invalid_state[STATE_COUNT] = {};
    memcpy(invalid_state, default_states, sizeof(default_states));
    invalid_state[1].function = NULL;

    enum FSMReturnCode rc = fsm_api_init(&handler, invalid_state, STATE_COUNT, STATE_0);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_INVALID_MACHINE, rc, "Expected FSM_RC_INVALID_MACHINE when state function is NULL");
}

void test_fsm_init_invalid_next_default_state(void) {

    invalidate_states();

    struct State invalid_state[STATE_COUNT] = {};
    memcpy(invalid_state, default_states, sizeof(default_states));
    invalid_state[1].next_default = STATE_COUNT; // Invalid state ID

    enum FSMReturnCode rc = fsm_api_init(&handler, invalid_state, STATE_COUNT, STATE_0);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_INVALID_MACHINE, rc, "Expected FSM_RC_INVALID_MACHINE when next default state is invalid");
}

void test_fsm_init_invalid_next_default_state_not_in_transitions(void) {

    invalidate_states();

    struct State invalid_state[STATE_COUNT] = {};
    memcpy(invalid_state, default_states, sizeof(default_states));
    invalid_state[0].next_default = STATE_3; // State 3 is not in the transitions of state 0 and state 0 is not a repeat state

    enum FSMReturnCode rc = fsm_api_init(&handler, invalid_state, STATE_COUNT, STATE_0);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_INVALID_MACHINE, rc, "Expected FSM_RC_INVALID_MACHINE when next default state is not in transitions for non-repeat states");
}

void test_fsm_init_null_state_transitions_pointer(void) {

    invalidate_states();

    struct State invalid_state[STATE_COUNT] = {};
    memcpy(invalid_state, default_states, sizeof(default_states));
    invalid_state[1].transitions = NULL;

    enum FSMReturnCode rc = fsm_api_init(&handler, invalid_state, STATE_COUNT, STATE_0);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_INVALID_MACHINE, rc, "Expected FSM_RC_INVALID_MACHINE when state transitions pointer is NULL");
}

void test_fsm_init_invalid_state_ID(void) {

    invalidate_states();

    struct State invalid_state[STATE_COUNT] = {};
    memcpy(invalid_state, default_states, sizeof(default_states));
    invalid_state[1].id = STATE_COUNT; // Invalid state ID

    enum FSMReturnCode rc = fsm_api_init(&handler, invalid_state, STATE_COUNT, STATE_0);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_INVALID_MACHINE, rc, "Expected FSM_RC_INVALID_MACHINE when state ID is invalid");
}

void test_fsm_init_non_consecutive_ID(void) {

    invalidate_states();

    struct State invalid_state[STATE_COUNT] = {};
    memcpy(invalid_state, default_states, sizeof(default_states));
    invalid_state[1].id = 5; // Non-consecutive state ID

    enum FSMReturnCode rc = fsm_api_init(&handler, invalid_state, STATE_COUNT, STATE_0);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_INVALID_MACHINE, rc, "Expected FSM_RC_INVALID_MACHINE when state IDs are not consecutive starting from 0");
}

void test_fsm_init_invalid_transition_ID(void) {

    invalidate_states();

    struct State invalid_state[STATE_COUNT] = {};
    memcpy(invalid_state, default_states, sizeof(default_states));
    invalid_state[1].transitions[0].to = STATE_COUNT; // Invalid transition state ID

    enum FSMReturnCode rc = fsm_api_init(&handler, invalid_state, STATE_COUNT, STATE_0);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_INVALID_TRANSITION, rc, "Expected FSM_RC_INVALID_TRANSITION when transition state ID is invalid");

    //Reset transition to valid value for subsequent tests
    invalid_state[1].transitions[0].to = STATE_2;
}

void test_fsm_init_isolated_state(void) {

    invalidate_states();

    struct State invalid_state[STATE_COUNT] = {};
    memcpy(invalid_state, default_states, sizeof(default_states));
    // State 2 is isolated because no transition leads to it and it's not the initial state
    invalid_state[1].transitions[0].to = STATE_3; // Remove transition to state 2

    enum FSMReturnCode rc = fsm_api_init(&handler, invalid_state, STATE_COUNT, STATE_0);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_INVALID_MACHINE, rc, "Expected FSM_RC_INVALID_MACHINE when there is an isolated state");

    //Reset transition to valid value for subsequent tests
    invalid_state[1].transitions[0].to = STATE_2;
}

void test_fsm_init_invalid_repeat_state(void) {

    invalidate_states();

    struct State invalid_state[STATE_COUNT] = {};
    memcpy(invalid_state, default_states, sizeof(default_states));
    invalid_state[3].repeat = false; // State 3 has no transitions and is not a repeat state

    enum FSMReturnCode rc = fsm_api_init(&handler, invalid_state, STATE_COUNT, STATE_0);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_INVALID_MACHINE, rc, "Expected FSM_RC_INVALID_MACHINE when there is a non-repeat state with no transitions");
}

void test_fsm_init_ok_parameters(void) {

    invalidate_states();

    enum FSMReturnCode rc = fsm_api_init(&handler, default_states, STATE_COUNT, STATE_0);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_OK, rc, "Expected FSM_RC_OK when parameters are valid");
}

// Run state tests (treating trigger event as trusted dependency)

void test_fsm_run_state_null_handler(void) {
    enum FSMReturnCode rc = fsm_api_run_state(NULL, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_NULL_POINTER, rc, "Expected FSM_RC_NULL_POINTER when handler is NULL");
}
void test_fsm_run_state_non_repeat(void) {
    uint8_t data = 0;
    enum FSMReturnCode rc = fsm_api_run_state(&handler, &data);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_OK, rc, "Expected FSM_RC_OK when parameters are valid");
    TEST_ASSERT_EQUAL_MESSAGE(1, routine_0_fake.call_count, "Expected state function to be called once");
    TEST_ASSERT_EQUAL_MESSAGE(&data, routine_0_fake.arg0_val, "Expected state function to be called with correct data");
    TEST_ASSERT_EQUAL_MESSAGE(1, transition_0_1_fake.call_count, "Expected transition function to be called once");
    TEST_ASSERT_EQUAL_MESSAGE(&data, transition_0_1_fake.arg0_val, "Expected transition function to be called with correct data");
    TEST_ASSERT_EQUAL_MESSAGE(STATE_1, handler.requested_state, "Expected requested state to be updated to the state specified in the transition");
}

void test_fsm_run_state_repeat(void) {
    handler.current_state = STATE_2; // State 2 is a repeat state
    handler.requested_state = STATE_2;

    uint8_t data = 0;
    enum FSMReturnCode rc = fsm_api_run_state(&handler, &data);

    TEST_ASSERT_EQUAL_MESSAGE(FSM_RC_OK, rc, "Expected FSM_RC_OK when parameters are valid");
    TEST_ASSERT_EQUAL_MESSAGE(1, routine_2_fake.call_count, "Expected state function to be called once");
    TEST_ASSERT_EQUAL_MESSAGE(&data, routine_2_fake.arg0_val, "Expected state function to be called with correct data");
    TEST_ASSERT_EQUAL_MESSAGE(STATE_2, handler.requested_state, "Expected requested state to be updated to the state specified in the transition");
};

// Trigger event tests

void test_fsm_trigger_event_null_handler(void) {
}
void test_fsm_trigger_event_invalid_state(void) {
}
void test_fsm_trigger_event_invalid_transition(void) {
}
void test_fsm_trigger_event_ok(void) {
}

int main() {
    UNITY_BEGIN();
    // init tests
    RUN_TEST(test_fsm_init_null_handler);
    RUN_TEST(test_fsm_init_null_state_list);
    RUN_TEST(test_fsm_init_invalid_state_count);
    RUN_TEST(test_fsm_init_invalid_initial_state);
    RUN_TEST(test_fsm_init_null_state_function);
    RUN_TEST(test_fsm_init_invalid_next_default_state);
    RUN_TEST(test_fsm_init_invalid_next_default_state_not_in_transitions);
    RUN_TEST(test_fsm_init_null_state_transitions_pointer);
    RUN_TEST(test_fsm_init_invalid_state_ID);
    RUN_TEST(test_fsm_init_non_consecutive_ID);
    RUN_TEST(test_fsm_init_invalid_transition_ID);
    RUN_TEST(test_fsm_init_isolated_state);
    RUN_TEST(test_fsm_init_invalid_repeat_state);
    RUN_TEST(test_fsm_init_ok_parameters);

    // run state tests
    RUN_TEST(test_fsm_run_state_null_handler);
    RUN_TEST(test_fsm_run_state_non_repeat);
    RUN_TEST(test_fsm_run_state_repeat);
    UNITY_END();
}