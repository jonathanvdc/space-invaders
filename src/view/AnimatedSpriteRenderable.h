#pragma once

#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Common.h"
#include "RenderContext.h"
#include "IRenderable.h"
#include "Transformation.h"
#include "SpriteRenderable.h"

namespace si
{
	namespace view
	{
		/// Defines a renderable animated sprite object.
		class AnimatedSpriteRenderable final : public SpriteRenderableBase
		{
		public:
			/// Creates a renderable animated sprite from the given texture,
            /// an integer that specifies the number of frames in the
            /// texture, and a duration that defines the cycle length
            /// for this animated sprite.
			AnimatedSpriteRenderable(
                const std::shared_ptr<sf::Texture>& texture, int frames,
                duration_t cycleDuration);

        protected:
            /// Gets the rectangular area of the texture to render.
            sf::IntRect getTextureRectangle(duration_t timeDelta) const final override;

		private:
            int frames;
            duration_t cycleDuration;
		};
	}
}
