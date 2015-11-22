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
			/// Renders this renderable object.
			virtual void render(RenderContext& target) const = 0;
		};
	}
}