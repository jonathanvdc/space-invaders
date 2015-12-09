#pragma once

#include <vector>
#include "IRenderable.h"
#include "RenderContext.h"

namespace si
{
	namespace view
	{
		/// Defines a class of renderable objects
		/// that render a number of other renderables.
		class GroupRenderable final : public IRenderable
		{
		public:
			/// Creates a new empty group renderable.
			GroupRenderable();
			/// Creates a group renderable from the given
			/// vector of child renderables.
			GroupRenderable(const std::vector<IRenderable_ptr>& children);
			/// Creates a group renderable from the given
			/// vector of child renderables. Move semantics
			/// are used for efficiency.
			GroupRenderable(std::vector<IRenderable_ptr>&& children);

			/// Renders this renderable object on the 
			/// given render context, within the given
			/// bounds, which is given in absolute 
			/// coordinates.
			void render(RenderContext& target, DoubleRect bounds) final override;
		private:
			std::vector<IRenderable_ptr> children;
		};
	}
}