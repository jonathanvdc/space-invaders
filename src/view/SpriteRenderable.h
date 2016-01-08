#pragma once

#include <functional>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Common.h"
#include "RenderContext.h"
#include "IRenderable.h"
#include "Transformation.h"

namespace si
{
	namespace view
	{
		/// A base class for renderable sprite objects.
		class SpriteRenderableBase : public IRenderable
		{
		public:
			/// Creates a renderable sprite from the given texture.
			SpriteRenderableBase(const std::shared_ptr<sf::Texture>& texture);

			/// Renders this sprite.
			void render(
				RenderContext& target, DoubleRect bounds,
				const Transformation& transform) final override;

			/// Gets the texture this sprite renderable uses.
			std::shared_ptr<sf::Texture> getTexture() const;
		protected:
			/// Gets the rectangular area of the texture to render.
			virtual sf::IntRect getTextureRectangle(duration_t timeDelta) const = 0;
		private:
			std::shared_ptr<sf::Texture> texture;
		};

		/// Defines a renderable sprite object, that draws the entire texture.
		class SpriteRenderable final : public SpriteRenderableBase
		{
		public:
			/// Creates a renderable sprite from the given texture.
			SpriteRenderable(const std::shared_ptr<sf::Texture>& texture);

		protected:
			/// Gets the rectangular area of the texture to render.
			sf::IntRect getTextureRectangle(duration_t timeDelta) const final override;
		};
	}
}
