#pragma once

#include <functional>
#include <memory>
#include "Common.h"
#include "IRenderable.h"
#include "RenderContext.h"

namespace si
{
	namespace view
	{
		/// Defines a renderable object that 
		/// draws its child renderable in
		/// a box equivalent to the enclosing box
		/// displaced by a certain offset,
		/// which is given by a path function.
		class PathOffsetRenderable final : public IRenderable
		{
		public:
			/// Creates a renderable relative box from the
			/// given contents and relative box.
			PathOffsetRenderable(const std::shared_ptr<IRenderable>& contents, const std::function<Vector2d()>& path);

			/// Renders the relative box renderable's child
			/// within a relative box in the given outer bounds.
			void render(RenderContext& target, DoubleRect bounds) final override;

			/// Gets this renderable relative box' contents.
			std::shared_ptr<IRenderable> getContents() const;

		private:
			std::function<Vector2d()> getPosition;
			std::shared_ptr<IRenderable> contents;
		};
	}
}