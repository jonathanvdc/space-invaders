#include "InvaderWaveEvent.h"

#include <set>
#include <vector>
#include "Common.h"
#include "model/Entity.h"
#include "model/Game.h"
#include "model/ProjectileEntity.h"
#include "view/IRenderable.h"
#include "controller/IController.h"
#include "controller/PathController.h"
#include "controller/IntervalActionController.h"
#include "parser/ParsedEntity.h"
#include "ITimelineEvent.h"
#include "Scene.h"
#include "ConcurrentEvent.h"
#include "SpawnEvent.h"
#include "RandomGenerator.h"

using namespace si;
using namespace si::timeline;

/// Creates an invader wave event from the given ship factory,
/// projectile factory, row count and column count.
InvaderWaveEvent::InvaderWaveEvent(
	const si::parser::ParsedEntityFactory<si::model::ShipEntity>& shipFactory,
	const si::parser::ParsedEntityFactory<si::model::ProjectileEntity>& projectileFactory,
	int rowCount, int columnCount, const InvaderBehavior& invaderBehavior)
	: shipFactory(shipFactory), projectileFactory(projectileFactory), 
	  rowCount(rowCount), columnCount(columnCount), 
	  invaderBehavior(invaderBehavior), ships()
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
			auto model = entity.model;

			const double pi = 3.1415;

			double velX = this->invaderBehavior.velocity.x;
			double velY = this->invaderBehavior.velocity.y;

			// Now compute the entity's initial position in the game.
			// We want to position ships like this, where the columns
			// are placed in the middle of the screen, and the nth row
			// is placed at the top of the screen:
			//
			//           | col 1 |  ...  | col n |
			//     ------|-----------------------|
			//     row n |       |       |       |
			//     ------|-----------------------|
			//     ...   |       |       |       |
			//     ------|-----------------------|
			//     row 1 |       |       |       |
			//
			double radius = model->getPhysicsProperties().radius;
			// Make sure the invader ships are positioned at safe distances, so
			// they don't crash into each other. Note: due to the nature of their
			// movements, inserting some empty space in the Y-direction is far
			// more important than inserting empty space in the X-direction.
			double spacingX = radius / 2.0 + 0.5 * pi * radius * velX;
			double spacingY = radius / 2.0 + 2.0 * pi * radius * velY;
			double totalX = radius * this->columnCount + spacingX * (this->columnCount - 1);
			double totalY = radius * this->rowCount + spacingY * (this->rowCount - 1);
			double offsetX = radius * i + spacingX * i;
			double offsetY = radius * j + spacingY * j;

			double posX = 0.5 - totalX / 2.0 + offsetX;
			double posY = totalY - offsetY;

			model->setPosition({ posX, posY });

			// Add the entity to the game, along with its
			// view and controllers.
			target.addTrackedEntity(model, entity.view);
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
			//
			// The X-axis component of their velocity will
			// determine how quickly they zig-zag.
			
			
			auto path = [=](duration_t time) -> Vector2d
			{
				return Vector2d(
					posX + radius * std::sin(velX * pi * time.count()), 
					posY + velY * time.count());
			};

			// A path controller will make the invader ship trace
			// the path function we just created.
			target.addController(std::make_shared<si::controller::PathController>(
				model, this->invaderBehavior.springConstant, path));

			auto projFactory = this->projectileFactory;

			// Let's get the invaders to fire some projectiles
			// at us by creating an interval action controller.
			target.addController(std::make_shared<si::controller::IntervalActionController>(
				this->invaderBehavior.fireInterval + 
					si::RandomGenerator::instance.nextReal<double>(-1.0, 1.0) * this->invaderBehavior.fireIntervalDeviation,
				[=](const si::model::Game& game, duration_t) -> bool
				{
					for (std::size_t k = 0; k < static_cast<std::size_t>(j); k++)
					{
						if (game.contains(column.at(k)))
							// Don't open fire if there is another invader 
							// in front of this ship.
							return false;
					}
					return true;
				}, [=, &target](si::model::Game&, duration_t) -> void
				{
					si::parser::fireAndAddProjectile(*model, projFactory, target);
				}, [=](const si::model::Game& game, duration_t) -> bool
				{
					return game.contains(model);
				}));

			column.push_back(model);
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