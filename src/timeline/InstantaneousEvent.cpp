#include "InstantaneousEvent.h"

#include "Common.h"
#include "ITimelineEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::timeline;

/// Creates an instananeous event from the given
/// action function.
InstantaneousEvent::InstantaneousEvent(
	const SceneAction& action)
	: action(action)
{ }

/// Starts the timeline event.
void InstantaneousEvent::start(Scene& target)
{
	this->action(target);
}

/// Has this timeline event update the given scene.
bool InstantaneousEvent::update(Scene&, duration_t)
{
	// This event is technically "over" from the get-go.
	return false;
}

/// Applies this timeline event's finalization
/// logic to the given scene,
void InstantaneousEvent::end(Scene&)
{
	// Do nothing here.
}
