#include "statemachine.h"

void transition_Init(transition* t, aState* to, bool(*condition)(aStateMachine* statemachine))
{
	t->trigger = true;
	t->to = to;
	t->condition = condition;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void aState_Init(aState* s, const char* _name, int(*_excution)(aState* s), int(*_entry)(aState* s), int(*_exit)(aState* s))
{
	s->name = _name;
	s->excution = _excution;
	s->entry = _entry;
	s->exit = _exit;
	s->transitions = cvector_create(sizeof(transition*));
}

int aState_excution(aState* s)
{
	ASTATEMACHINE_PRINTF_INFO("%s excution\n", s->name);
	return stateNone;
}

int aState_entry(aState* s)
{
	ASTATEMACHINE_PRINTF_INFO("%s entry\n", s->name);
	return stateNone;
}

int aState_exit(aState* s)
{
	ASTATEMACHINE_PRINTF_INFO("%s exit\n", s->name);
	return stateNone;
}

void aState_showTransition(aState* s)
{
	for (int i = 0; i < cvector_length(s->transitions); i++)	////////////////////////////////////////////////////////////?size   length
	{
		ASTATEMACHINE_PRINTF_INFO("transition[%d] %s -> %s\r\n", i, s->name, (*(transition**)cvector_at(s->transitions, i))->to->name);
	}
}

const char* aState_getName(aState* s)
{
	return s->name;
}

void aState_addTransition(aState* s, transition* _transition)
{
	cvector_pushback(s->transitions, &_transition);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void aStateMachine_Init(aStateMachine* sm, const char* _name)
{
	sm->name = _name;
	sm->current = NULL;
	sm->previous = NULL;
	sm->started = false;
	sm->finished = false;
	aState_Init(&(sm->defaultFinishState), "finish", NULL, NULL, NULL);
	sm->states = cvector_create(sizeof(aState*));
}

int defaultFinishState_excution(aState* s)
{
#if ASTSTE_USE_DEBUG_TRACK_DEFAULT_FINAL_STATE
	ASTATEMACHINE_PRINTF_INFO("finish excution\r\n");
#endif
	return stateFinish;
}

void aStateMachine_finish(aStateMachine* sm)
{
	sm->finished = true;
}

aState* aStateMachine_convert_state(aStateMachine* sm, const char* name)
{
	//查找name对应的state
	for (int i = 0; i < cvector_length(sm->states); i++)
	{

		if (aState_getName(*(aState**)cvector_at(sm->states, i)) == name)
		{
			return *(aState**)cvector_at(sm->states, i);
		}
	}
	return NULL;
}

const char* aStateMachine_convert_name(aState* state)
{
	return aState_getName(state);
}

void aStateMachine_showStates(aStateMachine* sm)
{
	ASTATEMACHINE_PRINTF_INFO("stateMachine %s has %llu states\r\n", sm->name, cvector_length(sm->states));
	for (int i = 0; i < cvector_length(sm->states); i++)
	{
		ASTATEMACHINE_PRINTF_INFO("state[%d] %s\r\n", i, aState_getName(*(aState**)cvector_at(sm->states, i)));
	}
}

void aStateMachine_addState(aStateMachine* sm, aState* s)
{
	cvector_pushback(sm->states, &s);
}

void aStateMachine_addTransition1(aStateMachine* sm, aState* from, aState* to, bool(*condition)(aStateMachine* statemachine))
{
	if (to == NULL)
	{
		//TODO:创建一个finish状态
		to = &(sm->defaultFinishState);
		ASTATEMACHINE_PRINTF_WARING("addTransition warning: to state not found,set defaultFinshState to replace\r\n");
	}
	//查找from状态
	for (int i = 0; i < cvector_length(sm->states); i++)
	{
		if ( ( *(aState**)cvector_at(sm->states, i) ) == from) {		////////两个指针指向的是同一块内存，但它们两个自己是不同的，不能直接比较
			//找到from状态
			transition* p = (transition*)malloc(sizeof(transition));
			transition_Init(p, to, condition);
			aState_addTransition(from, p);
			return;
		}
	}
	//如果没有找到from状态,则报错
	ASTATEMACHINE_PRINTF_ERROR("addTransition error: from state not found");
}

bool condition(aStateMachine* statemachine)
{
	return true;
}

void aStateMachine_addTransition2(aStateMachine* sm, const char* fromName, const char* toName, bool(*condition)(aStateMachine* statemachine))
{
	aState* from;
	aState* to;
	//转换为指针
	from = aStateMachine_convert_state(sm, fromName);
	to = aStateMachine_convert_state(sm, toName);
	aStateMachine_addTransition1(sm, from, to, condition);
}

void aStateMachine_addTransition3(aStateMachine* sm, aState* from, const char* toName, bool(*condition)(aStateMachine* statemachine))
{
	aState* to;
	to = aStateMachine_convert_state(sm, toName);
	aStateMachine_addTransition1(sm, from, to, condition);
}

void aStateMachine_addTransition4(aStateMachine* sm, const char* fromName, aState* to, bool(*condition)(aStateMachine* statemachine))
{
	aState* from;
	from = aStateMachine_convert_state(sm, fromName);
	aStateMachine_addTransition1(sm, from, to, condition);
}

void aStateMachine_start(aStateMachine* sm, aState* s)
{
	if (sm->started)
	{
		ASTATEMACHINE_PRINTF_INFO("stateMachine %s has already started\r\n", sm->name);
		return;
	}
	sm->started = true;
	sm->current = s;
	sm->previous = NULL;
	if (s->entry != NULL)
	{
		s->entry(s);
	}
}

void aStateMachine_restart(aStateMachine* sm, aState* s)
{
	sm->started = false;
	sm->finished = false;
	sm->current = s;
	sm->previous = NULL;
	if (s->entry != NULL)
	{
		s->entry(s);
	}
}

void aStateMachine_update(aStateMachine* sm)
{
#if !ASTATE_AUTO_START
	if (!sm->started)
	{
		ASTATEMACHINE_PRINTF_INFO("stateMachine %s has not started\r\n", sm->name);
		return;
	}
#endif
	if (sm->finished)
	{
#if ASTATE_DEBUG_TRACK_FINISH
		ASTATEMACHINE_PRINTF_INFO("stateMachine %s has finished\r\n", sm->name);
#endif
		return;
	}
	if (sm->current != NULL)
	{
		if (sm->current->excution != NULL)
		{
			if (sm->current->excution(sm->current) == stateFinish)
			{
				aStateMachine_finish(sm);
			}
		}
		//遍历当前状态的所有转换
		for (int i = 0; i < cvector_length(sm->current->transitions); i++)
		{
#if ASTATE_DEBUG_TRACK_CONDITION
			ASTATEMACHINE_PRINTF_INFO("update debug: stateMachine %s condition[%d]\r\n", aState_getName(sm->current), i);
#endif
			transition* t = *(transition**)cvector_at(sm->current->transitions, i);
			if (t->trigger)
			{
				if (t->condition(sm))
				{
#if ASTATE_DEBUG_TRACK_TRANSITION
					ASTATEMACHINE_PRINTF_INFO("update debug: stateMachine transition %s -> %s\r\n",
						aState_getName(sm->current),
						aState_getName(t->to));
#endif
					if (sm->current->exit != NULL)
					{
#if ASTATE_DEBUG_TRACK_EXIT
						ASTATEMACHINE_PRINTF_INFO("update debug: stateMachine %s exit\r\n", aState_getName(sm->current));
#endif
						sm->current->exit(sm->current);
					}
					sm->previous = sm->current;
					sm->current = t->to;
					if (sm->current->entry != NULL)
					{
#if ASTATE_DEBUG_TRACK_ENTRY
						ASTATEMACHINE_PRINTF_INFO("update debug: stateMachine %s entry\r\n", aState_getName(sm->current));
#endif
						sm->current->entry(sm->current);
					}
					break;
				}

			}
#if ASTSTE_USE_DEBUG_TRACK_TRIGGRT
			else {
				ASTATEMACHINE_PRINTF_INFO("update debug: transition %s to %s not triggered", aState_getName(sm->current), aState_getName(sm->current));
			}
#endif
		}

	}
	else if (sm->started == false)
	{
		aStateMachine_start(sm, *(aState**)cvector_at(sm->states, 0));
	}
	else
	{
		/* will never reaach */
		ASTATEMACHINE_PRINTF_INFO("update fault: stateMachine nuknown error\r\n");
	}
}
