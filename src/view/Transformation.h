#pragma once

#include "Common.h"
#include <SFML/Graphics.hpp>

namespace si
{
	namespace view
	{
		/// Defines a two-dimensional transformation.
		class Transformation
		{
		public:
			/// Creates an empty transformation.
			Transformation();

			/// Creates a transformation from the given
			/// 3x3 matrix.
			Transformation(const sf::Transform& matrix);

			/// Modifies this transformation such that the given
			/// transformation is applied after this transformation
			/// has been completed.
			Transformation& compose(const Transformation& other);

			/// Creates a 3x3 transformation matrix from this
			/// transformation.
			sf::Transform toMatrix() const;	
			
			/// Creates a render state from this transformation.
			sf::RenderStates toRenderState() const;

			/// Transforms the given point.
			Vector2d transformPoint(Vector2d point) const;

			/// The identity transformation.
			static Transformation identity;

			/// Stores a pi constant.
			static const double pi;

			/// Creates a scaling transformation from the 
			/// given scaling vector, and an origin vector.
			static Transformation scale(Vector2d factor, Vector2d origin);

			/// Creates a rotation transformation from the given
			/// angle (in radians), and the given origin.
			static Transformation rotate(double angle, Vector2d origin);

			/// Creates a translation transformation from 
			/// the given offset.
			static Transformation translate(Vector2d offset);

		private:
			sf::Transform matrix;
		};
	}
}

