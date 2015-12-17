#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <tuple>
#include <SFML/Graphics.hpp>
#include "Common.h"
#include "IRenderable.h"
#include "Transformation.h"
#include "PathOffsetRenderable.h"

namespace si
{
	namespace view
	{
		/// Defines a renderable particle emitter effect.
		class ParticleEmitterRenderable final : public IRenderable
		{
		public:
            /// A type for functions that create particles.
            typedef std::function<IRenderable_ptr()> ParticleFactory;

			/// Creates a particle emitter renderable from the
			/// given particle factory, a duration that represents
            /// the interval at which new particles are created,
            /// and a duration that represents the lifetime of a
            /// single particle, after it has been created.
			ParticleEmitterRenderable(
				const ParticleFactory& factory, double particleVelocity,
                duration_t particleInterval, duration_t particleLifetime);

            /// Please don't try to copy this thing.
            ParticleEmitterRenderable(const ParticleEmitterRenderable&) = delete;

			/// Renders this renderable object on the
			/// given render context, within the given
			/// bounds, which is given in absolute
			/// coordinates.
			void render(
				RenderContext& target, DoubleRect bounds,
				const Transformation& transform) final override;
		private:
			/// Creates new particles. The amount of particles
            /// that are created depends on the amount of time that has passed
            /// since the last batch of particles was created.
			void createParticles();

			/// Updates the elapsed time, optionally removing timed
			/// out particles.
			void updateTime(duration_t delta);

			const ParticleFactory factory;
            const double particleSpeed;
			const duration_t particleInterval;
			const duration_t particleLifetime;
			duration_t elapsedTime;
			duration_t totalElapsedTime;
			std::deque<std::tuple<IRenderable_ptr, duration_t>> particles;
		};
	}
}
