#include "LoopedEvent.h"

#include "Common.h"
#include "ITimelineEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::timeline;

/// Creates a timeline event that loops the given inner event.
LoopedEvent::LoopedEvent(
    const si::timeline::ITimelineEvent_ptr& innerEvent)
    : innerEvent(innerEvent)
{ }

/// Starts the timeline event.
void LoopedEvent::start(Scene& target)
{
    this->innerEvent->start(target);
}

/// Has this timeline event update the given scene.
bool LoopedEvent::update(Scene& target, duration_t timeDelta)
{
    if (!this->innerEvent->update(target, timeDelta))
    {
        this->innerEvent->end(target);
        this->innerEvent->start(target);
    }
    return true;
}

/// Applies this timeline event's finalization
/// logic to the given scene,
void LoopedEvent::end(Scene& target)
{
    this->innerEvent->end(target);
}
