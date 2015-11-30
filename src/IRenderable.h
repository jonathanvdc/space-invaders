#pragma once
#include "Common.h"
#include "RenderContext.h"

namespace si
{
	namespace view
	{
		/// Defines a renderable object. This is the absolute base
		/// class for view objects.
		class IRenderable
		{
		public:
			virtual ~IRenderable() { }

			/// Renders this renderable object on the 
			/// given render context, within the given
			/// bounds, which is given in absolute 
			/// coordinates.
			virtual void render(RenderContext& target, DoubleRect bounds) = 0;
		};
	}
}