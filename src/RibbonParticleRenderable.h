#pragma once

#include <cstddef>
#include <deque>
#include <tuple>
#include <SFML/Graphics.hpp>
#include "Common.h"
#include "IRenderable.h"

namespace si
{
	namespace view
	{
		/// Defines a renderable ribbon particle effect.
		class RibbonParticleRenderable final : public IRenderable
		{
		public:
			/// Creates a new ribbon particle renderable from the
			/// given color, a duration that represents the interval 
			/// at which new points are logged, and a duration
			/// that represents the lifetime of a single point,
			/// after it has been logged. An epsilon
			/// value specifies that points will only be logged
			/// if they are farther apart than this value.
			RibbonParticleRenderable(
				sf::Color color, duration_t pointInterval, 
				duration_t pointLifetime);

			/// Renders this renderable object on the 
			/// given render context, within the given
			/// bounds, which is given in absolute 
			/// coordinates.
			void render(RenderContext& target, DoubleRect bounds) final override;
		private:
			/// Logs the current position, given rectangular
			/// bounds.
			void logPosition(DoubleRect bounds);

			/// Updates the elapsed time, optionally removing timed
			/// out previous positions.
			void updateTime(duration_t delta);

			/// Gets the polygon points for the position at
			/// the given index. The deque of points
			/// is assumed to be nonempty.
			std::tuple<Vector2d, Vector2d> getPositions(std::size_t index);

			const sf::Color color;
			const duration_t pointInterval;
			const duration_t pointLifetime;
			duration_t elapsedTime;
			duration_t totalElapsedTime;
			std::deque<std::tuple<Vector2d, double, duration_t>> prevPositions;
		};
	}
}