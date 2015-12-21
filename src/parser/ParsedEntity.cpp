#include "ParsedEntity.h"

#include <memory>
#include "model/Entity.h"
#include "model/ShipEntity.h"
#include "model/DriftingEntity.h"
#include "controller/IController.h"
#include "view/IRenderable.h"
#include "timeline/InstantaneousEvent.h"
#include "Scene.h"

using namespace si;
using namespace si::parser;

/// Adds the given entity's model, view and
/// controllers to the given scene.
void si::parser::addToScene(
	const ParsedEntity<si::model::Entity>& entity,
	Scene& target)
{
	target.startEvent(entity.creationEvent);
}

/// Creates an event that adds the given vector
/// of controllers to the scene.
si::timeline::ITimelineEvent_ptr si::parser::createAddControllersEvent(
	const std::vector<si::controller::IController_ptr>& items)
{
	return std::make_shared<si::timeline::InstantaneousEvent>(
		[=](Scene& target) -> void
		{
			for (const auto& item : items)
			{
				target.addController(item);
			}
		});
}

/// Creates a bullet that is fired from the given source. 
/// Momentum is transferred from the source entity to
/// the projectile, but the bullet is not added to the
/// scene.
ParsedEntity<si::model::DriftingEntity> si::parser::fireProjectile(
	si::model::DriftingEntity& source,
	const ParsedDriftingEntityFactory& projectileFactory)
{
	// Create a new projectile.
	auto projectile = projectileFactory();

	auto sourcePhysProps = source.getPhysicsProperties();
	auto projPhysProps = projectile.model->getPhysicsProperties();

	// Compute the projectile's position and velocity,
	// relative to its source.
	auto veloc = vecLength(projectile.model->getVelocity()) * source.getOrientation();
	auto bulletOffset = (sourcePhysProps.radius + projPhysProps.radius) * si::normalizeVec(veloc);

	// Set the projectile's position and velocity.
	projectile.model->setPosition(source.getPosition() + bulletOffset);
	projectile.model->setVelocity(source.getVelocity() + veloc);

	// Firing a projectile in space should make ships
	// accelerate in the opposite direction.
	// To preserve momentum, we'll
	// multiply that acceleration by the projectile's mass,
	// and divide it by the ship's.
	source.accelerate(-si::Vector2d(veloc * projPhysProps.mass / sourcePhysProps.mass));

	// We're done here. Adding the 
	// projectile to the scene is some
	// other function's problem.
	return projectile;
}

/// Creates a bullet that is fired from the given source. 
/// Momentum is transferred from the source entity to
/// the projectile, after which the projectile is added
/// to the scene.
void si::parser::fireAndAddProjectile(
	si::model::DriftingEntity& source,
	const ParsedDriftingEntityFactory& projectileFactory,
	Scene& target)
{
	auto entity = fireProjectile(source, projectileFactory);
	addToScene(entity, target);
}