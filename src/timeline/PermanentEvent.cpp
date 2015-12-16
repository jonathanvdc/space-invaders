#include "PermanentEvent.h"

#include "Common.h"
#include "ITimelineEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::timeline;

/// Creates a permanent event from the given
/// inner event.
PermanentEvent::PermanentEvent(
    const si::timeline::ITimelineEvent_ptr& innerEvent)
    : innerEvent(innerEvent)
{ }

/// Starts the timeline event.
void PermanentEvent::start(Scene& target)
{
    this->innerEvent->start(target);
}

/// Has this timeline event update the given scene.
bool PermanentEvent::update(Scene&, duration_t)
{
    // This event is technically "over" from the get-go.
    return false;
}

/// Applies this timeline event's finalization
/// logic to the given scene,
void PermanentEvent::end(Scene&)
{
    // Do nothing here, and let the
    // child event live forever.
}
