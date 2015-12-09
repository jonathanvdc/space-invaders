#include "Timeline.h"

#include <cstddef>
#include <vector>
#include "Common.h"
#include "Container.h"
#include "ITimelineEvent.h"

using namespace si;
using namespace si::timeline;

/// Creates an empty timeline.
Timeline::Timeline()
	: currentEventIndex(0), allEvents()
{ }

/// Creates a new timeline from the given events, which
/// are executed in sequence.
Timeline::Timeline(const std::vector<ITimelineEvent_ptr>& allEvents)
	: currentEventIndex(allEvents.size()), allEvents(allEvents)
{ }

/// Creates a new timeline from the given events, which
/// are executed in sequence. Move semantics are
/// used to make this operation more efficient.
Timeline::Timeline(std::vector<ITimelineEvent_ptr>&& allEvents)
	: currentEventIndex(allEvents.size()), allEvents(allEvents)
{ }

/// Starts the timeline.
void Timeline::start(Scene& target)
{
	// End the timeline if it was started
	// already.
	this->end(target);

	// Initialize the current event index to the 
	// first event.
	this->currentEventIndex = 0;

	// Only start the first event if there *is*
	// a first event.
	if (this->isRunning())
	{
		this->getCurrentEvent().start(target);
	}
}

/// Has this timeline update the given scene.
/// The event's associated scene, as well as
/// the amount of time that has passed since the
/// last update, are given. A boolean flag is
/// returned that tells if this timeline event
/// has ended.
bool Timeline::update(Scene& target, duration_t timeDelta)
{
	if (this->isRunning())
	{
		auto& currentEvent = this->getCurrentEvent();

		// Have the current event update the scene.
		bool eventIsRunning = currentEvent.update(target, timeDelta);
		if (!eventIsRunning)
		{
			// If the current event has ended, then we'll 
			// move on to the next event.

			// Finalize this event.
			currentEvent.end(target);

			// Bump the current event index.
			this->currentEventIndex++;

			if (this->isRunning())
			{
				// Start the next event. This is
				// the new "current" event, because
				// we bumped the current event index.
				this->getCurrentEvent().start(target);
			}
		}
	}

	// Return a boolean value that tells the user
	// whether this timeline is still running or not.
	return this->isRunning();
}

/// Applies this timeline's finalization 
/// logic to the given scene: objects associated
/// with this event are removed from the scene.
/// This method can either be called to perform
/// cleanup after an event has ended, or 
/// to forcibly end the event. It should
/// be called exactly once for every event that
/// has been started.
void Timeline::end(Scene& target)
{
	// Only do something if the timeline
	// has not ended yet.
	if (this->isRunning())
	{
		// Perform cleanup for the currently executing event.
		this->getCurrentEvent().end(target);
		// End the timeline by updating the index.
		this->currentEventIndex = this->allEvents.size();
	}
}

/// Gets a boolean value that tells if this
/// timeline is running right now.
bool Timeline::isRunning() const
{
	return this->currentEventIndex < this->allEvents.size();
}

/// Gets the currently executing event, assuming 
/// that the timeline has not ended yet.
ITimelineEvent& Timeline::getCurrentEvent()
{
	return *this->allEvents.at(this->currentEventIndex);
}