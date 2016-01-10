#include "LoopedEvent.h"

#include "Common.h"
#include "ITimelineEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::timeline;

/// Creates a timeline event that loops the given inner event.
LoopedEvent::LoopedEvent(
    const si::timeline::ITimelineEvent_ptr& innerEvent,
    int maxIterationCount)
    : innerEvent(innerEvent), maxIterationCount(maxIterationCount)
{ }

/// Starts the timeline event.
void LoopedEvent::start(Scene& target)
{
    this->iterationCount = 0;
    this->innerEvent->start(target);
}

/// Has this timeline event update the given scene.
bool LoopedEvent::update(Scene& target, duration_t timeDelta)
{
    if (!this->innerEvent->update(target, timeDelta))
    {
        this->innerEvent->end(target);
        if (this->isInfiniteLoop())
        {
            this->innerEvent->start(target);
        }
        else
        {
            this->iterationCount++;
            if (this->maxIterationCount > this->iterationCount)
                this->innerEvent->start(target);
            else // Maximal number of iterations has been reached.
                return false;
        }
    }
    return true;
}

/// Applies this timeline event's finalization
/// logic to the given scene,
void LoopedEvent::end(Scene& target)
{
    this->innerEvent->end(target);
}

/// Checks if this event will loop forever,
bool LoopedEvent::isInfiniteLoop() const
{
    return this->maxIterationCount <= 0;
}

/// Gets the maximal number of loop iterations.
int LoopedEvent::getMaxIterationCount() const
{
    return this->maxIterationCount;
}
