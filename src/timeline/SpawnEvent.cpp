#include "SpawnEvent.h"

#include <memory>
#include "Common.h"
#include "model/Entity.h"
#include "ITimelineEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::timeline;

/// Creates a new entity-spawning event from the given
/// entity and view.
SpawnEvent::SpawnEvent(
	const si::model::Entity_ptr& entityModel,
	const si::view::IRenderable_ptr& entityView)
	: entityModel(entityModel), entityView(entityView)
{ }

/// Starts the timeline event.
void SpawnEvent::start(Scene& target)
{
	// End this event if that hasn't already been done.
	this->end(target);

	// Add the entity to the scene.
	target.addEntity(this->entityModel, this->entityView);
}

/// Has this timeline event update the given scene.
bool SpawnEvent::update(Scene& target, duration_t)
{
	// The entity-spawning event is alive as long as the
	// entity it spawned is.
	return target.getGame().contains(this->entityModel);
}

/// Applies this timeline event's finalization
/// logic to the given scene.
void SpawnEvent::end(Scene& target)
{
	// Remove the spawned entity from the scene.

	target.getGame().remove(this->entityModel);

	// No need to remove the view: the scene does
	// that for us.
}
