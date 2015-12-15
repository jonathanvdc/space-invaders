#include "OutOfBoundsController.h"

#include "Common.h"
#include "model/Entity.h"
#include "IController.h"

using namespace si;
using namespace si::controller;

OutOfBoundsController::OutOfBoundsController(
	const si::model::Entity_ptr& entity,
	si::DoubleRect bounds)
	: entity(entity), bounds(bounds)
{ }

/// Checks if this controller is still "alive".
/// A live controller will remain in the controller
/// list, whereas dead controllers will be eliminated.
bool OutOfBoundsController::isAlive() const
{
	return this->entity != nullptr;
}

/// Updates the game model based on the given time delta.
void OutOfBoundsController::update(si::model::Game& game, duration_t)
{
	if (this->isAlive() && !bounds.contains(this->entity->getPosition()))
	{
		// If this out-of-bounds controller is still
		// alive, and the entity it manages is out-of-bounds,
		// then we'll remove the entity from the game,
		// and mark this controller as dead, by
		// setting the entity pointer to null.
		game.remove(this->entity);
		this->entity = nullptr;
	}
}
