#include "ConditionalEvent.h"

#include <functional>
#include <memory>
#include "ITimelineEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::timeline;

/// Creates a conditional event from the given
/// condition, if-clause event, and else-clause
/// event.
ConditionalEvent::ConditionalEvent(
	const ScenePredicate& condition,
	const ITimelineEvent_ptr& ifEvent,
	const ITimelineEvent_ptr& elseEvent,
	bool shouldReevaluate)
	: condition(condition), ifEvent(ifEvent), 
	  elseEvent(elseEvent), 
	  shouldReevaluate(shouldReevaluate), 
	  state(NotRunning)
{ }

/// Starts the timeline event.
void ConditionalEvent::start(Scene& target)
{
	this->end(target);
	this->reevaluateCondition(target);
}

/// Has this timeline event update the given scene.
bool ConditionalEvent::update(Scene& target, duration_t timeDelta)
{
	if (shouldReevaluate)
		this->reevaluateCondition(target);
	return this->selectedEvent()->update(target, timeDelta);
}

/// Applies this timeline event's finalization 
/// logic to the given scene.
void ConditionalEvent::end(Scene& target)
{
	if (this->isRunning())
	{
		this->selectedEvent()->end(target);
		this->state = NotRunning;
	}
}

/// Checks if this event is currently running.
bool ConditionalEvent::isRunning() const
{
	return this->state != NotRunning;
}

/// Re-evaluates the condition, ending the 
/// current event and switching to the other
/// event if necessary.
void ConditionalEvent::reevaluateCondition(Scene& target)
{
	bool outcome = this->condition(target);
	if (outcome && this->state != IfClause)
	{
		this->end(target);
		this->state = IfClause;
		this->ifEvent->start(target);
	}
	else if (!outcome && this->state != ElseClause)
	{
		this->end(target);
		this->state = ElseClause;
		this->elseEvent->start(target);
	}
}

/// Gets the currently selected event.
ITimelineEvent_ptr ConditionalEvent::selectedEvent() const
{
	switch (this->state)
	{
	case IfClause:
		return this->ifEvent;
	case ElseClause:
		return this->elseEvent;
	default:
		return nullptr;
	}
}