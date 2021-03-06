#pragma once


namespace et {
namespace core {


//---------------------------------
// I_DefaultTickTriggerer
//
// Interface for things that can call a tick in a non realtime context
//
class I_DefaultTickTriggerer
{
protected:
	virtual ~I_DefaultTickTriggerer();

	void TriggerTick();

	void RegisterAsTriggerer();
	void UnRegisterAsTriggerer();
};


} // namespace core
} // namespace et
