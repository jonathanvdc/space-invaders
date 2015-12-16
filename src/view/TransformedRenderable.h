#pragma once

#include <functional>
#include <memory>
#include <utility>
#include "Common.h"
#include "IRenderable.h"
#include "RenderContext.h"
#include "Transformation.h"

namespace si
{
	namespace view
	{
		/// Defines a renderable object that
		/// draws its child renderable, after
		/// replacing the given transformation
		/// by its own transformation, which is
		/// provided by a transformation function.
		class TransformedRenderable final : public IRenderable
		{
		public:
			/// Creates a transformed renderable from the
			/// given contents and transformation function.
			TransformedRenderable(
				const std::shared_ptr<IRenderable>& contents,
				const std::function<std::pair<Transformation, DoubleRect>(DoubleRect)>& getTransformation);

			/// Renders this transformed renderable object.
			void render(
				RenderContext& target, DoubleRect bounds,
				const Transformation& transform) final override;

			/// Gets this transformed renderable's contents.
			std::shared_ptr<IRenderable> getContents() const;

		private:
			const std::shared_ptr<IRenderable> contents;
			const std::function<std::pair<Transformation, DoubleRect>(DoubleRect)> getTransformation;
		};
	}
}
