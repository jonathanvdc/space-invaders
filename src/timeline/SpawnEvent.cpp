#include "SpawnEvent.h"

#include <memory>
#include "Common.h"
#include "model/Entity.h"
#include "parser/ParsedEntity.h"
#include "ITimelineEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::timeline;

/// Creates a new entity-spawning event from the given
/// entity factory.
SpawnEvent::SpawnEvent(const si::parser::ParsedEntityFactory<si::model::Entity>& factory)
	: factory(factory), entity(nullptr)
{ }

/// Starts the timeline event.
void SpawnEvent::start(Scene& target)
{
	// End this event if that hasn't already been done.
	this->end(target);

	// Create a new entity, add it to the scene.
	this->entity = std::make_unique<si::parser::ParsedEntity<si::model::Entity>>(factory());
	target.addTrackedEntity(this->entity->model, this->entity->view);
	for (const auto& item : this->entity->controllers)
	{
		target.addController(item);
	}
}

/// Has this timeline event update the given scene.
bool SpawnEvent::update(Scene& target, duration_t timeDelta)
{
	if (this->entity != nullptr)
	{
		// The entity-spawning event is alive as long as the
		// entity it spawned is.
		return target.getGame().contains(this->entity->model);
	}
	else
	{
		return false;
	}
}

/// Applies this timeline event's finalization 
/// logic to the given scene.
void SpawnEvent::end(Scene& target)
{
	if (this->entity != nullptr)
	{
		// Remove the spawned entity from the scene.

		target.getGame().remove(this->entity->model);
		target.getRenderer().remove(this->entity->view);
		for (const auto& item : this->entity->controllers)
		{
			target.getController().remove(item);
		}

		this->entity = nullptr;
	}
}