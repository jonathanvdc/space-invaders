#include "InvaderWaveEvent.h"

#include <set>
#include <vector>
#include "Common.h"
#include "Entity.h"
#include "IRenderable.h"
#include "IController.h"
#include "PathController.h"
#include "ITimelineEvent.h"
#include "Scene.h"
#include "ParsedEntity.h"
#include "ProjectileEntity.h"
#include "ConcurrentEvent.h"
#include "SpawnEvent.h"

using namespace si;
using namespace si::timeline;

/// Creates an invader wave event from the given ship factory,
/// projectile factory, row count and column count.
InvaderWaveEvent::InvaderWaveEvent(
	const si::parser::ParsedEntityFactory<si::model::ShipEntity>& shipFactory,
	const si::parser::ParsedEntityFactory<si::model::ProjectileEntity>& projectileFactory,
	int rowCount, int columnCount, Vector2d invaderVelocity, double invaderSpringConstant)
	: shipFactory(shipFactory), projectileFactory(projectileFactory), 
	  rowCount(rowCount), columnCount(columnCount), 
	  invaderVelocity(invaderVelocity), 
	  invaderSpringConstant(invaderSpringConstant), ships()
{ }

/// Starts the timeline event.
void InvaderWaveEvent::start(Scene& target)
{
	// End this event if it was already running.
	this->end(target);

	// Create ships
	for (int i = 0; i < this->columnCount; i++)
	{
		std::vector<std::shared_ptr<si::model::ShipEntity>> column;
		for (int j = 0; j < this->rowCount; j++)
		{
			// Create a new ship entity.
			auto entity = this->shipFactory();

			const double pi = 3.1415;

			// Now compute the entity's initial position in the game.
			// We want to position ships like this, where the columns
			// are placed in the middle of the screen, and the nth row
			// is placed at the top of the screen:
			//
			//           | col 1 | ... | col n |
			//     ------|---------------------|
			//     row n |       |     |       |
			//     ------|---------------------|
			//     ...   |       |     |       |
			//     ------|---------------------|
			//     row 1 |       |     |       |
			//
			double radius = entity.model->getPhysicsProperties().radius;
			// Make sure the invader ships are positioned at safe distances, so
			// they don't crash into each other. Note: due to the nature of their
			// movements, inserting some empty space in the Y-direction is far
			// more important than inserting empty space in the X-direction.
			double spacingX = radius / 2.0 + 0.5 * radius * pi * this->invaderVelocity.x;
			double spacingY = radius / 2.0 + 2.0 * radius * pi * this->invaderVelocity.y;
			double totalX = radius * this->rowCount + spacingX * (this->rowCount - 1);
			double totalY = radius * this->columnCount + spacingY * (this->columnCount - 1);
			double offsetX = radius * j + spacingX * j;
			double offsetY = radius * i + spacingY * i;

			double posX = 0.5 - totalX / 2.0 + offsetX;
			double posY = totalY - offsetY;

			entity.model->setPosition({ posX, posY });

			// Add the entity to the game, along with its
			// view and controllers.
			target.addTrackedEntity(entity.model, entity.view);
			for (const auto& item : entity.controllers)
			{
				target.addController(item);
			}

			// Now sketch out a path for the ships to follow.
			// We'll have them follow a gentle sine curve, by
			// attaching them to a critically dampened spring
			// controller.
			//
			// We'll use the Y-axis component of their
			// velocity to determine how quickly they move
			// toward the bottom of the screen.
			double velY = this->invaderVelocity.y;
			// The X-axis component of their velocity will
			// determine how quickly they zig-zag.
			double velX = this->invaderVelocity.x;
			
			auto path = [=](duration_t time) -> Vector2d
			{
				return Vector2d(posX + radius * std::sin(velX * pi * time.count()), posY + velY * time.count());
			};

			target.addController(std::make_shared<si::controller::PathController>(entity.model, this->invaderSpringConstant, path));

			column.push_back(entity.model);
		}
		this->ships.push_back(column);
	}
}

/// Has this timeline event update the given scene.
bool InvaderWaveEvent::update(Scene& target, duration_t timeDelta)
{
	return this->isRunning(target.getGame());
}

/// Applies this timeline event's finalization 
/// logic to the given scene.
void InvaderWaveEvent::end(Scene& target)
{
	// Remove all ships from the scene.
	for (const auto& col : this->ships)
	{
		for (const auto& item : col)
		{
			// Remove the entity itself (the model)
			// from the scene.
			// Note: we don't have to remove the 
			//       view or controllers here.
			//       The scene will do that for us.
			target.getGame().remove(item);
		}
	}
	
	// Clear the ships vector, to make sure
	// we don't hang on to the chunks of 
	// memory occupied by the ships.
	this->ships.clear();
}

/// Checks if this event is still running.
bool InvaderWaveEvent::isRunning(const si::model::Game& game) const
{
	// The event is still running as long as there is
	// at least one invader left.
	for (const auto& col : this->ships)
	{
		for (const auto& item : col)
		{
			if (game.contains(item))
				return true;
		}
	}
	return false;
}