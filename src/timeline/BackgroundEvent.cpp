#include "BackgroundEvent.h"

#include "Common.h"
#include "ITimelineEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::timeline;

/// Creates a timeline event from the given main
/// and background events.
BackgroundEvent::BackgroundEvent(
    const si::timeline::ITimelineEvent_ptr& mainEvent,
    const si::timeline::ITimelineEvent_ptr& backgroundEvent)
    : mainEvent(mainEvent), backgroundEvent(backgroundEvent)
{ }

/// Starts the timeline event.
void BackgroundEvent::start(Scene& target)
{
    this->mainEvent->start(target);
    this->backgroundEvent->start(target);
}

/// Has this timeline event update the given scene.
bool BackgroundEvent::update(Scene& target, duration_t timeDelta)
{
    bool result = this->mainEvent->update(target, timeDelta);
    this->backgroundEvent->update(target, timeDelta);
    return result;
}

/// Applies this timeline event's finalization
/// logic to the given scene,
void BackgroundEvent::end(Scene& target)
{
    this->mainEvent->end(target);
    this->backgroundEvent->end(target);
}
