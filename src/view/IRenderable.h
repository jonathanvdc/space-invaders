#pragma once

#include <memory>
#include "Common.h"
#include "RenderContext.h"
#include "Transformation.h"

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
			/// coordinates. A transformation is also
			/// given.
			virtual void render(
				RenderContext& target, DoubleRect bounds, 
				const Transformation& transform) = 0;
		};

		typedef std::shared_ptr<IRenderable> IRenderable_ptr;
	}
}