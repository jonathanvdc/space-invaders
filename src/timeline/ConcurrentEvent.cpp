#include "ConcurrentEvent.h"

#include <cstddef>
#include <set>
#include <vector>
#include "Common.h"
#include "ITimelineEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::timeline;

/// Creates a new concurrent event from the given
/// vector of sub-events.
ConcurrentEvent::ConcurrentEvent(const std::vector<ITimelineEvent_ptr>& subEvents)
	: subEvents(subEvents), runningEventIndices()
{ }

/// Creates a new concurrent event from the given
/// vector of sub-events.
ConcurrentEvent::ConcurrentEvent(std::vector<ITimelineEvent_ptr>&& subEvents)
	: subEvents(subEvents), runningEventIndices()
{ }

/// Starts the timeline event.
void ConcurrentEvent::start(Scene& target)
{
	// End any events that are still running.
	this->end(target);

	// Start all events, and add their indices
	// to the running event set.
	for (std::size_t i = 0; i < this->subEvents.size(); i++)
	{
		this->subEvents.at(i)->start(target);
		this->runningEventIndices.insert(i);
	}
}

/// Has this timeline event update the given scene.
/// The event's associated scene, as well as
/// the amount of time that has passed since the
/// last update, are given. A boolean flag is
/// returned that tells if this timeline event
/// is still running.
bool ConcurrentEvent::update(Scene& target, duration_t timeDelta)
{
	// Create a copy of the running event indices,
	// and iterate over that. This will allow us
	// to iterate and erase in one step.
	auto indices = this->runningEventIndices;
	for (std::size_t i : indices)
	{
		// Check if the ith event is still running.
		if (!this->subEvents.at(i)->update(target, timeDelta))
		{
			// If not, then remove its index from the
			// running events set...
			this->runningEventIndices.erase(i);
			// ...and terminate it right away.
			this->subEvents.at(i)->end(target);
		}
	}
	
	// Check if any events are still running.
	return this->isRunning();
}

/// Applies this timeline event's finalization 
/// logic to the given scene: objects associated
/// with this event are removed from the scene.
/// This method can either be called to perform
/// cleanup after an event has ended, or 
/// to forcibly end the event. It should
/// be called exactly once for every event that
/// has been started.
void ConcurrentEvent::end(Scene& target)
{
	// Terminate any events that are still running.
	for (std::size_t i : this->runningEventIndices)
	{
		this->subEvents.at(i)->end(target);
	}

	this->runningEventIndices.clear();
}

/// Gets a boolean value that tells if this
/// concurrent event is running right now.
bool ConcurrentEvent::isRunning() const
{
	return !this->runningEventIndices.empty();
}