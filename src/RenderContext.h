#pragma once
#include <SFML/Graphics.hpp>
#include "Common.h"

namespace si
{
	namespace view
	{
		/// Encapsulates a render target,
		/// and provides helper functionality.
		class RenderContext
		{
		public:
			/// Creates a new render context from the given 
			/// render target.
			RenderContext(sf::RenderTarget& target);

			/// Gets this render context's render target.
			sf::RenderTarget& getTarget() const;

			/// Applies a viewport transformation to
			/// the given scalar by multiplying it
			/// with the geometric mean of the viewport
			/// size's coordinates. This can be used to
			/// scale things that have a uniform radius
			/// or size.
			double transformView(double size) const;

			/// Applies a viewport transformation to
			/// the given two-dimensional vector:
			/// (0..1, 0..1) is mapped to (0..x, 0..y),
			/// where x the render target's width,
			/// and y is its height.
			Vector2d transformView(Vector2d vec) const;

			/// Applies a viewport transformation to
			/// the given rectangle by applying said
			/// transformation to both its position
			/// and dimensions.
			DoubleRect transformView(DoubleRect vec) const;
		private:
			sf::RenderTarget& target;
		};
	}
}