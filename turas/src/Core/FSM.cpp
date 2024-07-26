#include "Core/FSM.h"
#include "Core/Log.h"
#include "Debug/Profile.h"
turas::FSM::State::State(int state, Function<int()> action, Function<void()> entry, Function<void()> exit)
	: m_State(state), m_Action(action)
{
	ZoneScoped;
	HasEntry = false;
	HasExit	 = false;
	if (entry != NULL) {
		m_EntryProcedure = entry;
		HasEntry		 = true;
	}
	if (exit != NULL) {
		m_ExitProcedure = exit;
		HasExit			= true;
	}
}
turas::FSM::FSM() : p_CurrentState(UINT16_MAX), p_RunEntry(false) { ZoneScoped; }
void		turas::FSM::SetStartingState(int state)
{
	ZoneScoped;
	p_CurrentState = state;
}
void turas::FSM::Process()
{
	ZoneScoped;
	int index = -1;
	for (int i = 0; i < p_States.size(); i++) {
		if (p_States[i].m_State == p_CurrentState) {
			index = i;
			break;
		}
	}
	if (index < 0) {
		turas::log::info("No State to process");
		p_PreviousState = p_CurrentState;
		return;
	}
	if (!p_RunEntry) {
		if (p_States[index].HasEntry) {
			p_States[index].m_EntryProcedure();
			p_RunEntry = true;
		}
	}
	int		trigger		   = p_States[index].m_Action();
	uint8_t numTransitions = 0;
	while (trigger != NO_TRIGGER) {
		p_PreviousState = p_CurrentState;
		if (!p_RunEntry) {
			if (p_States[index].HasEntry) {
				p_States[index].m_EntryProcedure();
				p_RunEntry = true;
			}
		}
		for (int i = 0; i < p_Transitions.size(); i++) {
			if (p_Transitions[i].Trigger == trigger) {
				if (p_Transitions[i].SrcState == p_CurrentState) {
					TransitionState(p_Transitions[i].DstState);
					numTransitions++;
					break;
				}
			}
		}
		if (p_CurrentState != p_PreviousState) {
			trigger = p_States[index].m_Action();
		}
	}
	p_PreviousState = p_CurrentState;
}
void turas::FSM::Trigger(int trigger)
{
	ZoneScoped;
	for (int i = 0; i < p_Transitions.size(); i++) {
		if (p_Transitions[i].Trigger == trigger) {
			if (p_Transitions[i].SrcState == p_CurrentState) {
				TransitionState(p_Transitions[i].DstState);
				break;
			}
		}
	}
}
void turas::FSM::AddState(int state, Function<int()> action)
{
	ZoneScoped;
	for (int i = 0; i < p_States.size(); i++) {
		if (p_States[i].m_State == state) {
			turas::log::error("Already have a state in machine for : {}", state);
			return;
		}
	}
	State stateObj = State(state, action);
	p_States.emplace_back(stateObj);
}
void turas::FSM::AddStateEntry(int state, Function<void()> entry)
{
	ZoneScoped;
	for (int i = 0; i < p_States.size(); i++) {
		if (p_States[i].m_State == state) {
			p_States[i].m_EntryProcedure = entry;
			p_States[i].HasEntry		 = true;
		}
	}
}
void turas::FSM::AddStateExit(int state, Function<void()> exit)
{
	ZoneScoped;
	for (int i = 0; i < p_States.size(); i++) {
		if (p_States[i].m_State == state) {
			p_States[i].m_ExitProcedure = exit;
			p_States[i].HasExit			= true;
		}
	}
}
void turas::FSM::AddTrigger(int trigger, int srcState, int dstState)
{
	ZoneScoped;
	StateTransition transition{trigger, srcState, dstState};
	p_Transitions.emplace_back(transition);
}
void turas::FSM::TransitionState(int newState)
{
	ZoneScoped;
	if (p_States[p_CurrentState].HasExit) {
		p_States[p_CurrentState].m_ExitProcedure();
	}
	p_CurrentState = newState;
	p_RunEntry	   = false;
}
