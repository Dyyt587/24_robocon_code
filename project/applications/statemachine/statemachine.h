#ifndef __CSTATEMACHINE_H
#define __CSTATEMACHINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cvector.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <ulog.h>
	/**
	* auto start: if user do not set the start state we will use the first state added in state machine to start
	*/
	/*enable auto start*/
#define ASTATE_AUTO_START 0

#define ASTSTE_USE_DEBUG_TRACK_TRIGGRT 1
#define ASTSTE_USE_DEBUG_TRACK_DEFAULT_FINAL_STATE 1
#define ASTATE_DEBUG_TRACK_FINISH 1
#define ASTATE_DEBUG_TRACK_TRANSITION 1
#define ASTATE_DEBUG_TRACK_ENTRY 1
#define ASTATE_DEBUG_TRACK_EXCUTION 1
#define ASTATE_DEBUG_TRACK_EXIT 1
#define ASTATE_DEBUG_TRACK_CONDITION 1

#define ASTATE_USE_C_SUPPORT 1

#define ASTATEMACHINE_PRINTF_WARING LOG_W
#define ASTATEMACHINE_PRINTF_ERROR LOG_E
#define ASTATEMACHINE_PRINTF_INFO LOG_I

enum {
	stateNone,
	statePause,
	stateFinish,
	stateNumber
};

struct aState;
struct aStateMachine;
typedef struct transition
{
	bool trigger;
	struct aState* to;
	bool (*condition)(struct aStateMachine* statemachine);
}transition;

typedef struct aState
{
	//����entry excution exit translation������
	int (*entry)(struct aState* s);
	int (*excution)(struct aState* s);
	int (*exit)(struct aState* s);

	const char* name;
	cvector transitions;
}aState;

typedef struct aStateMachine
{
	const char* name;
	cvector states;
	struct aState* current;
	struct aState* previous;
	bool started;
	bool finished;
	struct aState defaultFinishState;
}aStateMachine;

void transition_Init(transition* t, aState* to, bool(*condition)(aStateMachine* statemachine));
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void aState_Init(aState* s, const char* _name, int(*_excution)(aState* s), int(*_entry)(aState* s), int(*_exit)(aState* s));
int aState_excution(aState* s);
int aState_entry(aState* s);
int aState_exit(aState* s);
void aState_showTransition(aState* s);
const char* aState_getName(aState* s);
void aState_addTransition(aState* s, transition* _transition);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void aStateMachine_Init(aStateMachine* sm, const char* _name);

int defaultFinishState_excution(aState* s);
void aStateMachine_finish(aStateMachine* sm);
aState* aStateMachine_convert_state(aStateMachine* sm, const char* name);
const char* aStateMachine_convert_name(aState* state);
void aStateMachine_showStates(aStateMachine* sm);
void aStateMachine_addState(aStateMachine* sm, aState* s);
void aStateMachine_addTransition1(aStateMachine* sm, aState* from, aState* to, bool(*condition)(aStateMachine* statemachine));
bool condition(aStateMachine* statemachine);
void aStateMachine_addTransition2(aStateMachine* sm, const char* fromName, const char* toName, bool(*condition)(aStateMachine* statemachine));
void aStateMachine_addTransition3(aStateMachine* sm, aState* from, const char* toName, bool(*condition)(aStateMachine* statemachine));
void aStateMachine_addTransition4(aStateMachine* sm, const char* fromName, aState* to, bool(*condition)(aStateMachine* statemachine));
void aStateMachine_start(aStateMachine* sm, aState* s);
void aStateMachine_restart(aStateMachine* sm, aState* s);
void aStateMachine_update(aStateMachine* sm);

#if ASTATE_USE_C_SUPPORT

#endif

#endif
