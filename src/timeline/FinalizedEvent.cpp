#include "FinalizedEvent.h"

#include <memory>
#include "Common.h"
#include "ITimelineEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::timeline;

/// Creates a finalizing event from the given inner event
/// and finalization function.
FinalizedEvent::FinalizedEvent(
    const ITimelineEvent_ptr& event,
    const SceneAction& finalize)
	: isRunning(false), innerEvent(event), 
	  finalize(finalize)
{ }

/// Starts the timeline event.
void FinalizedEvent::start(Scene& target)
{
	// End this event if that hasn't already been done.
	this->end(target);

	// Start the inner event.
    this->innerEvent->start(target);

    this->isRunning = true;
}

/// Has this timeline event update the given scene.
bool FinalizedEvent::update(Scene& target, duration_t timeDelta)
{
	if (this->isRunning)
    {
        return this->innerEvent->update(target, timeDelta);
    }
	else
	{
		return false;
	}
}

/// Applies this timeline event's finalization
/// logic to the given scene.
void FinalizedEvent::end(Scene& target)
{
	if (this->isRunning)
    {
        this->innerEvent->end(target);
        this->finalize(target);
        this->isRunning = false;
    }
}

/// Creates an event that performs an event, and then
/// applies finalization logic.
std::shared_ptr<FinalizedEvent> si::timeline::finalize(
	const ITimelineEvent_ptr& event,
	const FinalizedEvent::SceneAction& finalization)
{
	return std::make_shared<FinalizedEvent>(event, finalization);
}