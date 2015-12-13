#include "DeadlineEvent.h"

#include <chrono>
#include "Common.h"
#include "ITimelineEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::timeline;
using namespace std::chrono_literals;

/// Creates a new deadline event from the given 
/// inner event, and a deadline.
DeadlineEvent::DeadlineEvent(
	const si::timeline::ITimelineEvent_ptr& innerEvent,
	duration_t deadline)
	: innerEvent(innerEvent), deadline(deadline), elapsed(0.0s)
{ }

/// Starts the timeline event.
void DeadlineEvent::start(Scene& target)
{
	this->innerEvent->start(target);
	this->elapsed = 0.0s;
}

/// Has this timeline event update the given scene.
bool DeadlineEvent::update(Scene& target, duration_t timeDelta)
{
	this->elapsed += timeDelta;

	if (this->elapsed > this->deadline)
		return false;

	return this->innerEvent->update(target, timeDelta);
}

/// Applies this timeline event's finalization 
/// logic to the given scene.
void DeadlineEvent::end(Scene& target)
{
	this->innerEvent->end(target);
}