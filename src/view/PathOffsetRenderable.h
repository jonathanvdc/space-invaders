#pragma once

#include <functional>
#include <memory>
#include "Common.h"
#include "IRenderable.h"
#include "RenderContext.h"
#include "Transformation.h"

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
			/// Creates a path offset renderable from the
			/// given contents and path.
			PathOffsetRenderable(
				const std::shared_ptr<IRenderable>& contents,
				const std::function<Vector2d()>& path);

			/// Renders this renderable object.
			void render(
				RenderContext& target, DoubleRect bounds,
				const Transformation& transform) final override;

			/// Gets this path offset renderable's contents.
			std::shared_ptr<IRenderable> getContents() const;

		private:
			const std::shared_ptr<IRenderable> contents;
			const std::function<Vector2d()> getPosition;
		};
	}
}
