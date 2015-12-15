#include "ShowEvent.h"

#include "Common.h"
#include "parser/ParsedEntity.h"
#include "view/IRenderable.h"
#include "ITimelineEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::timeline;

/// Creates an event that adds a renderable
/// object to the view.
ShowEvent::ShowEvent(const si::parser::Factory<si::view::IRenderable_ptr>& factory)
	: factory(factory), renderable(nullptr)
{ }

/// Starts the timeline event.
void ShowEvent::start(Scene& target)
{
	// End this event if it has already been started.
	this->end(target);

	// Create a new renderable.
	this->renderable = this->factory();
	// Add the renderable to the scene.
	target.addRenderable(this->renderable);
}

/// Has this timeline event update the given scene.
bool ShowEvent::update(Scene& target, duration_t)
{
	// This event is running if the renderable it
	// created is still in the scene, if
	// a renderable has been created.
	if (this->renderable == nullptr)
		return false;
	else
		return target.getRenderer().contains(this->renderable);
}

/// Applies this timeline event's finalization
/// logic to the given scene.
void ShowEvent::end(Scene& target)
{
	if (this->renderable != nullptr)
	{
		// Remove the renderable from the scene.
		target.getRenderer().remove(this->renderable);

		// Set the renderable pointer to null.
		// (We don't want to retain it forever.)
		this->renderable = nullptr;
	}
}
