#include "ParticleEmitterRenderable.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <deque>
#include <functional>
#include <tuple>
#include <SFML/Graphics.hpp>
#include "Common.h"
#include "IRenderable.h"
#include "Transformation.h"
#include "PathOffsetRenderable.h"
#include "RandomGenerator.h"

using namespace si;
using namespace si::view;
using namespace std::chrono_literals;

/// Creates a particle emitter renderable from the
/// given particle factory, a duration that represents
/// the interval at which new particles are created,
/// and a duration that represents the lifetime of a
/// single particle, after it has been created.
ParticleEmitterRenderable::ParticleEmitterRenderable(
    const ParticleFactory& factory, double particleSpeed,
    duration_t particleInterval, duration_t particleLifetime)
    : factory(factory), particleSpeed(particleSpeed),
      particleInterval(particleInterval), particleLifetime(particleLifetime),
      elapsedTime(0.0s), totalElapsedTime(0.0s), particles()
{ }

/// Renders this renderable object on the
/// given render context, within the given
/// bounds, which is given in absolute
/// coordinates.
void ParticleEmitterRenderable::render(
	RenderContext& target, DoubleRect bounds,
	const Transformation& transform)
{
	// Update elapsed time, particle list
	this->updateTime(target.getTimeDelta());
	// Create a new batch of particles
	this->createParticles();

	for (const auto& item : this->particles)
    {
        const auto& renderable = std::get<0>(item);
        renderable->render(target, bounds, transform);
    }
}

/// Creates new particles, given rectangular
/// bounds and a transformation. The amount of particles
/// that are created depends on the amount of time that has passed
/// since the last batch of particles was created.
void ParticleEmitterRenderable::createParticles()
{
	if (this->elapsedTime > this->particleInterval)
	{
        // Compute the number of particles we can create.
        double frac = this->elapsedTime / this->particleInterval;
        int amount = static_cast<int>(frac);
        // Subtract the amount of time we "used up" by creating these particles
        // from the elapsed time.
        duration_t rem = this->elapsedTime - amount * this->particleInterval;

        for (int i = 0; i < amount; i++)
        {
            // Create a new particle.
            auto renderable = this->factory();

            auto& rng = RandomGenerator::instance;

            // Pick a random direction.
            Vector2d dir(
                rng.nextReal<double>(-1.0, 1.0),
                rng.nextReal<double>(-1.0, 1.0));

            // Normalize it, multiply it by the particle speed.
            Vector2d vel = this->particleSpeed * normalizeVec(dir);
            duration_t currentTime = this->totalElapsedTime;

            // This makes copying particle emitters impossible. Fortunately,
            // the copy constructor has been deleted.
            auto path = [this, vel, currentTime]() -> Vector2d
            {
                return (this->totalElapsedTime - currentTime).count() * vel;
            };

            auto pathRenderable = std::make_shared<PathOffsetRenderable>(renderable, path);

            this->particles.push_back(std::make_tuple(pathRenderable, this->totalElapsedTime));
        }

		// Set the elapsed time to the remaining time.
		this->elapsedTime = rem;
	}
}

/// Updates the elapsed time, optionally removing timed
/// out previous positions.
void ParticleEmitterRenderable::updateTime(duration_t delta)
{
	// Update the (total) elapsed time.
	this->totalElapsedTime += delta;
	this->elapsedTime += delta;

	auto totalTime = this->totalElapsedTime;
	auto lifetime = this->particleLifetime;

	// Remove old particles.
	this->particles.erase(std::remove_if(this->particles.begin(), this->particles.end(),
		[=](const std::tuple<IRenderable_ptr, duration_t>& item) -> bool
		{
			return totalTime - std::get<1>(item) > lifetime;
		}), this->particles.end());
}
