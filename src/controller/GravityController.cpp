#include "GravityController.h"

#include <cmath>
#include <functional>
#include "Common.h"
#include "model/Entity.h"
#include "model/ShipEntity.h"
#include "IController.h"

using namespace si;
using namespace si::controller;

/// Creates a gravity controller from the given gravity well
/// entity, as well as a gravitation constant that determines
/// how much drifting entities are attracted to the target entity.
GravityController::GravityController(
    const std::shared_ptr<si::model::PhysicsEntity>& target,
    double gravitationalConstant)
	: target(target), gravitationalConstant(gravitationalConstant)
{ }

/// Checks if this controller is still "alive".
/// A live controller will remain in the controller
/// list, whereas dead controllers will be eliminated.
bool GravityController::isAlive() const
{
	return this->target != nullptr;
}

/// Updates the game model based on the given time delta.
void GravityController::update(si::model::Game& game, duration_t timeDelta)
{
    if (this->target == nullptr)
    {
        return;
    }
    else if (!game.contains(this->target))
    {
        this->target = nullptr;
        return;
    }

    // Start by asking the game to fetch a list of all drifting entities.
    auto entities = game.getAll<si::model::DriftingEntity>();

    auto pos = this->target->getPosition();

    // Compute G * m1, where m1 is the mass of the gravity well.
    auto factor = this->gravitationalConstant * this->target->getPhysicsProperties().mass;

    for (const auto& other : entities)
    {
        auto offset = pos - other->getPosition();
        if (offset.x != 0.0 && offset.y != 0.0)
        {
            // Don't accelerate entities that are at the exact same position
            // as the tracked entity. The inverse-square law will result in a
            // division by zero, which is somewhat undesirable.

            // Since the scene consists of medium-sized objects, we'll apply
            // Newton's law of universal gravitation, which states that
            //
            //   F = G * m1 * m2 / d^2
            //
            // Let's assume that m1 is the mass of the (constant) entity belonging
            // to this controller, and m2 is the mass of any drifting entity.
            // Since we'll accelerate the latter, the second law of motion states:
            //
            //     F = m2 * a
            // <=> a = F / m2
            // <=> a = G * m1 / d^2
            //
            // Essentially, the mass of the other entity does not matter.
            // We can compute its acceleration purely based on the mass of the
            // gravity well, the gravitational constant, and the distance between
            // the gravity well and the attracted entity.
            //
            // The acceleration vector is directed toward the gravity well.
            // Thus, we can state that:
            //
            // (vector) a = normalize(offset) * (G * m1 / d^2)
            // <=>      a = offset / length(offset) * (G * m1 / length(offset)^2)
            // <=>      a = offset * (G * m1 / length(offset)^3)

            double dist = vecLength(offset);
            Vector2d accel = offset * (factor / (dist * dist * dist));

            // Now accelerate the drifting entity.
            other->accelerate(accel * timeDelta.count());
        }
    }
}
