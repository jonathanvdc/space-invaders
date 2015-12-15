#pragma once

#include <memory>
#include "Common.h"
#include "IRenderable.h"
#include "RenderContext.h"

namespace si
{
	namespace view
	{
		/// Defines a renderable object that
		/// draws its child renderable in a
		/// box that is relative to the
		/// containing box.
		class RelativeBoxRenderable final : public IRenderable
		{
		public:
			/// Creates a renderable relative box from the
			/// given contents and relative box.
			RelativeBoxRenderable(const IRenderable_ptr& contents, DoubleRect box);

			/// Renders the relative box renderable's child
			/// within a relative box in the given outer bounds.
			void render(RenderContext& target, DoubleRect bounds) final override;

			/// Gets this renderable relative box' contents.
			std::shared_ptr<IRenderable> getContents() const;

			/// Gets the relative box that is used to
			/// render contents in.
			DoubleRect getRelativeBox() const;

		private:
			const IRenderable_ptr contents;
			const DoubleRect box;
		};
	}
}
